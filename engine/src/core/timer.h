#pragma once

#include "defines.h"
#include "platform/platform.h"

class Timer
{
public:
	void Start()
	{
		StartTime = Platform::GetAbsoluteTime();
		UpdatedTime = 0.0;
	}

	void Stop()
	{
		StartTime = 0;
	}

	void Update()
	{
		if (StartTime != 0.0)
			UpdatedTime = Platform::GetAbsoluteTime();
	}

	f64 GetElapsedTime()
	{
		return UpdatedTime - StartTime;
	}

	f64 StartTime;
	f64 UpdatedTime;
};