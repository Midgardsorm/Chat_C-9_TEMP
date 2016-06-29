#include "functions.h"

int isSocket(SOCKET socket)
{
	if (socket < 0)
	{
		std::cout << "B³ad funkcji akceptuj¹cej.\n";
		return 1;
	}
	return 0;
}