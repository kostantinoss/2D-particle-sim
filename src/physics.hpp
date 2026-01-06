#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <math.h>
#include "particle.hpp"
#include "utils.hpp"


namespace physics {
    void handle_boundary_collisions(Particle& particle) {}
    
    bool colision_detected(const Particle& p1, const Particle& p2) {
        return utils::euclidean_distance(p1.get_position(), p2.get_position()) < (p1.get_radius() + p2.get_radius());
    }

    void handle_collision(Particle& p1, Particle& p2) {
        // ... code to handle collision
        sf::Vector2f distance = p1.get_position() - p2.get_position();
        if (distance.length() == 0) {
            return;
        }
        
        sf::Vector2f normal = distance / utils::euclidean_distance(p1.get_position(), p2.get_position());
        
        float overlap = (p1.get_radius() + p2.get_radius()) - distance.length();
        if (overlap > 0) {
            p1.set_position(p1.get_position() + (overlap / 2.0f) * normal);
            p2.set_position(p2.get_position() - (overlap / 2.0f) * normal);
        }

        // 2. Velocity vector decomposition
        sf::Vector2f v1_normal = utils::dot_product(p1.get_velocity(), normal) * normal;
        sf::Vector2f v1_tangent = p1.get_velocity() - v1_normal;

        sf::Vector2f v2_normal = utils::dot_product(p2.get_velocity(), normal) * normal;
        sf::Vector2f v2_tangent = p2.get_velocity() - v2_normal;

        // 3. Calculate new velocities
        float v1_before = utils::dot_product(p1.get_velocity(), normal);
        float v2_before = utils::dot_product(p2.get_velocity(), normal);
        
        float m1 = p1.get_mass();
        float m2 = p2.get_mass();
        float v1_after = (m1*v1_before + m2*v2_before - m2*p1.get_restitution() * (v1_before - v2_before)) / (m1 + m2);
        float v2_after = (m1*v1_before + m2*v2_before + m1*p2.get_restitution() * (v1_before - v2_before)) / (m1 + m2);

        sf::Vector2f new_v1 = v1_after*normal + v1_tangent;
        sf::Vector2f new_v2 = v2_after*normal + v2_tangent;

        p1.set_velocity(new_v1);
        p2.set_velocity(new_v2);

    }

    void handle_particle_collisions(std::vector<Particle>& particles) {
        for (int i = 0; i < particles.size(); i++) {
            for (int j = i + 1; j < particles.size(); j++) {
                if (colision_detected(particles[i], particles[j])) {
                    handle_collision(particles[i], particles[j]);
                }
            }
        }
    }

    void handle_ground_collision(Particle& particle) {
        // TODO: Find a way to hold info about the boundaries
        
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

}