#include <SFML\Network.hpp>
#include <iostream>
#include <string>
#include <Windows.h>
#include <unordered_map>
#include <thread>

unsigned short PORTSERVER = 43000;
unsigned short PORTCLIENT = 43001;

std::string nickname;
std::string serverIP;

HANDLE console;

void read();
void write();

void server()
{
	sf::UdpSocket socket;
	std::unordered_map<std::string, std::string> clients;

	//bind to a port
	if (socket.bind(PORTSERVER) != sf::Socket::Done)
	{
		std::cout << "server socket bind failed" << std::endl;
	}

	//receive data
	sf::IpAddress sender;
	sf::Packet packet;
	unsigned short port;

	std::cout << "waiting for messages..." << std::endl;
	while (true)
	{
		packet.clear();
		if (socket.receive(packet, sender, port) != sf::Socket::Done)
		{
			std::cout << "message received failed" << std::endl;
		}
		else
		{
			std::string message;
			std::string nickname;
			std::string color;

			if (packet >> message >> nickname >> color)
			{
				// register if it's a new client
				
				if (clients.find(sender.toString()) == clients.end())
				{
					std::cout << "new client registered: " << nickname << std::endl;
					clients.insert({ sender.toString(), nickname });
				}

				if (message == "")
					continue;

				for (std::pair<std::string, std::string> client : clients)
				{
					//if (client.first == nickname)
						//continue;

					sf::Packet sendPacket;
					sendPacket.clear();
					sendPacket << message << nickname << color;
					if (socket.send(sendPacket, client.first, PORTCLIENT) != sf::Socket::Done)
					{
						std::cout << "server message failed" << std::endl;
					}
				}
			}
			else
				std::cout << "Packet failed" << std::endl;
		}
	}
}

void client()
{
	std::string type;
	std::cout << "Write (w) or read (r): ";
	std::getline(std::cin, type);
	
	if (type == "w")
	{
		write();
	}
	else
	{
		read();
	}
}

void read()
{
	sf::UdpSocket socket;
	sf::Packet receivedPacket;
	sf::IpAddress sender;
	
	//bind to a port
	if (socket.bind(PORTCLIENT) != sf::Socket::Done)
	{
		std::cout << "client socket bind failed" << std::endl;
	}

	unsigned short port;
	while (true)
	{
		receivedPacket.clear();
		if (socket.receive(receivedPacket, sender, port) == sf::Socket::Done)
		{
			std::string receivedMessage;
			std::string messenger;
			std::string color;

			if (receivedPacket >> receivedMessage >> messenger >> color)
			{
				SetConsoleTextAttribute(console, std::stoi(color));
				std::cout << "<" << messenger << "> ";
				SetConsoleTextAttribute(console, 15);
				std::cout << receivedMessage << std::endl;
			}
		}
		else
		{
			Sleep(50);
		}
	}
}

void write()
{
	sf::UdpSocket socket;
	sf::Packet packet;

	std::cout << "Server address: ";
	std::cin >> serverIP;
	std::cin.ignore(); //clears newline

	std::cout << "Your nickname: ";
	std::getline(std::cin, nickname);

	for (int i = 0; i < 16; i++)
	{
		SetConsoleTextAttribute(console, i);
		std::cout << "(" << i << ")" << std::endl;
	}

	std::string color;
	std::cout << "Your color: ";
	std::getline(std::cin, color);

	while (true)
	{
		std::string message;
		std::getline(std::cin, message);
		packet.clear();
		packet << message << nickname << color;

		if (socket.send(packet, serverIP, PORTSERVER) != sf::Socket::Done)
		{
			//error
			std::cout << "client message failed" << std::endl;
		}

		for (int i = 0; i < 100; i++)
		{
			std::cout << "" << std::endl;
		}
	}
}

int main()
{
	char who;
	std::cout << "Do you want to be a server (s) or a client (c)? ";
	std::cin >> who;
	std::cin.ignore(); //clears newline

	console = GetStdHandle(STD_OUTPUT_HANDLE);

	if (who == 'c')
		client();
	else
		server();

	char pause;
	std::cin >> pause;

	return 0;
}