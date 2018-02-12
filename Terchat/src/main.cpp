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
			Sleep(50);//reduce cpu usage
		}
		else
		{
			std::string message;
			std::string nickname;

			if (packet >> message >> nickname)
			{
				// register if it's a new client
				if (false)
				{
					if (clients.find(sender.toString()) == clients.end())
					{
						std::cout << "new client registered" << port << std::endl;
						clients.insert({ sender.toString(), nickname });
					}
				}
				else
				{
					if (clients.find(nickname) == clients.end())
					{
						std::cout << "new client registered" << std::endl;
						clients.insert({ nickname, sender.toString() });
					}
				}

				for (std::pair<std::string, std::string> client : clients)
				{
					//if (client.first == nickname)
						//continue;

					sf::Packet sendPacket;
					sendPacket.clear();
					sendPacket << message << nickname;
					if (socket.send(sendPacket, client.second, PORTCLIENT) == sf::Socket::Done)
					{
						std::cout << "Sent: " << message << std::endl;
					}
					else
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
		std::cout << "Server address: ";
		std::cin >> serverIP;
		std::cin.ignore(); //clears newline

		std::cout << "Your nickname: ";
		std::getline(std::cin, nickname);
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

			if (receivedPacket >> receivedMessage >> messenger)
			{
				std::cout << "<" << messenger << "> " << receivedMessage << std::endl;
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

	while (true)
	{
		std::string message;
		std::getline(std::cin, message);
		packet.clear();
		packet << message << nickname;

		if (socket.send(packet, serverIP, PORTSERVER) != sf::Socket::Done)
		{
			//error
			std::cout << "client message failed" << std::endl;
		}
	}
}

int main()
{
	char who;
	std::cout << "Do you want to be a server (s) or a client (c)? ";
	std::cin >> who;
	std::cin.ignore(); //clears newline

	if (who == 'c')
		client();
	else
		server();

	char pause;
	std::cin >> pause;

	return 0;
}