#pragma once

#include<iostream>
#include"SFML/Graphics.hpp"
#include"SFML/Window.hpp"
#include"SFML/System.hpp"
#include<SFML/Network.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Prediction.h"
#include "PlayerMessage.h"
#include<vector>
#include<cstdlib>
#include <string>
#include <iostream>
#include<vector>
#include <chrono>
using std::cout;
using std::endl;
using namespace sf;


class  Level
{
public:
	 Level(sf::RenderWindow* w);
	~ Level();
	void textInit();
	void Input();
	void Update(float dt);
	void Render();
	void UpdateGameplay(float dt);
	void UpdateLobby();
	void RenderGameplay();
	void RenderLobby();
	void InputGameplay();
	void InputLobby();
	void VictoryCondition();

private:

	sf::RenderWindow* window;

	//Projectile Variables
	CircleShape projectile;
	std::vector<CircleShape> projectiles[2];

	//Player Variables
	CircleShape players[2];
	Vector2f playerCenters[2];
	int playerPoints[2];
	int shootTimer[2];

	//Prediction Class
	Prediction prediction;

	//Enemy Variables
	RectangleShape enemy;
	std::vector<RectangleShape> enemies;
	int enemySpawnTimer;

	//Fonts & Texts
	sf::Font font;
	sf::Text PointsText[2], timerText, winnerText, Title, playerConnection[2], instructions, predictionText, interpolationText;


	//Networking Components
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	sf::TcpSocket client;
	sf::Packet packetSend, packetRecieved;
	int ID;
	bool recieved = false;

	float packetSendTimer, gametime, sendRate;
	sf::Time timer;
	sf::Clock clock;


	bool gameover, connected[2], begin;
	int predictionChange;

	
	//enmu to form game states
	enum GAMESTATE{LOBBY, LEVEL};
	GAMESTATE gameState = LOBBY;

};






