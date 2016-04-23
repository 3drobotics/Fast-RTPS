/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima Fast RTPS is licensed to you under the terms described in the
 * FASTRTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file eClock.cpp
 *
 */
#include <cmath>
#include <iostream>

#if (defined __MACH__ && defined __APPLE__)
#include <mach/mach_time.h>
#include <sys/time.h>
#else
#include <time.h>
#endif

#include <fastrtps/utils/eClock.h>
using namespace eprosima::fastrtps::rtps;

namespace eprosima {
namespace fastrtps {
	//FIXME: UTC SECONDS AUTOMATICALLY
eClock::eClock():
		m_seconds_from_1900_to_1970(0),
		m_utc_seconds_diff(0)
{
#if defined(_WIN32)
	QueryPerformanceFrequency(&freq);
#endif
}

eClock::~eClock() {

}


#if defined(_WIN32)
#include <cstdint>

bool eClock::setTimeNow(Time_t* tnow, ClockId_t clockId)
{

    if (clockId != REALTIME)
        return false;

    GetSystemTimeAsFileTime(&ft);
    ftlong = ft.dwHighDateTime;
    ftlong <<=32;
    ftlong |= ft.dwLowDateTime;
    ftlong /=10;
    ftlong -= 11644473600000000ULL;
	
//	std::cout << "ftlong: " << ftlong << std::endl;
	//std::cout << "sec from 1900 " << m_seconds_from_1900_to_1970<<std::endl;
	tnow->seconds = (int32_t)((long)(ftlong/1000000UL)+(long)m_seconds_from_1900_to_1970+(long)m_utc_seconds_diff);
	//std::cout << "seconds: " << tnow->seconds << " seconds " << std::endl;
	tnow->fraction = (uint32_t)((long)(ftlong%1000000UL)*pow(10.0,-6)*pow(2.0,32));
	return true;
}


void eClock::my_sleep(uint32_t milliseconds)
{
	#pragma warning(disable: 4430)
	Sleep(milliseconds);
	return;
}


void eClock::intervalStart()
{
	  GetSystemTimeAsFileTime(&ft1);
	  QueryPerformanceCounter(&li1);
}
uint64_t eClock::intervalEnd()
{
	GetSystemTimeAsFileTime(&ft2);
	QueryPerformanceCounter(&li2);
	return 0;

}

#else //UNIX VERSION
#include <unistd.h>

bool eClock::setTimeNow(Time_t* tnow, ClockId_t clockId)
{
	if (clockId == REALTIME)
	{
		gettimeofday(&m_now,NULL);
		tnow->seconds = m_now.tv_sec+m_seconds_from_1900_to_1970+m_utc_seconds_diff;
		tnow->fraction = (uint32_t)(m_now.tv_usec*pow(2.0,32)*pow(10.0,-6));
		return true;
	}
	else if (clockId == MONOTONIC)
	{
		constexpr int64_t i10p9 = 1000000000ll;
		constexpr uint64_t u2p32 = 1ull << 32;
#if (!defined __MACH__ || !defined __APPLE__)
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		int64_t now_ns = (int64_t)ts.tv_sec * i10p9 + ts.tv_nsec;
#else
		// read timebase info from os just once
		static struct mach_timebase_info tb_info = {0, 0};
		if (tb_info.denom == 0)
			mach_timebase_info(&tb_info);
		int64_t now_ns = (mach_absolute_time() * tb_info.numer) / tb_info.denom;
#endif
		tnow->seconds = now_ns / i10p9;
		// nsec % i10p9 is < i10p9 < 2^31, so the i64 multiply can't overflow
		uint64_t fraction_ns = now_ns % i10p9;
		tnow->fraction = (fraction_ns * u2p32 + i10p9 / 2) / i10p9;
		// XXX does m_now need to be set?
		m_now.tv_sec = tnow->seconds;
		m_now.tv_usec = uint32_t(fraction_ns) + 500 / 1000;
		return true;
	}
	else
	{
		return false;
	}
}

void eClock::my_sleep(uint32_t milliseconds)
{
	usleep(milliseconds*1000);
	return;
}

void eClock::intervalStart()
{
	gettimeofday(&m_interval1,NULL);
}

uint64_t eClock::intervalEnd()
{
	gettimeofday(&m_interval2,NULL);
	return (m_interval2.tv_sec-m_interval1.tv_sec)*1000000+m_interval2.tv_usec-m_interval1.tv_usec;
}



#endif



} /* namespace rtps */
} /* namespace eprosima */
