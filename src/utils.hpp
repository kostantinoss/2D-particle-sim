// Utils for the particle simulation

#include <cmath>
#include <SFML/System/Vector2.hpp>
#include "particle.hpp"


namespace utils {
    float euclidean_distance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
        return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
    }
    float dot_product(const sf::Vector2f& v1, const sf::Vector2f& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }
}

