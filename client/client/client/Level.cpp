#include "Level.h"

Level::Level(sf::RenderWindow* w)
{
	srand(time(NULL));
	//sf::RenderWindow window(VideoMode(640, 480), "Ball Shooter");

	window = w;
	connected[0] = false;
	connected[1] = false;


	//connecting to server and getting playerId 
	std::string IP;
	cout << "Enter IP: ";
	std::cin >> IP;
	ip = IP;
	while (client.connect(ip, 1234) != sf::Socket::Done) {
		cout << "Failed to connect, trying again...\n";
	}
	cout << "Connected to server\n";
	client.send(packetSend);
	packetRecieved.clear();
	client.receive(packetRecieved);
	packetRecieved >> ID;

	//Assigning ID for easier traversal of all the arrays
	ID -= 1;

	client.setBlocking(false);

	window->setFramerateLimit(60);
	
	//Projectile Variables Initializations
	projectile.setFillColor(Color::Red);
	projectile.setRadius(5.f);
	projectiles[ID].push_back(CircleShape(projectile));

	
	//Player Variables Initializations
	players[0].setFillColor(Color::White);
	players[0].setRadius(50.f);
	players[0].setPosition(window->getSize().x / 2 - players[0].getRadius() - 100, window->getSize().y - players[0].getRadius() * 2 - 10.f);

	players[1].setFillColor(Color::Black);
	players[1].setRadius(50.f);
	players[1].setPosition(window->getSize().x / 2 - players[1].getRadius() + 100, window->getSize().y - players[1].getRadius() * 2 - 10.f);
	
	shootTimer[ID] = 0;
	
	//Enemy Variables Initializations
	enemy.setFillColor(Color::Magenta);
	enemy.setSize(Vector2f(50.f, 50.f));
	enemies.push_back(RectangleShape(enemy));
	enemySpawnTimer = 0;


	//loading font for fps 
	if (!font.loadFromFile("fonts/FifteenTwenty.otf"))
	{
		std::cout << "Error Loading the font" << std::endl;
	}

	//Points
	playerPoints[0] = 0;
	playerPoints[1] = 0;

	gameover = false; 
	begin = false;
	predictionChange = 1;
	packetSendTimer = 0;
	sendRate = 0.1f;

	textInit();

}

Level::~Level()
{
}

void Level::textInit()
{
	PointsText[0].setFont(font);
	PointsText[0].setCharacterSize(34);
	PointsText[0].setFillColor(sf::Color::White);
	PointsText[0].setPosition(0.f, 0.f);
	PointsText[0].setString(std::to_string('0'));
	PointsText[1].setFont(font);
	PointsText[1].setCharacterSize(34);
	PointsText[1].setFillColor(sf::Color::White);
	PointsText[1].setPosition(window->getSize().x - 50, 0.0f);
	PointsText[1].setString(std::to_string('0'));

	timerText.setFont(font);
	timerText.setCharacterSize(34);
	timerText.setFillColor(sf::Color::White);
	timerText.setPosition(window->getSize().x / 2 - 100, 0.f);
	timerText.setString(std::to_string(timer.asSeconds()));

	winnerText.setFont(font);
	winnerText.setCharacterSize(28);
	winnerText.setFillColor(sf::Color::White);
	winnerText.setPosition(120, window->getSize().y / 2);

	Title.setFont(font);
	Title.setCharacterSize(28);
	Title.setFillColor(sf::Color::White);
	Title.setPosition(150, 0);
	Title.setString("Target Practise");

	playerConnection[0].setFont(font);
	playerConnection[0].setCharacterSize(20);
	playerConnection[0].setFillColor(sf::Color::White);
	playerConnection[0].setPosition(0, 200);
	playerConnection[0].setString("Waiting for Player 1");

	playerConnection[1].setFont(font);
	playerConnection[1].setCharacterSize(20);
	playerConnection[1].setFillColor(sf::Color::White);
	playerConnection[1].setPosition(400, 200);
	playerConnection[1].setString("Waiting for Player 2");

	instructions.setFont(font);
	instructions.setCharacterSize(24);
	instructions.setFillColor(sf::Color::White);
	instructions.setPosition(200, 400);
	instructions.setString("Press Space to Start");

	predictionText.setFont(font);
	predictionText.setCharacterSize(20);
	predictionText.setFillColor(sf::Color::White);
	predictionText.setPosition(window->getSize().x / 2 - 100, 50.f);
	predictionText.setString("No Prediction");
}

