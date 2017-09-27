/*
 * MyDebug.h
 *
 *  Created on: 2017. 9. 20.
 *      Author: crimson_88@naver.com
 */

#ifndef _MYDEBUG_H_
#define _MYDEBUG_H_

//#define MYDEBUG

///> General Debug Macro
#if MYDEBUG
#define DBG_ERR(fmt, args...)	printf("[ERROR: %s() at \"%s\"::line(%d)] " fmt"\n", __FUNCTION__, __FILE__, __LINE__, ## args);
#define DBG_WRN(fmt, args...)	printf("[WARNING: %s() at \"%s\"::line(%d)] " fmt"\n", __FUNCTION__, __FILE__, __LINE__, ## args);
#define DBG_INFO(fmt, args...)	printf("[INFO: %s() at \"%s\"::line(%d)] " fmt"\n", __FUNCTION__, __FILE__, __LINE__, ## args);
#define DBG_TRACE(fmt, args...)	printf("[TRACE: %s() at \"%s\"::line(%d)] " fmt"\n", __FUNCTION__, __FILE__, __LINE__, ## args);
#else
#define DBG_ERR(fmt, args...)
#define DBG_WRN(fmt, args...)
#define DBG_INFO(fmt, args...)
#define DBG_TRACE(fmt, args...)
#endif


///> For Sudoku.cpp
#if MYDEBUG
#define DBG_INFO_D2(x)	do { for(auto & p1: x) { for(auto & p2: p1) printf("%d ", p2); printf("\n"); } printf("\n");} while(false);
#define DBG_TRACE_D1(x)	do { for(auto & p: x) printf("%d ", p); printf("\n");} while(false);
#define DBG_TRACE_D2(x)	do { for(auto & p1: x) { for(auto & p2: p1) printf("%d ", p2); printf("\n"); } printf("\n");} while(false);
#else
#define DBG_INFO_D2(x)
#define DBG_TRACE_D1(x)
#define DBG_TRACE_D2(x)
#endif


#endif /* _MYDEBUG_H_ */
