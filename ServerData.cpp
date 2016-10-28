#include "ServerData.h"

Data::Data()
	: m_buffsize(1024), m_max_clients(30), m_addrlen(sizeof(struct sockaddr_in)), m_port(27015),
	m_message("Small Global Chat v1.0)  \r\n")
{
	m_buffer = new char[m_buffsize + 1];

	//creating empty socket table
	for (m_i = 0; m_i < 30; m_i++)
	{
		m_client_socket[m_i] = 0;
	}
}

Data::~Data()
{
}

int Data::winsockInit()
{
	std::cout << m_message << "\nWinsock initialization: ";
	int result = WSAStartup(MAKEWORD(2, 2), &m_wsa);
	if (result != NO_ERROR)
	{
		std::cout << "Cannot initialize.";
		return 1;
	}
	std::cout << "Done\n";
	return 0;
}

int Data::socketInit()
{	
	std::cout << m_message << "\nSocket function initialization: ";
	m_main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_main_socket == INVALID_SOCKET)
	{
		std::cout << "Cannot initialize.: \n" << WSAGetLastError();
		WSACleanup();
		return 1;
	}
	std::cout << "Done.\n";
	return 0;
}

void Data::adressInit()
{
	memset(&m_server, 0, sizeof(m_server));
	m_server.sin_family = AF_INET;
	m_server.sin_addr.s_addr = INADDR_ANY;
	m_server.sin_port = htons(m_port);
}

int Data::bindInit()
{	
	std::cout << m_message << "Binding adress: ";
	int binding = bind(m_main_socket, (SOCKADDR *)& m_server, sizeof(m_server));
	if (binding == SOCKET_ERROR)
	{
		std::cout << "Fail.\n";
		closesocket(m_main_socket);
		return 1;
	}
	std::cout << "Done.\n";
	return 0;
}

void Data::listenInit()
{
	std::cout << m_message << "Ongoing listetning for incoming connectons: ";
	int listening = listen(m_main_socket, 5);
	if (listening == SOCKET_ERROR)
	{
		std::cout << "Error.\n";
	}
	std::cout << "Initialized. \n";
}

//adding sockets to m_fdset_socket (typ fd_set)
//fd_set will be used by select()
void Data::setFD()
{

	FD_ZERO(&m_fdset_socket);

	//adding main socket to socket table
	FD_SET(m_main_socket, &m_fdset_socket);

	//adding existing client connections to socket table
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		m_tmp_socket = m_client_socket[m_i];
		if (m_tmp_socket > 0)
		{
			FD_SET(m_tmp_socket, &m_fdset_socket);
		}
	}
}

int Data::isSmthActive()
{

	//select waits for incoming connections (fifth NULL means, that it will wait forever :)  
	m_activity = select(0, &m_fdset_socket, NULL, NULL, NULL);

	if (m_activity == SOCKET_ERROR)
	{
		std::cout << "Error on select function: " << WSAGetLastError();
		return 1;
	}
	return 0;
}

void Data::isNewConnection()
{
	if (FD_ISSET(m_main_socket, &m_fdset_socket))
	{
		m_new_socket = accept(m_main_socket, (struct sockaddr *)&m_address, (int *)&m_addrlen);

		isSocket(m_new_socket);

		//Displaying new connection on log console:
		std::cout << "New connection: " << m_new_socket << ", ip address : "
			<< inet_ntoa(m_address.sin_addr) << ", port : " << ntohs(m_address.sin_port) << "\n";

		//sending testing message
		this->sendMsg();

		this->addSocket();

	}
}

int Data::sendMsg()
{
	std::cout << m_message << "Sending testing message: ";
	int sending = send(m_new_socket, m_message, strlen(m_message), 0);
	if (sending != strlen(m_message))
	{
		std::cout << "Failed.";
		return 1;
	}

	std::cout << "Success.\n";
	return 0;
}

void Data::addSocket()
{
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		if (m_client_socket[m_i] == 0)
		{
			m_client_socket[m_i] = m_new_socket;
			std::cout << "Connection added to socket table with id: " << m_i << "\n";
			break;
		}
	}
}

void Data::msgExchange()
{
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		m_tmp_socket = m_client_socket[m_i];

		//jesli klient jest dodany           
		if (FD_ISSET(m_tmp_socket, &m_fdset_socket))
		{
			//pobiera dane od obiektu przychodzacego i przypisuje do m_adress
			getpeername(m_tmp_socket, (struct sockaddr*)&m_address, (int*)&m_addrlen);

			//odczytanie przychodzacego sygnalu
			m_read_value = recv(m_tmp_socket, m_buffer, m_buffsize, 0);


			if (m_read_value == SOCKET_ERROR)
			{
				int error_code = WSAGetLastError();

				//jesli ktos sie odlaczyl
				if (error_code == WSAECONNRESET)
				{

					std::cout << "Klient odlaczyl sie , adres ip " << inet_ntoa(m_address.sin_addr)
						<< ", port " << ntohs(m_address.sin_port) << "\n";


					closesocket(m_tmp_socket);
					m_client_socket[m_i] = 0;
				}
				else
				{
					std::cout << "Blad podczas odbierania : " << error_code;
				}
			}
			if (m_read_value == 0)
			{
				std::cout << "Klient odlaczony , adres ip " << inet_ntoa(m_address.sin_addr)
					<< ", port " << ntohs(m_address.sin_port) << "\n";

				closesocket(m_tmp_socket);
				m_client_socket[m_i] = 0;
			}
			else
				this->broadcastMsg();
		}
	}
}

void Data::broadcastMsg()
{
	//wypisuje wiadomosc na oknie serwera
	std::cout << inet_ntoa(m_address.sin_addr) << ":" << ntohs(m_address.sin_port) << " : " << m_buffer << "\n";
	//wysyla do wszystkich poza klientem ktory jest autorem wiadomosci
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		if (m_client_socket[m_i] != m_tmp_socket)
		{
			send(m_client_socket[m_i], m_buffer, m_read_value, 0);
		}
	}
}

SOCKET Data::getSocket()
{
	return m_tmp_socket;
}
