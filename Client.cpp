#define WIN32_LEAN_AND_MEAN						//przy u�yciu windows.h wyrzucamy windows socketes, by unikn�� k��tni z winsock2
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <process.h>							//dla w�tk�w
#include <winsock2.h>							//dla socket�w + ws2_32.lib dodany do projektu
#include <conio.h>								//dla getch()

SOCKET mainSocket;

class sendFlag									//ustawia flag� podaj�c�, czy wywo�a� w�tek wysy�aj�cy
{												//utworzony dynamicznie b�dzie wsp�ldzielony mi�dzy g��wnym w�tkiem wysy�aj�cym a nas�uchuj�cym
private:
	bool toSend;								//jesli przyjdzie jaka� wiadomo��, w�tek nas�uchuj�cy ustawi t� zmienn� na warto�� true
public:											//a po wys�aniu do wszystkich wiadomo�ci ustawi ponownie na false
	sendFlag()
		: toSend(false)
	{

	}
};

void listenForMsg(void * buffer)
{
	do {
		int bytesRecv = SOCKET_ERROR;
		while (bytesRecv == SOCKET_ERROR)		//p�tla nas�uchuj�ca wiadomo�ci
		{
			bytesRecv = recv(mainSocket, reinterpret_cast<char*>(buffer), 1000, 0);
		}
		std::cout << "Server: " << reinterpret_cast<char*>(buffer) << "\n";
		Sleep(1000);
	} while (1);

	_endthread();
}

void sendMsg(void * buffer)
{


	do {
		std::cin.getline(reinterpret_cast<char*>(buffer), 1000);
		send(mainSocket, reinterpret_cast<char*>(buffer), 1000, 0);
	} while (*reinterpret_cast<char*>(buffer) != '#');

	std::cout << "Ending trasmission ... \n";


	_endthread();
}

int main()
{

	int buffsize = 1000;
	char* buffer = new char[buffsize];											//bufor przetrzymuj�cy wiadomo��

	
	//inicjalizacja winsocka
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
	{
		std::cout << "Nie mo�na zainicjowa� gniazda";
	}

	//inicjalizacja gniazda
	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainSocket == INVALID_SOCKET)
	{
		printf("Error creating socket: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	
	//podanie ip i portu
	sockaddr_in service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27015);

	//pod��czanie do serwera
	if (connect(mainSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("Failed to connect.\n");
		WSACleanup();
		return 1;
	}

	//
	
	//watki
	
	_beginthread(listenForMsg, 0, buffer);										//nas�uchuje wiadomo�ci od pod��czonych klient�w
	HANDLE watek = (HANDLE)_beginthread(sendMsg, 0, buffer);					//wysy�a wiadomo�� do klient�w

	WaitForSingleObject(watek, INFINITE);										//dopoki klient nie zadecyduje o wylaczeniu, poprzez wyslanie '#'
	
	delete[] buffer;
	buffer = 0;

	_getch();


	return 0;
}
