#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include <math.h>

class Particle
{
private:
    sf::Vector2f position;
    sf::Vector2f velocity {2.0f, 0.0};
    sf::Vector2f acceleration {0.0f, 0.0f};
    float radius {5.0f};
    float mass {2.0f};
    float restitution {0.75};  // Coefficient of restitution (bounciness)
    sf::Color color {sf::Color::White};  // Particle color

    int circumferance_points {32};
    sf::VertexArray vertices;

    sf::VertexArray draw_particle(sf::Vector2f center_position) {
        int total_points = circumferance_points + 2;    // 1 center, 1 to close the circle
        
        sf::VertexArray circle(sf::PrimitiveType::TriangleFan, total_points); 

        circle[0].position = position;
        circle[0].color = color;  // Use particle's color

        float angle_inc = 2 * M_PI / circumferance_points;   // -1 because the first is the center
        float angle = 0;
        for (int i = 1; i <= circumferance_points + 1; i++) {
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            circle[i].position = circle[0].position + sf::Vector2f({x, y}); 
            circle[i].color = color;  // Use particle's color
            angle += angle_inc;
        }
        
        return circle;
    }

public:
    Particle(sf::Vector2f p, sf::Vector2f v, sf::Vector2f a)
        :position {p}, velocity{v}, acceleration{a} {
            vertices = draw_particle(position);
    }
    
    Particle(sf::Vector2f p, sf::Vector2f v, sf::Vector2f a, sf::Color c)
        :position {p}, velocity{v}, acceleration{a}, color{c} {
            vertices = draw_particle(position);
    }

    Particle(sf::Vector2f p, float r): position {p}, radius {r} {
            vertices = draw_particle(position);
    }

    const sf::VertexArray& getVertices() const {
        return vertices;
    }

    void update(sf::Time elapsed) {
        float dt = elapsed.asSeconds();
        position = position + (velocity * dt) + (acceleration * (dt*dt * 0.5f));
        sf::Vector2f new_acceleration = apply_forces();
        velocity = velocity + (acceleration + new_acceleration)*(dt * 0.5f);

        vertices = draw_particle(position);
    }

    sf::Vector2f apply_forces() const {
        sf::Vector2f total_force {0.0f, 0.0f};

        // Apply gravity F = m*g
        sf::Vector2f gravity {0.0f, 9.81f * 100.0f * mass};
        
        // Apply collision force
        // TODO: ...

        total_force += gravity;

        sf::Vector2f acceleration = total_force / mass;
        
        return acceleration;
    }
    sf::Vector2f get_position() const {
        return position;
    }
    void set_position(sf::Vector2f p) {
        position = p;
    }
    float get_radius() const {
        return radius;
    }
    sf::Vector2f get_velocity() const {
        return velocity;
    }
    void set_velocity(sf::Vector2f v) {
        velocity = v;
    }
    sf::Vector2f get_acceleration() const {
        return acceleration;
    }
    float get_mass() const {
        return mass;
    }
    float get_restitution() const {
        return restitution;
    }

};

class ParticleSim : public sf::Drawable, public sf::Transformable
{
public:
    ParticleSim(int total_num_of_particles, sf::Vector2u window_size) 
        : window_size{window_size}, max_particles{total_num_of_particles} {
        // Create ground line at the bottom of the screen
        const float margin = 40.f;
        ground_y = static_cast<float>(window_size.y) - margin;
        
        ground_line = sf::VertexArray(sf::PrimitiveType::Lines, 2);
        ground_line[0].position = sf::Vector2f(0.f, ground_y);
        ground_line[0].color = sf::Color::White;
        ground_line[1].position = sf::Vector2f(static_cast<float>(window_size.x), ground_y);
        ground_line[1].color = sf::Color::White;

        // Create left wall
        left_wall = sf::VertexArray(sf::PrimitiveType::Lines, 2);
        left_wall[0].position = sf::Vector2f(left_wall_x, 0.f);
        left_wall[0].color = sf::Color::White;
        left_wall[1].position = sf::Vector2f(left_wall_x, ground_y);
        left_wall[1].color = sf::Color::White;
        
        // Create right wall
        right_wall = sf::VertexArray(sf::PrimitiveType::Lines, 2);
        right_wall[0].position = sf::Vector2f(right_wall_x, 0.f);
        right_wall[0].color = sf::Color::White;
        right_wall[1].position = sf::Vector2f(right_wall_x, ground_y);
        right_wall[1].color = sf::Color::White;

        // Set spawn position at top-center of screen (for spawning mode)
        spawn_position = sf::Vector2f(
            static_cast<float>(window_size.x) / 2.f, 
            100.f
        );
        
        // Create particles in a circle pattern
        create_particles_circle(total_num_of_particles);
    }

    void update(sf::Time dt) {
        for (auto& particle : particles) {
            particle.update(dt);
            handle_ground_collision(particle);
        }
        handle_particle_collisions();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // Draw the boundaries
        target.draw(ground_line, states);
        // target.draw(left_wall, states);
        // target.draw(right_wall, states);
        
        // Draw all particles
        for (const auto& particle: particles) {
            target.draw(particle.getVertices(), states);
        }
    }
    
    // Spawn a single particle shooting downward
    void spawn_particle() {
        if (particles.size() < static_cast<size_t>(max_particles)) {
            sf::Vector2f velocity(0.0f, 200.0f);  // Shoot downward at 200 units/sec
            sf::Vector2f acceleration(0.0f, 0.0f);
            particles.push_back(Particle(spawn_position, velocity, acceleration));
        }
    }
    
