#ifndef CSYSTIMER_H
#define CSYSTIMER_H

#ifdef __linux__
#include <sys/time.h>
#endif

class CTimeSys
{
private:
	bool m_bActive;
#ifdef _WIN32
    LARGE_INTEGER _tstart;
    LARGE_INTEGER _freq;
#endif
#ifdef __linux__
	struct timespec start, stop;
#endif
public:
	CTimeSys() {
		m_bActive = false;
	}
	
	bool isActive() {
		return m_bActive;
	}

	double Stop() {
		m_bActive = false;
		return GetSpan();
	}

#ifdef _WIN32
	void Start(void)
	{	
		m_bActive = true;

       QueryPerformanceFrequency(&_freq);
       QueryPerformanceCounter(&_tstart);
    }
    double GetSpan(void)
    {
		if(!m_bActive)
			return 0;
        LARGE_INTEGER _tend;
		QueryPerformanceCounter(&_tend);
        return ((double)_tend.QuadPart -
                    (double)_tstart.QuadPart)/((double)_freq.QuadPart);
    }
#endif
#ifdef __linux__
	#define BILLION  1000000000L;
	void Start(void)
	{
		m_bActive = true;

		clock_gettime( CLOCK_REALTIME, &start);
	}
	
	double GetSpan()
	{
		if(!m_bActive)
			return 0;

		clock_gettime( CLOCK_REALTIME, &stop);
		return  ( stop.tv_sec - start.tv_sec )+ (double)( stop.tv_nsec - start.tv_nsec )/ (double)BILLION;
	}
#endif
};

#endif
