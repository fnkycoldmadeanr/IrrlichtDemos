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


#ifndef __TIMER_H_INCLUDED__
#define __TIMER_H_INCLUDED__

#include <toolbox_stdafx.h>



class dHeightResolutionTimer
{
	public:
	dHeightResolutionTimer(); 
	~dHeightResolutionTimer();
	dFloat GetElapsedSeconds();
	static unsigned64 GetTimeInMicrosenconds();
	
	private:
	unsigned64 m_prevTime;
	unsigned64 m_totalFrames;
};

#endif 

