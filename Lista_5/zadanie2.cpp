#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>

constexpr float PI = 3.141592653589793f;
constexpr float G = 1.0f;
constexpr float MASS = 1.0f;
constexpr float DT = 0.0015f;
constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 800;
constexpr unsigned TRAIL_LENGTH = 500;
constexpr unsigned SIMULATION_STEPS = 3000;

struct Body {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f prev_position;
    sf::Color color;
    std::vector<sf::Vertex> trail;

    Body(sf::Vector2f pos, sf::Vector2f vel, sf::Color col) 
        : position(pos), velocity(vel), prev_position(pos), color(col) {}
};

class ThreeBodySystem {
private:
    std::vector<Body> bodies;
    unsigned step_count = 0;

    sf::Vector2f computeForce(const Body& a, const Body& b) const {
        const sf::Vector2f delta = a.position - b.position;
        const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (distance < 0.1f) return {0, 0};

        const float force = G * MASS * MASS / (distance * distance);
        return (delta / distance) * force;
    }

public:
    ThreeBodySystem() {

        bodies.emplace_back(sf::Vector2f{ 0.97000436f, -0.24308753f}, 
                           sf::Vector2f{ 0.466203685f, 0.432365730f}, 
                           sf::Color::Red);
        
        bodies.emplace_back(sf::Vector2f{-0.97000436f,  0.24308753f}, 
                           sf::Vector2f{ 0.466203685f, 0.432365730f}, 
                           sf::Color::Green);
        
        bodies.emplace_back(sf::Vector2f{0.0f, 0.0f}, 
                           sf::Vector2f{-0.93240737f, -0.86473146f}, 
                           sf::Color::Blue);
    }

    void update() {
        if (step_count++ >= SIMULATION_STEPS) return;

  
        std::vector<sf::Vector2f> forces(bodies.size(), {0, 0});
        
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                const sf::Vector2f force = computeForce(bodies[i], bodies[j]);
                forces[i] -= force;
                forces[j] += force;
            }
        }


        for (size_t i = 0; i < bodies.size(); ++i) {
            Body& body = bodies[i];
            
            if (step_count == 1) {

                body.velocity += forces[i] / MASS * DT;
                body.prev_position = body.position;
                body.position += body.velocity * DT;
            } else {

                const sf::Vector2f new_position = 
                    2.0f * body.position - body.prev_position + 
                    (forces[i] / MASS) * DT * DT;
                
                body.prev_position = body.position;
                body.position = new_position;
                body.velocity = (body.position - body.prev_position) / (2.0f * DT);
            }


            const sf::Vector2f screen_pos = toScreenCoordinates(body.position);
            body.trail.emplace_back(screen_pos, body.color);
            
            if (body.trail.size() > TRAIL_LENGTH) {
                body.trail.erase(body.trail.begin());
            }
        }
    }

    void draw(sf::RenderWindow& window) const {

        for (const auto& body : bodies) {
            if (!body.trail.empty()) {
                window.draw(body.trail.data(), body.trail.size(), sf::LineStrip);
            }
        }


        constexpr float BODY_RADIUS = 8.0f;
        for (const auto& body : bodies) {
            sf::CircleShape shape(BODY_RADIUS);
            shape.setFillColor(body.color);
            shape.setOrigin(BODY_RADIUS, BODY_RADIUS);
            shape.setPosition(toScreenCoordinates(body.position));
            window.draw(shape);
        }

    }

private:
    sf::Vector2f toScreenCoordinates(sf::Vector2f pos) const {
        constexpr float SCALE = 250.0f;
        return {
            pos.x * SCALE + WINDOW_WIDTH / 2.0f,
            pos.y * SCALE + WINDOW_HEIGHT / 2.0f
        };
    }
};

int main() {
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
        "Symulacja problemu trzech ciał"
    );
    window.setFramerateLimit(60);

    ThreeBodySystem system;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        system.update();

        window.clear(sf::Color::Black);
        system.draw(window);
        window.display();
    }

    return 0;
}