    size_t get_particle_count() const {
        return particles.size();
    }

private:
    void create_particles_circle(int total_num_of_particles) {
        float center_x = static_cast<float>(window_size.x) / 2.f;
        float center_y = 300.f;  // Height where circle center is
        float ring_radius = 200.f;  // Radius of the circle formation
        
        for (int i = 0; i < total_num_of_particles; i++) {
            float angle = 2.f * M_PI * i / total_num_of_particles;
            float x = center_x + ring_radius * std::cos(angle);
            float y = center_y + ring_radius * std::sin(angle);
            
            // Generate a unique color for each particle using HSV-like approach
            float hue = 360.f * i / total_num_of_particles;  // Distribute hue evenly
            sf::Color particle_color = hsv_to_rgb(hue, 0.8f, 1.0f);
            
            particles.push_back(Particle(sf::Vector2f(x, y), sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), particle_color));
        }
    }
    
    // Helper function to convert HSV to RGB
    sf::Color hsv_to_rgb(float h, float s, float v) {
        float c = v * s;
        float x = c * (1.f - std::abs(std::fmod(h / 60.f, 2.f) - 1.f));
        float m = v - c;
        
        float r, g, b;
        if (h < 60) {
            r = c; g = x; b = 0;
        } else if (h < 120) {
            r = x; g = c; b = 0;
        } else if (h < 180) {
            r = 0; g = c; b = x;
        } else if (h < 240) {
            r = 0; g = x; b = c;
        } else if (h < 300) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }
        
        return sf::Color(
            static_cast<unsigned char>((r + m) * 255),
            static_cast<unsigned char>((g + m) * 255),
            static_cast<unsigned char>((b + m) * 255)
        );
    }
    
    void handle_ground_collision(Particle& particle) {
        // Ground collision
        if (particle.get_position().y + particle.get_radius() >= ground_y) {
            sf::Vector2f new_position {particle.get_position().x, ground_y - particle.get_radius()};
            particle.set_position(new_position);
            
            sf::Vector2f new_velocity {particle.get_velocity().x, -particle.get_velocity().y * particle.get_restitution()};
            particle.set_velocity(new_velocity);
        }
        
        // Right wall collision
        if (particle.get_position().x + particle.get_radius() >= right_wall_x) {
            sf::Vector2f new_position {right_wall_x - particle.get_radius(), particle.get_position().y};
            particle.set_position(new_position);
            
            sf::Vector2f new_velocity {-particle.get_velocity().x * particle.get_restitution(), particle.get_velocity().y};
            particle.set_velocity(new_velocity);
        }
        
        // Left wall collision
        if (particle.get_position().x - particle.get_radius() <= left_wall_x) {
            sf::Vector2f new_position {left_wall_x + particle.get_radius(), particle.get_position().y};
            particle.set_position(new_position);
            
            sf::Vector2f new_velocity {-particle.get_velocity().x * particle.get_restitution(), particle.get_velocity().y};
            particle.set_velocity(new_velocity);
        }
    }

    void handle_particle_collisions() {
        for (int i = 0; i < particles.size(); i++) {
            for (int j = i + 1; j < particles.size(); j++) {
                if (colision_detected(particles[i], particles[j])) {
                    handle_collision(particles[i], particles[j]);
                }
            }
        }
    }   
    float dot_product(const sf::Vector2f& v1, const sf::Vector2f& v2) const {
        return v1.x * v2.x + v1.y * v2.y;
    }
    void handle_collision(Particle& p1, Particle& p2) {
        // ... code to handle collision
        sf::Vector2f distance = p1.get_position() - p2.get_position();
        if (distance.length() == 0) {
            return;
        }
        
        sf::Vector2f normal = distance / euclidean_distance(p1.get_position(), p2.get_position());
        
        float overlap = (p1.get_radius() + p2.get_radius()) - distance.length();
        if (overlap > 0) {
            p1.set_position(p1.get_position() + (overlap / 2.0f) * normal);
            p2.set_position(p2.get_position() - (overlap / 2.0f) * normal);
        }

        // 2. Velocity vector decomposition
        sf::Vector2f v1_normal = dot_product(p1.get_velocity(), normal) * normal;
        sf::Vector2f v1_tangent = p1.get_velocity() - v1_normal;

        sf::Vector2f v2_normal = dot_product(p2.get_velocity(), normal) * normal;
        sf::Vector2f v2_tangent = p2.get_velocity() - v2_normal;

        // 3. Calculate new velocities
        float v1_before = dot_product(p1.get_velocity(), normal);
        float v2_before = dot_product(p2.get_velocity(), normal);
        
        float m1 = p1.get_mass();
        float m2 = p2.get_mass();
        float v1_after = (m1*v1_before + m2*v2_before - m2*p1.get_restitution() * (v1_before - v2_before)) / (m1 + m2);
        float v2_after = (m1*v1_before + m2*v2_before + m1*p2.get_restitution() * (v1_before - v2_before)) / (m1 + m2);

        sf::Vector2f new_v1 = v1_after*normal + v1_tangent;
        sf::Vector2f new_v2 = v2_after*normal + v2_tangent;

        p1.set_velocity(new_v1);
        p2.set_velocity(new_v2);

    }

    bool colision_detected(Particle& p1, Particle& p2) {
        return euclidean_distance(p1.get_position(), p2.get_position()) < (p1.get_radius() + p2.get_radius());
    }

    float euclidean_distance(const sf::Vector2f& p1, const sf::Vector2f& p2) const {
        return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
    }


    std::vector<Particle> particles;
    sf::VertexArray ground_line;
    sf::VertexArray left_wall;
    sf::VertexArray right_wall;
    sf::Vector2u window_size;
    float ground_y;
    int max_particles;
    sf::Vector2f spawn_position;
    float left_wall_x = 100.0f;
    float right_wall_x = 1800.0f;

};
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
