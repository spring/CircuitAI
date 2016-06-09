/*
 * mutex.h
 *
 *  Created on: Jun 9, 2016
 *      Author: rlcevg
 */

#ifndef SRC_CIRCUIT_UTIL_MUTEX_H_
#define SRC_CIRCUIT_UTIL_MUTEX_H_

#ifdef __MINGW32__
	#include <boost/thread/mutex.hpp>
	#define MUTEX boost::mutex
	#define UNIQUE_LOCK boost::unique_lock
	#define LOCK_GUARD boost::lock_guard
#else
	#include <mutex>
	#define MUTEX std::mutex
	#define UNIQUE_LOCK std::unique_lock
	#define LOCK_GUARD std::lock_guard
#endif

#endif // SRC_CIRCUIT_UTIL_MUTEX_H_
