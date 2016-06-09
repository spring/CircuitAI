/*
 * thread.h
 *
 *  Created on: Jun 9, 2016
 *      Author: rlcevg
 */

#ifndef SRC_CIRCUIT_UTIL_THREAD_H_
#define SRC_CIRCUIT_UTIL_THREAD_H_

#ifdef __MINGW32__
	#include <boost/thread/thread.hpp>
	#define THIS_THREAD boost::this_thread
	#define THREAD boost::thread
#else
	#include <thread>
	#define THIS_THREAD std::this_thread
	#define THREAD std::thread
#endif

#endif // SRC_CIRCUIT_UTIL_THREAD_H_
