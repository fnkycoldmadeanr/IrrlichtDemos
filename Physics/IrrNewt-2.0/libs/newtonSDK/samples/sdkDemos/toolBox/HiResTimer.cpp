/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/




#include <toolbox_stdafx.h>
#include "HiResTimer.h"
#include "OpenGlUtil.h"

#ifdef _MSC_VER
#include <mmsystem.h> 
#endif


extern int keyBoardTraceMode;

#define LOCK_FRAME_RATE

#define LOCKED_FPS		float (100.0f)
#define LOCKED_TIMESTEP int ((1000000.0f/LOCKED_FPS))

const dFloat TICKS2SEC = 1.0e-6f;

CHiResTimer::CHiResTimer() 
{
	m_prevTime = 0;
}

CHiResTimer::~CHiResTimer() 
{
}



dFloat CHiResTimer::GetElapsedSeconds()
{
	dFloat timeStep;
	unsigned miliseconds;
	
//	miliseconds = glutGet (GLUT_ELAPSED_TIME);
	miliseconds = GetTimeInMicrosenconds();

	// optimal keep the fps below 120 fps
	#ifdef LOCK_FRAME_RATE
	while ((miliseconds - m_prevTime) < LOCKED_TIMESTEP) {
		miliseconds = GetTimeInMicrosenconds();
	}
	#endif

	timeStep = dFloat (miliseconds - m_prevTime) * TICKS2SEC;
	m_prevTime = miliseconds;
	
	if (timeStep > 1.0f) {
		timeStep = 1.0f;
	}
//	if (timeStep < 0.005f) {
//		timeStep = 0.005f;
//	}


if (keyBoardTraceMode)
{
#ifdef RECORD_LOG
//	Sleep (100);
	extern FILE * file;
	fwrite (&timeStep, sizeof (float), 1, file);
	fflush (file);
#endif

#ifdef READ_LOG
	extern FILE * file;
	//Sleep (150);
	fread (&timeStep, sizeof (float), 1, file);
#endif
}

	
	return timeStep;
} 



unsigned CHiResTimer::GetTimeInMicrosenconds()
{
#ifdef _MSC_VER
	LARGE_INTEGER count;
	static bool firstTime = true;
	static bool haveTimer = false;
	static LARGE_INTEGER frequency;
	static LARGE_INTEGER baseCount;

	if (firstTime) {
		firstTime = false;
		haveTimer = QueryPerformanceFrequency(&frequency) ? true : false;
		QueryPerformanceCounter (&baseCount);
	} 

	if (haveTimer) {
		unsigned ticks;
		QueryPerformanceCounter (&count);
		count.QuadPart -= baseCount.QuadPart;
		ticks = unsigned (count.QuadPart * LONGLONG (1000000) / frequency.QuadPart);
		return ticks;
	} else {
		//return glutGet (GLUT_ELAPSED_TIME) * 1000;
		return timeGetTime() * 1000;
	}
#endif

#ifdef _LINUX_VER
	timespec ts;
	// clock_gettime(CLOCK_MONOTONIC, &ts); // Works on FreeBSD
	clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux
	return ts.tv_nsec / 1000;
#endif


#ifdef _MAC_VER
	timeval tp;
	gettimeofday(&tp, NULL);
	return unsigned (tp.tv_usec);
#endif
}





