#pragma once

#include<iostream>
#include<SFML/Network.hpp>
#include "PlayerMessage.h"
using std::cout;
using namespace sf;

class Prediction
{
public:
	Prediction();
	~Prediction();
	void AddLocalMessage(const PlayerMessage& msg);
	void AddNetworkMessage(const PlayerMessage & msg);
	sf::Vector2f linearPrediction(std::vector<PlayerMessage>& messages, float gameTime, float recievedX, float recievedY);
	sf::Vector2f quadraticPrediction(std::vector<PlayerMessage>& messages, float gameTime, float recievedX, float recievedY);

	std::vector<PlayerMessage> net_Messages;
	std::vector<PlayerMessage> local_Messages;

private:

 
};