void Level::Input()
{
	switch (gameState) {

	case LOBBY:
		InputLobby();
		break;
	case LEVEL:
		InputGameplay();
		break;
	}
}

void Level::Update(float dt)
{
	packetSendTimer += dt;

	switch (gameState) {

	case LOBBY:
		UpdateLobby();
		break;
	case LEVEL:
		UpdateGameplay(dt);
		break;
	}
}

void Level::Render()
{
	switch (gameState) {

	case LOBBY:
		RenderLobby();
		break;
	case LEVEL:
		RenderGameplay();
		break;
	}
}

void Level::UpdateGameplay(float dt)
{
	gametime += dt;

	if (packetSendTimer > sendRate) {
		packetSend << ID << players[ID].getPosition().x << players[ID].getPosition().y << gametime << playerPoints[ID];

		client.send(packetSend);
		packetSend.clear();

		packetSendTimer = 0;
	}

	packetRecieved.clear();
	if (client.receive(packetRecieved) == sf::Socket::Done)
	{
		int id, points;
		float x, y, recievedTime;
		packetRecieved >> id >> x >> y >> recievedTime >> points;

		PlayerMessage m_Local;
		m_Local.id = id;
		m_Local.time = gametime;
		m_Local.x = players[id].getPosition().x;
		m_Local.y = players[id].getPosition().y;
		prediction.AddLocalMessage(m_Local);

		PlayerMessage m_Net;
		m_Net.id = id;
		m_Net.time = recievedTime;
		m_Net.x = x;
		m_Net.y = y;
		prediction.AddNetworkMessage(m_Net);

		sf::Vector2f LocalPredict, NetworkPredict, InterpolationPos;

		switch (predictionChange)
		{
		case 1:
			predictionText.setString("No Prediction");
			players[id].setPosition(x, y);
			break;
		case 2:
			predictionText.setString("Linear Prediction");
			cout << "\nLinear Prediction: ";
			cout << "\nLocal Prediction: ";
			LocalPredict = prediction.linearPrediction(prediction.local_Messages, gametime, x, y);

			cout << "\nNetwork Prediction: ";
			NetworkPredict = prediction.linearPrediction(prediction.net_Messages, gametime, x, y);
			players[id].setPosition(NetworkPredict);
			break;
		case 3:
			predictionText.setString("Quadratic Prediction");
			cout << "\nQuadratic Prediction: ";
			cout << "\nLocal Prediction: ";
			LocalPredict = prediction.quadraticPrediction(prediction.local_Messages, gametime, x, y);

			cout << "\nNetwork Prediction: ";
			NetworkPredict = prediction.quadraticPrediction(prediction.net_Messages, gametime, x, y);
			players[id].setPosition(NetworkPredict);
			break;
		default:
			break;
		}



		playerPoints[id] = points;
	}




	timerText.setString(std::to_string(timer.asSeconds()));

	if (timer.asSeconds() <= 30)
	{
		timer = clock.getElapsedTime();

		//Incriment the shoot timer
		if (shootTimer[ID] < 20)
		{
			shootTimer[ID]++;
		}

		//Player can shoot 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && shootTimer[ID] >= 20)
		{
			projectile.setPosition(playerCenters[ID]);
			projectiles[ID].push_back(CircleShape(projectile));

			shootTimer[ID] = 0;
		}

		//Incriment the enemy spawn timer
		if (enemySpawnTimer < 40)
		{
			enemySpawnTimer++;
		}

		//Spawn an enemy at random location
		if (enemySpawnTimer >= 40)
		{
			enemy.setPosition((rand() % int(window->getSize().x - enemy.getSize().x)), (rand() % int(window->getSize().y - 150)));
			enemies.push_back(RectangleShape(enemy));

			enemySpawnTimer = 0;
		}
	}
	else if (timer.asSeconds() >= 30)
	{
		VictoryCondition();
		gameover = true;
	}

	for (size_t i = 0; i < projectiles[ID].size(); i++)
	{
		projectiles[ID][i].move(0.f, -10.f);

		if (projectiles[ID][i].getPosition().y <= 0)
		{
			projectiles[ID].erase(projectiles[ID].begin() + i);
		}
	}

	//Collision
	if (!enemies.empty() && !projectiles[ID].empty())
	{
		for (size_t i = 0; i < projectiles[ID].size(); i++)
		{
			for (size_t k = 0; k < enemies.size(); k++)
			{
				if (projectiles[ID][i].getGlobalBounds().intersects(enemies[k].getGlobalBounds()))
				{
					projectiles[ID].erase(projectiles[ID].begin() + i);
					enemies.erase(enemies.begin() + k);
					playerPoints[ID] += 1;
					break;
				}
			}
		}
	}



	//Updating the points 

	PointsText[0].setString(std::to_string(playerPoints[0]));
	PointsText[1].setString(std::to_string(playerPoints[1]));

}

