#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>

class Particle
{
private:
    sf::Vector2f position;
    sf::Vector2f velocity {2.0f, 0.0};
    sf::Vector2f acceleration {0.0f, 0.0f};
    float radius;
    float mass {1.0f};

    int circumferance_points {32};
    sf::VertexArray vertices;

    sf::VertexArray draw_particle(sf::Vector2f center_position) {
        int total_points = circumferance_points + 2;    // 1 center, 1 to close the circle
        
        sf::VertexArray circle(sf::PrimitiveType::TriangleFan, total_points); 

        circle[0].position = position;
        circle[0].color = sf::Color::White;

        float angle_inc = 2 * M_PI / circumferance_points;   // -1 because the first is the center
        float angle = 0;
        for (int i = 1; i <= circumferance_points + 1; i++) {
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            circle[i].position = circle[0].position + sf::Vector2f({x, y}); 
            circle[i].color = sf::Color::White;
            angle += angle_inc;
        }
        
        return circle;
    }

public:
    Particle(sf::Vector2f p, sf::Vector2f v, sf::Vector2f a)
        :position {p}, velocity{v}, acceleration{a} {
            vertices = draw_particle(position);
    }

    Particle(sf::Vector2f p, float r): position {p}, radius {r} {
            vertices = draw_particle(position);
    }

    const sf::VertexArray& getVertices() const {
        return vertices;
    }

    void update(sf::Time elapsed) {
        // float dt = elapsed.asSeconds();
        // position = position + (velocity * dt) + (acceleration * (dt*dt * 0.5f));
        // sf::Vector2f new_acceleration = apply_forces();
        // velocity = velocity + (acceleration + new_acceleration)*(dt * 0.5f);

        // acceleration = new_acceleration;
        float dt = elapsed.asSeconds();  // Use SECONDS, not milliseconds!
        
        // Apply forces to get new acceleration
        sf::Vector2f new_acceleration = apply_forces();
        
        // Simple Euler integration (easier to understand)
        velocity += new_acceleration * dt;
        position += velocity * dt;

        vertices = draw_particle(position);
    }

    sf::Vector2f apply_forces() const {
        sf::Vector2f total_force {0.0f, 0.0f};

        // Apply gravity F = m*g
        sf::Vector2f gravity {0.0f, mass * 9.81f * 100};
        
        // Apply collision force
        // TODO: ...

        total_force += gravity;

        sf::Vector2f acceleration = total_force / mass;
        
        return acceleration;
    }

};

class ParticleSim : public sf::Drawable, public sf::Transformable
{
public:
    ParticleSim(int total_num_of_particles, sf::Vector2u window_size) {
        // Visual boundary (drawing only; no collision logic here)
        const float margin = 40.f;
        boundary_shape.setPosition({margin, margin});
        boundary_shape.setSize(
            {static_cast<float>(window_size.x) - 2.f * margin,
             static_cast<float>(window_size.y) - 2.f * margin}
        );
        boundary_shape.setFillColor(sf::Color::Transparent);
        boundary_shape.setOutlineThickness(2.f);
        boundary_shape.setOutlineColor(sf::Color(180, 180, 180));

        create_particles(total_num_of_particles);
    }

    void update(sf::Time dt) {
        for (auto& particle : particles) {
            particle.update(dt);
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(boundary_shape, states);
        for (const auto& particle: particles) {
            target.draw(particle.getVertices(), states);
        }
    }

private:
    void create_particles(int total_num_of_particles) {
        for (int i = 0; i < total_num_of_particles; i++) {
            float x = 200.f + (i % 5) * 100.f;  // 5 particles per row
            float y = 200.f + (i / 5) * 100.f;  // New row every 5 particles
            particles.push_back(Particle(sf::Vector2f(x, y), 30.f));  // radius 30
        }
    }

    std::vector<Particle> particles;
    sf::RectangleShape boundary_shape;

};

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    ParticleSim particleSim(3, window.getSize());
    sf::Clock clock;

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

        window.clear();
        window.draw(particleSim);
        window.display();
    }
}
