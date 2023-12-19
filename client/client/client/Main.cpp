#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <SFML/Network/UdpSocket.hpp>
#include <iostream>
#include<chrono>
#include <cmath>
#define _CMATH_
#include <cstdlib>
#include "Level.h"

int main()
{
    sf::RenderWindow window(VideoMode(640, 480), "Target Practise");

    Level level(&window);
   
    sf::Clock clock;
    float deltaTime;


    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        level.Input();
        level.Update(deltaTime);

        window.clear(sf::Color(71, 187, 255));
        level.Render();
        window.display();
    }

    return 0;
}