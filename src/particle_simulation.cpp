#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <math.h>
#include "physics.hpp"
#include "particle.hpp"
#include "utils.hpp"



class ParticleSim : public sf::Drawable, public sf::Transformable
{
public:
    ParticleSim(int total_num_of_particles, sf::Vector2u window_size) 
        : window_size{window_size}, max_particles{total_num_of_particles} {
        create_boundaries();
        create_particles(total_num_of_particles);
    }

    void update(sf::Time dt) {
        for (auto& particle : particles) {
            // particle.update(dt);
            update_particle(particle, dt);
        }

        physics::handle_particle_collisions(particles);
        physics::handle_boundary_collisions(particles);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (const auto& particle: particles) {
            target.draw(particle.getVertices(), states);
        }
    }
    

private:
    void create_boundaries() {
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

    }

    void create_particles(int total_num_of_particles) {
        float center_x = static_cast<float>(window_size.x) / 2.f;
        float center_y = 300.f;  // Height where circle center is
        float ring_radius = 200.f;  // Radius of the circle formation
        
        for (int i = 0; i < total_num_of_particles; i++) {
            float angle = 2.f * M_PI * i / total_num_of_particles;
            float x = center_x + ring_radius * std::cos(angle);
            float y = center_y + ring_radius * std::sin(angle);
            
            // Generate a unique color for each particle using HSV-like approach
            float hue = 360.f * i / total_num_of_particles;  // Distribute hue evenly
            
            // Create particle and set its properties
            // TODO: fix Particle class structure. It must be a simple data class
            // with the basic properties of a particle
            Particle p {x, y};
            p.set_radius(10.f);
            p.set_mass(1.f);
            p.set_restitution(0.75f);
            p.set_color(particle_color);

            // Add particle to the simulation
            particles.push_back(particle);
        }
    }

    void update_particle(Particle& particle, sf::Time dt) {
        // TODO: Implement particle update
    }

    std::vector<Particle> particles;

    sf::Vector2u window_size;
    int max_particles;

    // sf::VertexArray ground_line;
    // sf::VertexArray left_wall;
    // sf::VertexArray right_wall;
    // sf::Vector2u window_size;
    
    // float ground_y;
    // int max_particles;
    
    // float left_wall_x = 100.0f;
    // float right_wall_x = 1800.0f;

};