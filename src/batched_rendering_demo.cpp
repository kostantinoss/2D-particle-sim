// Demo: Batched Rendering with Triangles
// This demonstrates how to render multiple circles efficiently
// using a single VertexArray with Triangle primitives

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

// Simple particle structure for demo
struct DemoParticle {
    sf::Vector2f position;
    float radius;
    sf::Color color;
    
    DemoParticle(float x, float y, float r, sf::Color c)
        : position(x, y), radius(r), color(c) {}
};

class BatchedCircleRenderer {
public:
    BatchedCircleRenderer(int segments = 16) 
        : circle_segments(segments) {
        vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    }
    
    // Rebuild the vertex array for all particles
    void rebuild(const std::vector<DemoParticle>& particles) {
        vertices.clear();
        
        for (const auto& p : particles) {
            add_circle(p.position, p.radius, p.color);
        }
        
        std::cout << "Built " << particles.size() << " circles with " 
                  << vertices.getVertexCount() << " total vertices ("
                  << (vertices.getVertexCount() / 3) << " triangles)\n";
    }
    
    void draw(sf::RenderTarget& target) const {
        target.draw(vertices);
    }
    
private:
    void add_circle(sf::Vector2f center, float radius, sf::Color color) {
        float angle_inc = 2.f * M_PI / circle_segments;
        
        // Create triangles forming a circle
        for (int i = 0; i < circle_segments; ++i) {
            float angle1 = i * angle_inc;
            float angle2 = (i + 1) * angle_inc;
            
            // Calculate circumference points
            sf::Vector2f point1(
                center.x + radius * std::cos(angle1),
                center.y + radius * std::sin(angle1)
            );
            sf::Vector2f point2(
                center.x + radius * std::cos(angle2),
                center.y + radius * std::sin(angle2)
            );
            
            // Add triangle: center -> point1 -> point2
            sf::Vertex v1, v2, v3;
            v1.position = center;
            v1.color = color;
            v2.position = point1;
            v2.color = color;
            v3.position = point2;
            v3.color = color;
            vertices.append(v1);
            vertices.append(v2);
            vertices.append(v3);
        }
    }
    
    sf::VertexArray vertices;
    int circle_segments;
};

int main() {
    // Create window (SFML 3.0 syntax)
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Batched Circle Rendering Demo");
    window.setFramerateLimit(60);
    
    // Create some demo particles
    std::vector<DemoParticle> particles;
    particles.emplace_back(200.f, 200.f, 80.f, sf::Color::Red);
    particles.emplace_back(400.f, 300.f, 60.f, sf::Color::Green);
    particles.emplace_back(600.f, 250.f, 70.f, sf::Color::Blue);
    
    // Create renderer
    BatchedCircleRenderer renderer(32);  // 32 segments per circle
    renderer.rebuild(particles);
    
    std::cout << "\n=== Batched Rendering Demo ===\n";
    std::cout << "Drawing " << particles.size() << " circles with ONE draw call!\n";
    std::cout << "Press ESC to exit\n\n";
    
    // For comparison, let's also show the "old way"
    std::vector<sf::CircleShape> old_way_circles;
    for (const auto& p : particles) {
        sf::CircleShape shape(p.radius);
        shape.setPosition(sf::Vector2f(p.position.x - p.radius, p.position.y - p.radius));
        shape.setFillColor(p.color);
        shape.setPointCount(32);
        old_way_circles.push_back(shape);
    }
    
    // Toggle between methods
    bool use_batched = true;
    sf::Font font;
    std::optional<sf::Text> info_text;
    
    // Try to load a font (optional, won't crash if not found)
    if (font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        info_text.emplace(font);
        info_text->setCharacterSize(20);
        info_text->setFillColor(sf::Color::White);
        info_text->setPosition(sf::Vector2f(10.f, 10.f));
    }
    
    sf::Clock clock;
    int frame_count = 0;
    float fps = 0.f;
    
    while (window.isOpen()) {
        // Handle events
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (event->is<sf::Event::KeyPressed>()) {
                auto key_event = event->getIf<sf::Event::KeyPressed>();
                if (key_event->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
                if (key_event->code == sf::Keyboard::Key::Space) {
                    use_batched = !use_batched;
                    std::cout << "Switched to: " 
                              << (use_batched ? "BATCHED" : "INDIVIDUAL") 
                              << " rendering\n";
                }
            }
        }
        
        // Calculate FPS
        frame_count++;
        if (clock.getElapsedTime().asSeconds() >= 1.f) {
            fps = frame_count / clock.getElapsedTime().asSeconds();
            frame_count = 0;
            clock.restart();
        }
        
        // Update info text
        if (info_text) {
            std::string info = "Method: ";
            info += use_batched ? "BATCHED (1 draw call)" : "INDIVIDUAL (3 draw calls)";
            info += "\nFPS: " + std::to_string(static_cast<int>(fps));
            info += "\nPress SPACE to toggle methods";
            info += "\nPress ESC to exit";
            info_text->setString(info);
        }
        
        // Draw
        window.clear(sf::Color(30, 30, 30));
        
        if (use_batched) {
            // NEW WAY: Single draw call for all circles
            renderer.draw(window);
        } else {
            // OLD WAY: Multiple draw calls
            for (const auto& circle : old_way_circles) {
                window.draw(circle);
            }
        }
        
        // Draw info text
        if (info_text) {
            window.draw(*info_text);
        }
        
        window.display();
    }
    
    return 0;
}