void Level::UpdateLobby()
{
	connected[ID] = true;

	if (packetSendTimer > sendRate) {
		packetSend << ID << connected[ID] << begin;

		client.send(packetSend);
		packetSend.clear();

		packetSendTimer = 0;
	}

	packetRecieved.clear();
	if (client.receive(packetRecieved) == sf::Socket::Done)
	{
		int id;
		bool connect, start;
		packetRecieved >> id >> connect >> start;

		begin = start;
		connected[id] = connect;
	}

	if (begin == true)
	{
		clock.restart();
		gameState = LEVEL;
	}


	if (connected[0] == true)
	{
		playerConnection[0].setString("Player 1 Connected");
	}
	if (connected[1] == true)
	{
		playerConnection[1].setString("Player 2 Connected");
	}
}

void Level::RenderGameplay()
{
	window->draw(players[0]);
	window->draw(players[1]);

	for (size_t i = 0; i < enemies.size(); i++)
	{
		window->draw(enemies[i]);
	}

	for (size_t i = 0; i < projectiles[ID].size(); i++)
	{
		window->draw(projectiles[ID][i]);
	}

	window->draw(PointsText[0]);
	window->draw(PointsText[1]);
	window->draw(timerText);
	window->draw(predictionText);

	if (gameover = true)
	{
		window->draw(winnerText);
	}

}

void Level::RenderLobby()
{
	window->draw(Title);
	window->draw(playerConnection[0]);
	window->draw(playerConnection[1]);

	if (connected[1] == true)
	{
		window->draw(instructions);
	}
}

void Level::InputGameplay()
{
	playerCenters[ID] = Vector2f(players[ID].getPosition().x + players[ID].getRadius(), players[ID].getPosition().y + players[ID].getRadius());

	//Player Controls
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		players[ID].move(-5.f, 0.f);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		players[ID].move(5.f, 0.f);
	}



	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		predictionChange = 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		predictionChange = 2;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		predictionChange = 3;
	}
}

void Level::InputLobby()
{
	if (connected[1] == true)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			begin = true;
		}
	}
}

void Level::VictoryCondition()
{

	if (playerPoints[0] > playerPoints[1])
	{
		winnerText.setString("Player 1 is the Winner");
	}
	else if (playerPoints[0] < playerPoints[1])
	{
		winnerText.setString("Player 2 is the Winner");
	}
	else if (playerPoints[0] == playerPoints[1])
	{
		winnerText.setString("Its a Draw");
	}

}
