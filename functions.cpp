#include "functions.h"

int isSocket(SOCKET socket)
{
	if (socket < 0)
	{
		std::cout << "B�ad funkcji akceptuj�cej.\n";
		return 1;
	}
	return 0;
}