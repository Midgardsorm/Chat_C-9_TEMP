#include "Ending.h"

Ending::Ending() : isEnd(false)
{
}

//when you put waitForEsc into _behinthread, you can pass one argument 
//for function, and it need to be void pointer, thus waitForEsc(void * arg) 
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
