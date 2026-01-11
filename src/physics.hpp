#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include "particle.hpp"
#include "utils.hpp"


struct Boundary {
    sf::Vector2f start;
    sf::Vector2f end;
};

namespace physics {
    struct CollisionResult {
        bool hit;
        sf::Vector2f collision_point;
    };

    const sf::Vector2f GRAVITY {0.f, 500.f};  // pixels/sec²

    void update_particle_position(Particle& p, sf::Time dt) {
        float dt_sec = dt.asSeconds();

        // Semi-implicit Euler: update velocity first, then position
        sf::Vector2f velocity = p.get_velocity();
        velocity += GRAVITY * dt_sec;
        p.set_velocity(velocity);

        sf::Vector2f position = p.get_position();
        position += velocity * dt_sec;
        p.set_position(position);
    }

    CollisionResult particle_collision_detected(Particle& p1, Particle& p2) {
        float dist = utils::euclidean_distance(p1.get_position(), p2.get_position());
        bool hit = dist < (p1.get_radius() + p2.get_radius());

        if (dist == 0.f) {
            return {hit, p1.get_position()};
        }

        // Collision point: on the line between centers, at p1's surface toward p2
        sf::Vector2f dir = (p2.get_position() - p1.get_position()) / dist;
        sf::Vector2f point = p1.get_position() + dir * p1.get_radius();

        return {hit, point};
    }

    CollisionResult boundary_collision_detected(Particle& p, Boundary& b) {
        sf::Vector2f ps = p.get_position() - b.start;
        sf::Vector2f boundary_vector = b.end - b.start;

        float boundary_length = boundary_vector.length();
        sf::Vector2f boundary_unit_vector = boundary_vector / boundary_length;
        
        float projection_length = utils::dot_product(ps, boundary_unit_vector);
        projection_length = std::clamp(projection_length, 0.f, boundary_length);

        sf::Vector2f collision_point = b.start + projection_length * boundary_unit_vector;

        float dist = utils::euclidean_distance(collision_point, p.get_position());
        bool hit = dist < p.get_radius();

        return {hit, collision_point};
    }

    void handle_boundary_collision(Particle& p, Boundary& b, sf::Vector2f collision_point) {
        sf::Vector2f distance = collision_point - p.get_position();
        if (distance.length() == 0.f) {
            return;
        }

        sf::Vector2f normal = distance / distance.length();

        // Move particle out of wall
        float overlap = p.get_radius() - distance.length();
        if (overlap > 0) {
            p.set_position(p.get_position() - overlap * normal);
        }

        // Only reflect if moving into the wall
        float v_normal = utils::dot_product(p.get_velocity(), normal);
        if (v_normal >= 0) {
            sf::Vector2f v_tangent = p.get_velocity() - v_normal * normal;
            sf::Vector2f new_v = -p.get_restitution() * v_normal * normal + v_tangent;
            p.set_velocity(new_v);
        }
    }

    void handle_particle_collision(Particle& p1, Particle& p2) {
        sf::Vector2f distance = p1.get_position() - p2.get_position();
        sf::Vector2f normal = distance / utils::euclidean_distance(p1.get_position(), p2.get_position());
        
        if (distance.length() == 0) {
            p1.set_position(p1.get_position() + (p1.get_radius()) * normal);
            p2.set_position(p2.get_position() - (p2.get_radius()) * normal);
            return;
        }
        
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
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                auto result = particle_collision_detected(particles[i], particles[j]);
                if (result.hit) {
                    handle_particle_collision(particles[i], particles[j]);
                }
            }
        }
    }

    void handle_environment_collisions(std::vector<Particle>& particles, std::vector<Boundary>& boundaries) {
        float margin = 150.f;
        float ground_y = 1080.f - margin;
        float left_wall_x = margin;
        float right_wall_x = 1920.f - margin;

        for (auto& particle: particles) {
            // Ground collision
            if (particle.get_position().y + particle.get_radius() >= ground_y) {
                sf::Vector2f new_position {particle.get_position().x, ground_y - particle.get_radius()};
                particle.set_position(new_position);

                if (particle.get_velocity().y > 0) {
                    sf::Vector2f new_velocity {particle.get_velocity().x, -particle.get_velocity().y * particle.get_restitution()};
                    particle.set_velocity(new_velocity);
                }
            }

            // Right wall collision
            if (particle.get_position().x + particle.get_radius() >= right_wall_x) {
                sf::Vector2f new_position {right_wall_x - particle.get_radius(), particle.get_position().y};
                particle.set_position(new_position);

                if (particle.get_velocity().x > 0) {
                    sf::Vector2f new_velocity {-particle.get_velocity().x * particle.get_restitution(), particle.get_velocity().y};
                    particle.set_velocity(new_velocity);
                }
            }

            // Left wall collision
            if (particle.get_position().x - particle.get_radius() <= left_wall_x) {
                sf::Vector2f new_position {left_wall_x + particle.get_radius(), particle.get_position().y};
                particle.set_position(new_position);

                if (particle.get_velocity().x < 0) {
                    sf::Vector2f new_velocity {-particle.get_velocity().x * particle.get_restitution(), particle.get_velocity().y};
                    particle.set_velocity(new_velocity);
                }
            }

            // Roof collision
            float roof_y = margin;
            if (particle.get_position().y - particle.get_radius() <= roof_y) {
                sf::Vector2f new_position {particle.get_position().x, roof_y + particle.get_radius()};
                particle.set_position(new_position);

                if (particle.get_velocity().y < 0) {
                    sf::Vector2f new_velocity {particle.get_velocity().x, -particle.get_velocity().y * particle.get_restitution()};
                    particle.set_velocity(new_velocity);
                }
            }
        }
    }

    void handle_collisions(std::vector<Particle>& particles, std::vector<Boundary>& boundaries) {
        handle_particle_collisions(particles);
        handle_environment_collisions(particles, boundaries);
    }

    void apply_mouse_force(Particle& particle, sf::Vector2f force_application_point, float force_strength, float dt) {
        if (force_strength != 0.f) {
            sf::Vector2f force_vector = force_application_point - particle.get_position();
            float dist = force_vector.length();
            sf::Vector2f force_direction_normal = force_vector / force_vector.length();
            sf::Vector2f force = force_strength * force_direction_normal;
            sf::Vector2f newVel = particle.get_velocity() + force * dt;
            particle.set_velocity(newVel);
        }
    }
}