#include <iostream>
#include <process.h>						//dla w�tk�w
#include <winsock2.h>						//dla socket�w + ws2_32.lib dodany do projektu
#include <conio.h>							//dla getch()


class sendFlag								//ustawia flag� podaj�c�, czy wywo�a� w�tek wysy�aj�cy
{											//utworzony dynamicznie b�dzie wsp�ldzielony mi�dzy g��wnym w�tkiem wysy�aj�cym a nas�uchuj�cym
private:
	bool toSend;							//jesli przyjdzie jaka� wiadomo��, w�tek nas�uchuj�cy ustawi t� zmienn� na warto�� true
public:										//a po wys�aniu do wszystkich wiadomo�ci ustawi ponownie na false
	sendFlag()
		: toSend(false)
	{

	}
};

void isSomeoneThere(void * Args)
{
	do
	{
		std::cout << "kumbaja\n";
		Sleep(1000);
	} while (1);

	_endthread();
}

void listenForMsg(void * Args)
{
	do
	{
		std::cout << "KUKARACZA\n";
		Sleep(10000);
	} while (1);

	_endthread();
}

void sendMsg(void * Args)
{
	do
	{
		std::cout << "kAlABaNgA!\n";
		Sleep(500);
	} while (1);

	_endthread();
}

int main()
{
	


	int buffsize = 1000;
	char* buffer = new char[buffsize];							//bufor przetrzymuj�cy wiadomo��


	//inicjalizacja winsocka
	WSADATA wsaData;															

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
		std::cout << "Nie mo�na zainicjowa� gniazda";


	//watki
	_beginthread(isSomeoneThere, 0, NULL);						//w�tek nas�uchuj�cy nowych u�ytkownik�w
	_beginthread(listenForMsg, 0, NULL);						//nas�uchuje wiadomo�ci od pod��czonych klient�w
	_beginthread(sendMsg, 0, NULL);								//wysy�a wiadomo�� do klient�w



	delete[] buffer;
	buffer = 0;

	_getch();

	return 0;
}