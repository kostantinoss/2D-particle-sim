#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include <string>
#include "particle_simulation.hpp"


int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "2D Particle Simulation");
    window.setFramerateLimit(144);

    ParticleSim particleSim(200, window.getSize());
    sf::Clock clock;
    sf::Clock fpsClock;
    int frameCount = 0;
    float fps = 0.f;

    // Load font for UI text
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Montserrat-Black.ttf")) {
        printf("Font not found\n");
    }

    // Info text
    sf::Text infoText(font, "", 20);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition({10.f, 10.f});

    // Controls text
    sf::Text controlsText(font, "Controls:\nSpace - Spawn particle\nR - Reset simulation\nLeft click - Attract\nRight click - Repel", 16);
    controlsText.setFillColor(sf::Color(200, 200, 200));
    controlsText.setPosition({10.f, 1080.f - 120.f});

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Handle keyboard events
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    particleSim.spawn_particle();
                }
                else if (keyPressed->code == sf::Keyboard::Key::R) {
                    particleSim.reset();
                }
                else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        // Mouse force: left = attract, right = repel
        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        float forceStrength = 0.0f;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            forceStrength = 2000.f;  // Attract
        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            forceStrength = -2000.f;  // Repel
        }

        sf::Time dt = clock.restart();
        particleSim.update(dt, mousePos, forceStrength);

        // Calculate FPS
        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 0.5f) {
            fps = static_cast<float>(frameCount) / fpsClock.getElapsedTime().asSeconds();
            frameCount = 0;
            fpsClock.restart();
        }

        // Update info text
        infoText.setString("FPS: " + std::to_string(static_cast<int>(fps)) +
                          "\nParticles: " + std::to_string(particleSim.get_particle_count()));

        window.clear();
        window.draw(particleSim);
        window.draw(infoText);
        window.draw(controlsText);
        window.display();
    }
}
