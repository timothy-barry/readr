#include "cpp11/R.hpp"
#include "cpp11/list.hpp"
#include "cpp11/sexp.hpp"

#include "Collector.h"
#include "LocaleInfo.h"
#include "Source.h"
#include "Tokenizer.h"
#include "TokenizerLine.h"
#include "Warnings.h"

#include <Rcpp.h>
using namespace Rcpp;

[[cpp11::export]] IntegerVector
dim_tokens_(cpp11::list sourceSpec, cpp11::list tokenizerSpec) {
  SourcePtr source = Source::create(sourceSpec);
  TokenizerPtr tokenizer = Tokenizer::create(tokenizerSpec);
  tokenizer->tokenize(source->begin(), source->end());

  int rows = -1, cols = -1;

  for (Token t = tokenizer->nextToken(); t.type() != TOKEN_EOF;
       t = tokenizer->nextToken()) {
    rows = t.row();

    if ((int)t.col() > cols)
      cols = t.col();
  }

  return IntegerVector::create(rows + 1, cols + 1);
}

[[cpp11::export]] std::vector<int>
count_fields_(cpp11::list sourceSpec, cpp11::list tokenizerSpec, int n_max) {
  SourcePtr source = Source::create(sourceSpec);
  TokenizerPtr tokenizer = Tokenizer::create(tokenizerSpec);
  tokenizer->tokenize(source->begin(), source->end());

  std::vector<int> fields;

  for (Token t = tokenizer->nextToken(); t.type() != TOKEN_EOF;
       t = tokenizer->nextToken()) {
    if (n_max > 0 && t.row() >= (size_t)n_max)
      break;

    if (t.row() >= fields.size()) {
      fields.resize(t.row() + 1);
    }

    fields[t.row()] = t.col() + 1;
  }

  return fields;
}

[[cpp11::export]] cpp11::list guess_header_(
    cpp11::list sourceSpec, cpp11::list tokenizerSpec, cpp11::list locale_) {
  Warnings warnings;
  LocaleInfo locale(locale_);
  SourcePtr source = Source::create(sourceSpec);
  TokenizerPtr tokenizer = Tokenizer::create(tokenizerSpec);
  tokenizer->tokenize(source->begin(), source->end());
  tokenizer->setWarnings(&warnings);

  CollectorCharacter out(&locale.encoder_);
  out.setWarnings(&warnings);
  Token t = tokenizer->nextToken();
  size_t row_num = t.row();

  for (; t.type() != TOKEN_EOF && t.row() == row_num;
       t = tokenizer->nextToken()) {
    if (t.col() >= (size_t)out.size()) {
      out.resize(t.col() + 1);
    }

    if (t.type() == TOKEN_STRING) {
      out.setValue(t.col(), t);
    }
  }

  using namespace cpp11::literals;
  return cpp11::writable::list(
      {"header"_nm = out.vector(), "skip"_nm = source->skippedRows() + 1});
}

[[cpp11::export]] SEXP
tokenize_(cpp11::list sourceSpec, cpp11::list tokenizerSpec, int n_max) {
  Warnings warnings;

  SourcePtr source = Source::create(sourceSpec);
  TokenizerPtr tokenizer = Tokenizer::create(tokenizerSpec);
  tokenizer->tokenize(source->begin(), source->end());
  tokenizer->setWarnings(&warnings);

  std::vector<std::vector<std::string>> rows;

  for (Token t = tokenizer->nextToken(); t.type() != TOKEN_EOF;
       t = tokenizer->nextToken()) {
    if (n_max > 0 && t.row() >= (size_t)n_max)
      break;

    if (t.row() >= rows.size()) {
      rows.resize(t.row() + 1);
    }

    std::vector<std::string>& row = rows[t.row()];
    if (t.col() >= row.size())
      row.resize(t.col() + 1);

    row[t.col()] = t.asString();
  }

  cpp11::sexp out = wrap(rows);
  return warnings.addAsAttribute(out);
}

[[cpp11::export]] SEXP parse_vector_(
    CharacterVector x,
    cpp11::list collectorSpec,
    cpp11::list locale_,
    const std::vector<std::string>& na,
    const bool trim_ws) {
  Warnings warnings;
  int n = x.size();

  LocaleInfo locale(locale_);

  boost::shared_ptr<Collector> col = Collector::create(collectorSpec, &locale);
  col->setWarnings(&warnings);
  col->resize(n);

  for (int i = 0; i < n; ++i) {
    Token t;
    if (x[i] == NA_STRING) {
      t = Token(TOKEN_MISSING, i, -1);
    } else {
      SEXP string = x[i];
      t = Token(CHAR(string), CHAR(string) + Rf_length(string), i, -1, false);
      if (trim_ws) {
        t.trim();
      }
      t.flagNA(na);
    }
    col->setValue(i, t);
  }

  return warnings.addAsAttribute(static_cast<SEXP>(col->vector()));
}
