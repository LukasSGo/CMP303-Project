#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <iostream>
#include<vector>
using std::cout;
using std::cin;
using std::endl;

int main()
{
	cout << "The is now Server Running" << endl;

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	cout << "Server IP: " << ip << "\n";

	std::vector<sf::TcpSocket*> clients;

	sf::TcpListener listener;
	listener.listen(1234);

	sf::SocketSelector selector;
	selector.add(listener);

	sf::Packet packet, sendPacket;
	bool loop = true;
	int ID;

	while (loop)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				if (clients.size() <= 2)
				{
					//Accept the incoming socket
					sf::TcpSocket* sock = new sf::TcpSocket;
					if (listener.accept(*sock) == sf::Socket::Done)
					{
						if (sock->receive(packet) == sf::Socket::Done)
						{

						}
						cout << "\nNew user connected";
						clients.push_back(sock);
						selector.add(*sock);
						packet.clear();
						ID = clients.size();
						packet << ID;
						sock->send(packet);
					}
				}

			}

			else
			{
				for (int i = 0; i < clients.size(); ++i) {
					if (selector.isReady(*clients[i]))
					{
						//convert to switch statement
						sf::Socket::Status status = clients[i]->receive(packet);
						switch (status)
						{
						case sf::Socket::Done:
							for (int j = 0; j < clients.size(); j++)
							{
								if (i != j)
								{
									clients[j]->send(packet);
								}
							}
							break;
						case sf::Socket::Disconnected:
							cout << "\nUser disconnected";
							selector.remove(*clients[i]);
							clients.erase(find(clients.begin(), clients.end(), clients[i]));
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}

	return 0;
}