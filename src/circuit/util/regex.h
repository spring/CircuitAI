/*
 * regex.h
 *
 *  Created on: Jun 9, 2016
 *      Author: rlcevg
 */

#ifndef SRC_CIRCUIT_UTIL_REGEX_H_
#define SRC_CIRCUIT_UTIL_REGEX_H_

#ifdef HAVE_CXX_11_REGEX
	#include <regex>
	#define REGEX std::regex
	#define REGEX_SEARCH std::regex_search
	#define SMATCH std::smatch
	#define CMATCH std::cmatch
	#define SREGEX_TOKEN_ITERATOR std::sregex_token_iterator
#else // HAVE_CXX_11_REGEX
	#include <boost/regex.hpp>
	#define REGEX boost::regex
	#define REGEX_SEARCH boost::regex_search
	#define SMATCH boost::smatch
	#define CMATCH boost::cmatch
	#define SREGEX_TOKEN_ITERATOR boost::sregex_token_iterator
#endif // HAVE_CXX_11_REGEX

#endif // SRC_CIRCUIT_UTIL_REGEX_H_
