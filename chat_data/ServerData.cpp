#include "ServerData.h"

Data::Data()
	: buffsize(1024), max_clients(30), addrlen(sizeof(struct sockaddr_in)), port(27015),
	message("Small Global Chat v1.0  \r\n")
{
	buffer = new char[buffsize + 1];

	//creating empty socket table
	for (i = 0; i < 30; i++)
	{
		client_socket[i] = 0;
	}
}

Data::~Data()
{
}

int Data::winsockInit()
{
	std::cout << message << "\nWinsock initialization...................................";
	int result = WSAStartup(MAKEWORD(2, 2), &wsa);
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
	std::cout << "Socket function initialization...........................";
	main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (main_socket == INVALID_SOCKET)
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
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
}

int Data::bindInit()
{
	std::cout << "Binding adress...........................................";
	int binding = bind(main_socket, (SOCKADDR *)& server, sizeof(server));
	if (binding == SOCKET_ERROR)
	{
		std::cout << "Fail.\n";
		closesocket(main_socket);
		return 1;
	}
	std::cout << "Done.\n";
	return 0;
}

void Data::listenInit()
{
	std::cout << "Ongoing listetning for incoming connectons...............";
	int listening = listen(main_socket, 5);
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

	FD_ZERO(&fdset_socket);

	//adding main socket to socket table
	FD_SET(main_socket, &fdset_socket);

	//adding existing client connections to socket table
	for (i = 0; i < max_clients; i++)
	{
		tmp_socket = client_socket[i];
		if (tmp_socket > 0)
		{
			FD_SET(tmp_socket, &fdset_socket);
		}
	}
}

int Data::isSmthActive()
{

	//select function  waits for incoming connections (fifth NULL means, that it will wait forever :)  
	activity = select(0, &fdset_socket, NULL, NULL, NULL);

	if (activity == SOCKET_ERROR)
	{
		std::cout << "Error on select function: " << WSAGetLastError();
		return 1;
	}
	return 0;
}

void Data::isNewConnection()
{
	if (FD_ISSET(main_socket, &fdset_socket))
	{
		new_socket = accept(main_socket, (struct sockaddr *)&address, (int *)&addrlen);

		isSocket(new_socket);

		//displaying new connection on log console:
		std::cout << "New connection: " << new_socket << ", ip address : "
			<< inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) << "\n";

		//sending testing message
		this->sendMsg();

		this->addSocket();

	}
}

int Data::sendMsg()
{
	std::cout << "Sending testing message: ";
	int sending = send(new_socket, message, strlen(message), 0);
	if (sending != strlen(message))
	{
		std::cout << "Failed.";
		return 1;
	}

	std::cout << "Success.\n";
	return 0;
}

void Data::addSocket()
{
	for (i = 0; i < max_clients; i++)
	{
		if (client_socket[i] == 0)
		{
			client_socket[i] = new_socket;
			std::cout << "Connection added to socket table with id: " << i << "\n";
			break;
		}
	}
}

void Data::msgExchange()
{
	for (i = 0; i < max_clients; i++)
	{
		tmp_socket = client_socket[i];

		//if client is in table           
		if (FD_ISSET(tmp_socket, &fdset_socket))
		{
			//taking adress of incoming source of message
			getpeername(tmp_socket, (struct sockaddr*)&address, (int*)&addrlen);

			//reading incoming message
			read_value = recv(tmp_socket, buffer, buffsize, 0);


			if (read_value == SOCKET_ERROR)
			{
				int error_code = WSAGetLastError();

				//if someone disconnected
				if (error_code == WSAECONNRESET)
				{

					std::cout << "Client disconnected: ip adress: " << inet_ntoa(address.sin_addr)
						<< ", port " << ntohs(address.sin_port) << "\n";


					closesocket(tmp_socket);
					client_socket[i] = 0;
				}
				else
				{
					std::cout << "Error with reading/receaving message: " << error_code;
				}
			}
			if (read_value == 0)
			{
				std::cout << "Client disconnected: ip adress: " << inet_ntoa(address.sin_addr)
					<< ", port " << ntohs(address.sin_port) << "\n";

				closesocket(tmp_socket);
				client_socket[i] = 0;
			}
			else
				this->broadcastMsg();
		}
	}
}

void Data::broadcastMsg()
{
	//writing message in log console
	std::cout << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << " : " << buffer << "\n";
	//sending message to all connected clients, besides sender
	for (i = 0; i < max_clients; i++)
	{
		if (client_socket[i] != tmp_socket)
		{
			send(client_socket[i], buffer, read_value, 0);
		}
	}
}

SOCKET Data::getSocket()
{
	return tmp_socket;
}
