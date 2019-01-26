/** @file common.h
 * @brief Int to chars conversion
 * @author Volodin Sergei, EPFL
 * @date October 2018
 */

#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>
#include "inmemorylog.h"
#include <sstream>
#include <chrono>
#include <iostream>
#include <iterator>
#include <algorithm>

using std::stringstream;
using std::chrono::steady_clock;

/** @macro minimal of two numbers */
#define min(x, y) ((x) > (y) ? (y) : (x))

/** @macro minimal of two numbers */
#define max(x, y) ((x) < (y) ? (y) : (x))

/// @macro Enable localized causal broadcast
#define LOCALIZED_CAUSAL_BROADCAST

/// @macro Enable tests for main()
//#define DEBUG_TEST 1

/// @macro Output debug files (.timestamps and .recvall)
//#define DEBUG_FILES 1

/// @macro Message = 1000 * sender + payload
//#define UNIQUE_MESSAGES 1

/** @macro Print messages stored in memory */
//#define INMEMORY_PRINT 1

/// @macro Dump to a separate file immediately
//#define IMMEDIATE_FILE 1

/// @macro Debug perfectlink
//#define PERFECTLINK_DEBUG

/// @macro Debug UDP messages
//#define UDP_DEBUG

/// @macro Debug BestEffortBroadcast
//#define BEB_DEBUG

/// @macro Debug UniformReliableBroadcast
//#define URB_DEBUG

/// @macro Debug UniformReliableBroadcast
//#define LCB_DEBUG

/// @macro Debug Failure Detector
//#define FAILUREDETECTOR_DEBUG

/// @macro Delay after each UDPSender::send
//#define UDPSENDER_DELAY_MS 0.5

/**
 * @brief int32ToChars Convert unsigned int32 to chars
 * @param input int
 * @param dest chars
 */
void int32ToChars(uint32_t input, char* dest);

/**
 * @brief charsToInt32 Convert chars to uint32
 * @param input chars
 * @return uint32
 */
uint32_t charsToInt32(const char* input);

void prettyprint(uint32_t* arr, unsigned length);


/** @brief Maximal buffer size
 * @see https://en.wikipedia.org/wiki/User_Datagram_Protocol */
const unsigned MAXLEN = 65000;

/**
 * @brief memorylog In Memory Log
 */
extern InMemoryLog* memorylog;


/// @macro Current time in milliseconds
#define TIME_MS_NOW() (std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch()).count())

#endif // COMMON_H
