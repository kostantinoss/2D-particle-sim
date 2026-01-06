#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include <math.h>



int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    ParticleSim particleSim(200, window.getSize());  // 50 particles in circle
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
        
        sf::Time dt = clock.restart();
        particleSim.update(dt);
        
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
