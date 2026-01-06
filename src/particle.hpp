#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <math.h>


class Particle
{
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float radius;
    float mass;
    float restitution;
    sf::Color color;

    int circumferance_points {32};
    sf::VertexArray vertices;

    // sf::VertexArray draw_particle(sf::Vector2f center_position) {
    //     int total_points = circumferance_points + 2;    // 1 center, 1 to close the circle
        
    //     sf::VertexArray circle(sf::PrimitiveType::TriangleFan, total_points); 

    //     circle[0].position = position;
    //     circle[0].color = color;  // Use particle's color

    //     float angle_inc = 2 * M_PI / circumferance_points;   // -1 because the first is the center
    //     float angle = 0;
    //     for (int i = 1; i <= circumferance_points + 1; i++) {
    //         float x = radius * cos(angle);
    //         float y = radius * sin(angle);
    //         circle[i].position = circle[0].position + sf::Vector2f({x, y}); 
    //         circle[i].color = color;  // Use particle's color
    //         angle += angle_inc;
    //     }
        
    //     return circle;
    // }

public:
    Particle() {
        // vertices = draw_particle(position);
    }
    
    const sf::VertexArray& getVertices() const {
        return vertices;
    }

    // void update(sf::Time elapsed) {
    //     float dt = elapsed.asSeconds();
    //     position = position + (velocity * dt) + (acceleration * (dt*dt * 0.5f));
    //     sf::Vector2f new_acceleration = apply_forces();
    //     velocity = velocity + (acceleration + new_acceleration)*(dt * 0.5f);

    //     vertices = draw_particle(position);
    // }

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
    void set_radius(float r) {
        radius = r;
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
    float get_restitution() const {
        return restitution;
    }
    void set_restitution(float r) {
        restitution = r;
    }
    
};