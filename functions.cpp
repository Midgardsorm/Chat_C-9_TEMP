#include "functions.h"

int isSocket(SOCKET socket)
{
	if (socket < 0)
	{
		std::cout << "Error on accept() function \n";
		return 1;
	}
	return 0;
}
