#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <random>
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

    void update(sf::Time dt, sf::Vector2f mouse_pos = {0, 0}, float force_strength = 0.f) {
        const int substeps = 2;
        sf::Time sub_dt = dt / static_cast<float>(substeps);

        for (int i = 0; i < substeps; i++) {
            for (auto& particle : particles) {
                physics::apply_mouse_force(particle, mouse_pos, force_strength, sub_dt.asSeconds());
                physics::update_particle_position(particle, sub_dt);
            }
            physics::handle_collisions(particles, boundaries);
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (const auto& particle: particles) {
            target.draw(particle.getShape(), states);
        }
    }
    

private:
    void create_boundaries() {
        float margin = 150;

        Boundary left_wall;
        left_wall.start = sf::Vector2f(margin, margin);
        left_wall.end = sf::Vector2f(margin, window_size.y - margin);
        boundaries.push_back(left_wall);

        Boundary floor;
        floor.start = sf::Vector2f(margin, window_size.y - margin);
        floor.end = sf::Vector2f(window_size.x - margin, window_size.y - margin);
        boundaries.push_back(floor);

        Boundary right_wall;
        right_wall.start = sf::Vector2f(window_size.x - margin, margin);
        right_wall.end = sf::Vector2f(window_size.x - margin, window_size.y - margin);
        boundaries.push_back(right_wall);

    }

    void create_particles(int total_num_of_particles) {
        std::random_device rd;
        std::mt19937 gen(rd());

        float margin = 150.f;
        std::uniform_real_distribution<float> dist_x(margin + 15.f, window_size.x - margin - 15.f);
        std::uniform_real_distribution<float> dist_y(margin + 15.f, margin + 300.f);  // Top area
        std::uniform_real_distribution<float> dist_vx(-1000.f, 1000.f);  // Random horizontal velocity
        std::uniform_real_distribution<float> dist_vy(-200.f, 200.f);  // Random vertical velocity
        std::uniform_int_distribution<int> dist_color(50, 255);  // Random color component

        for (int i = 0; i < total_num_of_particles; i++) {
            float x = dist_x(gen);
            float y = dist_y(gen);

            Particle p {};
            p.set_radius(10.f);
            p.set_position(sf::Vector2f(x, y));
            p.set_velocity(sf::Vector2f(dist_vx(gen), dist_vy(gen)));
            p.set_mass(1.f);
            p.set_restitution(0.75f);
            p.set_color(sf::Color(dist_color(gen), dist_color(gen), dist_color(gen)));

            particles.push_back(p);
        }
    }

    sf::Vector2u window_size;
    std::vector<Particle> particles;
    std::vector<Boundary> boundaries;
    int max_particles;

};
