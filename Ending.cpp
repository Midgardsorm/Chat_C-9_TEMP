#include "Ending.h"

Ending::Ending() : isEnd(false)
{
}


void waitForEsc(void * mainEnding)
{
	Ending *someData = reinterpret_cast<Ending*>(mainEnding);
	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			someData->isEnd = true;
			_endthread();
		}
	}
}
