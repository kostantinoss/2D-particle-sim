#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>


class Particle
{
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float radius {0.f};
    float mass {1.f};
    float restitution {0.8f};
    sf::CircleShape shape;

public:
    Particle() {}

    const sf::CircleShape& get_shape() const {
        return shape;
    }

    sf::Vector2f get_position() const {
        return position;
    }

    void set_position(sf::Vector2f p) {
        position = p;
        shape.setPosition(p);
    }

    float get_radius() const {
        return radius;
    }

    void set_radius(float r) {
        radius = r;
        shape.setRadius(r);
        shape.setOrigin({r, r});  // Center the origin
    }

    float get_mass() const {
        return mass;
    }

    void set_mass(float m) {
        mass = m;
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

    void set_acceleration(sf::Vector2f new_acceleration) {
        acceleration = new_acceleration;
    }

    float get_restitution() const {
        return restitution;
    }

    void set_restitution(float r) {
        restitution = r;
    }

    sf::Color get_color() const {
        return shape.getFillColor();
    }

    void set_color(sf::Color new_color) {
        shape.setFillColor(new_color);
    }

};
