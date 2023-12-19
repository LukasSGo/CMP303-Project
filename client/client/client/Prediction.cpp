#include "Prediction.h"
using namespace std;

Prediction::Prediction()
{
}

Prediction::~Prediction()
{
}

void Prediction::AddLocalMessage(const PlayerMessage& msg)
{
	local_Messages.push_back(msg);
}

void Prediction::AddNetworkMessage(const PlayerMessage& msg)
{
	net_Messages.push_back(msg);
}

sf::Vector2f Prediction::linearPrediction(std::vector<PlayerMessage>& messages, float gameTime, float recievedX, float recievedY)
{
	float predictedX = -1.0f;
	float predictedY = -1.0f;

	const int msize = messages.size();
	if (msize < 2) {
		return sf::Vector2f(recievedX, recievedY);
	}

	const PlayerMessage& msg0 = messages[msize - 1];
	const PlayerMessage& msg1 = messages[msize - 2];

	float speedX, speedY, displacementx, displacementy;

	cout << "\nMessage 1 x: " << msg0.x << ". Message 1 y: " << msg0.y << ". Message 1 Time:" << msg0.time;
	cout << "\nMessage 2 x: " << msg1.x << ". Message 2 y: " << msg1.y << ". Message 2 Time:" << msg1.time;

	speedX = (msg0.x - msg1.x) / (msg0.time - msg1.time);
	speedY = (msg0.y - msg1.y) / (msg0.time - msg1.time);
	displacementx = speedX * (gameTime - msg1.time);
	displacementy = speedY * (gameTime - msg1.time);
	predictedX = displacementx + msg1.x;
	predictedY = displacementy + msg1.y;

	cout << "\nPredicted Cords: (" << predictedX << " , " << predictedY << ")";


	return sf::Vector2f(predictedX, predictedY);
}

sf::Vector2f Prediction::quadraticPrediction(std::vector<PlayerMessage>& messages, float gameTime, float recievedX, float recievedY)
{
	float predictedX = -1.0f;
	float predictedY = -1.0f;

	const int msize = messages.size();
	if (msize < 3) {
		return sf::Vector2f(recievedX, recievedY);
	}

	const PlayerMessage& msg0 = messages[msize - 1];
	const PlayerMessage& msg1 = messages[msize - 2];
	const PlayerMessage& msg2 = messages[msize - 3];

	float x_average_velocity, y_average_velocity;

	cout << "\nMessage 1 x: " << msg0.x << ". Message 1 y: " << msg0.y << ". Message 1 Time:" << msg0.time;
	cout << "\nMessage 2 x: " << msg1.x << ". Message 2 y: " << msg1.y << ". Message 2 Time:" << msg1.time;
	cout << "\nMessage 3 x: " << msg2.x << ". Message 3 y: " << msg1.y << ". Message 3 Time:" << msg2.time;

	//Quadratic Model

	float speedX1, speedY1, speedX2, speedY2, accelX, accelY, displacementx, displacementy;


	speedX1 = (msg0.x - msg1.x) / (msg0.time - msg1.time);
	speedY1 = (msg0.y - msg1.y) / (msg0.time - msg1.time);

	speedX2 = (msg1.x - msg2.x) / (msg1.time - msg2.time);
	speedY2 = (msg1.y - msg2.y) / (msg1.time - msg2.time);

	accelX = (speedX2 - speedX1) / (msg2.time - (msg0.time));
	accelY = (speedY2 - speedY1) / (msg2.time - (msg0.time));

	displacementx = (speedX2 * (gameTime - msg2.time)) + ((0.5 * accelX) * powf((gameTime - msg2.time), 2));
	displacementy = (speedY2 * (gameTime - msg2.time)) + ((0.5 * accelY) * powf((gameTime - msg2.time), 2));

	predictedX = msg2.x + displacementx;
	predictedY = msg2.y + displacementy;

	cout << "\nPredicted Cords: (" << predictedX << " , " << predictedY << ")";


	return sf::Vector2f(predictedX, predictedY);
}
