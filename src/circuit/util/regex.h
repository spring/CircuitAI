/*
 * regex.h
 *
 *  Created on: Jun 9, 2016
 *      Author: rlcevg
 */

#ifndef SRC_CIRCUIT_UTIL_REGEX_H_
#define SRC_CIRCUIT_UTIL_REGEX_H_

//#include <regex>
#include <boost/regex.hpp>
#define REGEX boost::regex
#define REGEX_SEARCH boost::regex_search
#define SMATCH boost::smatch
#define CMATCH boost::cmatch
#define SREGEX_TOKEN_ITERATOR boost::sregex_token_iterator

#endif // SRC_CIRCUIT_UTIL_REGEX_H_
