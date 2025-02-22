% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/collectors.R
\name{parse_atomic}
\alias{parse_logical}
\alias{parse_integer}
\alias{parse_double}
\alias{parse_character}
\alias{col_logical}
\alias{col_integer}
\alias{col_double}
\alias{col_character}
\title{Parse logicals, integers, and reals}
\usage{
parse_logical(x, na = c("", "NA"), locale = default_locale(), trim_ws = TRUE)

parse_integer(x, na = c("", "NA"), locale = default_locale(), trim_ws = TRUE)

parse_double(x, na = c("", "NA"), locale = default_locale(), trim_ws = TRUE)

parse_character(x, na = c("", "NA"), locale = default_locale(), trim_ws = TRUE)

col_logical()

col_integer()

col_double()

col_character()
}
\arguments{
\item{x}{Character vector of values to parse.}

\item{na}{Character vector of strings to interpret as missing values. Set this
option to \code{character()} to indicate no missing values.}

\item{locale}{The locale controls defaults that vary from place to place.
The default locale is US-centric (like R), but you can use
\code{\link[=locale]{locale()}} to create your own locale that controls things like
the default time zone, encoding, decimal mark, big mark, and day/month
names.}

\item{trim_ws}{Should leading and trailing whitespace (spaces and tabs) be trimmed from
each field before parsing it?}
}
\description{
Use \verb{parse_*()} if you have a character vector you want to parse. Use
\verb{col_*()} in conjunction with a \verb{read_*()} function to parse the
values as they're read in.
}
\examples{
parse_integer(c("1", "2", "3"))
parse_double(c("1", "2", "3.123"))
parse_number("$1,123,456.00")

# Use locale to override default decimal and grouping marks
es_MX <- locale("es", decimal_mark = ",")
parse_number("$1.123.456,00", locale = es_MX)

# Invalid values are replaced with missing values with a warning.
x <- c("1", "2", "3", "-")
parse_double(x)
# Or flag values as missing
parse_double(x, na = "-")
}
\seealso{
Other parsers: 
\code{\link{col_skip}()},
\code{\link{cols_condense}()},
\code{\link{cols}()},
\code{\link{parse_datetime}()},
\code{\link{parse_factor}()},
\code{\link{parse_guess}()},
\code{\link{parse_number}()},
\code{\link{parse_vector}()}
}
\concept{parsers}
