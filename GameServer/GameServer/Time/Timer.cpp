// Timer.cpp: implementation of the CTimer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//*****************************************************************************
// constructor
// � timer�� �̿��� ������ check�ϰ� start time�� �����Ѵ�
//*****************************************************************************
CTimer::CTimer()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency) == FALSE)
	{
		// performance counter ��� �Ұ���.
		m_bUsePerformanceCounter = FALSE;

		// Windows NT/2000�� ���, ������ ���� �����ν� timeGetTime�� precision
		//�� ������ �� �ִ�
		TIMECAPS Caps;
		::timeGetDevCaps(&Caps, sizeof(Caps)); // resolution�� �����ϱ� ���ؼ�
		//timer device�� Caps�� ���´�.
		
		if (::timeBeginPeriod(Caps.wPeriodMin) == TIMERR_NOCANDO)
		{
			// ��� �� error�� ���� ��� ����
			// ���� �̷� error�� ���ٸ� default timeGetTime()�� ���̰� �ɰ�!
#ifdef __TIMER_DEBUG
			__TraceF(TEXT("timeBeginPeriod(...) Error\n"));
			//CDebug::OutputDebugString("timeBeginPeriod(...) Error");
#endif //__TIMER_DEBUG 
		}

		// multimedia timer�� ���� �ð��� ��´�
		m_mmAbsTimerStart = m_mmTimerStart = ::timeGetTime();
	}
	else
	{
		// Performance Counter ��밡��.
		m_bUsePerformanceCounter = TRUE;

		// Performance Counter�� �̿��� ���� �ð��� ��´� 
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_pcTimerStart);

		// ���� �ð��� �����Ѵ�
		m_pcAbsTimerStart = m_pcTimerStart;
		
		// QueryPerformanceFrequency�� ��� frequency�κ��� timer resolution��
		//����Ѵ�.
		// timer resolution(�ֱ�)�̶�, ������ �ּ� �ð� ������ ���Ѵ�.
		// frequency�� ������ �ֱ��̹Ƿ� ������ ���ؼ� ���Ѵ�.
		//1000.0�� ���ϴ� ���� sec unit�� millisecond unit���� �ٲٱ� ���ؼ��̴�
		m_resolution = (float)(1.0 / (double)m_frequency) * 1000.0f;
	}
}

CTimer::~CTimer()
{
	if (!m_bUsePerformanceCounter) // timeGetTime�� �̿��ϴ� ���
	{
		// multimedia timer�� �ݴ´�
		TIMECAPS Caps;
		::timeGetDevCaps(&Caps, sizeof(Caps));
		// ������ mimimum timer resoltion�� clear�Ѵ�
		::timeEndPeriod(Caps.wPeriodMin);
	}
}

//*****************************************************************************
// ResetTimer()�� �θ� ���ķ� �帥 �ð��� ��´�
//*****************************************************************************
double CTimer::GetTimeElapsed()
{
	__int64 timeElapsed;

	if (m_bUsePerformanceCounter)	// if using Performance Counter
	{
		// ���� �ð��� ��� ���� �ð��� ����
		::QueryPerformanceCounter((LARGE_INTEGER*)&timeElapsed);
		timeElapsed -= m_pcTimerStart;
		return (double)timeElapsed * (double)m_resolution;
	}
	else	// if not
	{
		timeElapsed = ::timeGetTime() - m_mmTimerStart;
		return (double)timeElapsed;
	}
}

//*****************************************************************************
// ���� �ð��� ��´�
//*****************************************************************************
double CTimer::GetAbsTime()
{
	__int64 absTime;

	if (m_bUsePerformanceCounter)	// if using Performance Counter
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&absTime);
		return (double)absTime * (double)m_resolution;
	}
	else	// if not
	{
		absTime = ::timeGetTime();
		return (double)absTime;
	}
}

//*****************************************************************************
// Timer�� reset�Ѵ�
//*****************************************************************************
void CTimer::ResetTimer()
{
	// start time�� �ٽ� ��´�
	if (m_bUsePerformanceCounter)	// if using Performance Counter
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_pcTimerStart);
	else	// if not
		m_mmTimerStart = ::timeGetTime();
}
