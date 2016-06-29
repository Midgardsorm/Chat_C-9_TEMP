#pragma once

#include <iostream>
#include <process.h>
#include <Windows.h>

class Ending
{
public:
	bool isEnd;
	Ending();
	
};

void waitForEsc(void * mainEnding);