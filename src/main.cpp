#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include "particle_simulation.hpp"



int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    ParticleSim particleSim(200, window.getSize());
    sf::Clock clock;
    // sf::Clock spawn_timer;  // Timer for spawning particles (commented out for circle mode)

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // Mouse force: left = attract, right = repel
        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        float forceStrength = 0.f;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            forceStrength = 2000.f;  // Attract
        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            forceStrength = -2000.f;  // Repel
        }

        sf::Time dt = clock.restart();
        particleSim.update(dt, mousePos, forceStrength);
        
        // Spawn a particle every 100ms (commented out for circle mode)
        // if (spawn_timer.getElapsedTime().asMilliseconds() >= 100) {
        //     particleSim.spawn_particle();
        //     spawn_timer.restart();
        // }

        window.clear();
        window.draw(particleSim);
        window.display();
    }
}
