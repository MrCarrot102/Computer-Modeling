#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "body.hpp"
#include "verlet.hpp"

// Stała grawitacji
const double G = 1.0;

// Promień rysowanych ciał
const float RADIUS = 5.0f;

// Funkcja pomocnicza do rysowania
void drawBodies(sf::RenderWindow& window, const std::vector<Body>& bodies) {
    for (const auto& b : bodies) {
        sf::CircleShape circle(RADIUS);
        circle.setOrigin(RADIUS, RADIUS);
        circle.setPosition(static_cast<float>(b.x), static_cast<float>(b.y));
        circle.setFillColor(sf::Color::White);
        window.draw(circle);
    }
}

// Siła grawitacji (bardzo uproszczona)
sf::Vector2f calculateForce(const Body& a, const Body& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double distSq = dx * dx + dy * dy;
    double dist = std::sqrt(distSq) + 1e-5; // unikanie dzielenia przez 0
    double force = (G * a.mass * b.mass) / distSq;
    return sf::Vector2f(static_cast<float>(force * dx / dist),
                        static_cast<float>(force * dy / dist));
}

int main() {
    const unsigned int windowSize = 800;
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Symulacja 3 ciał");
    window.setFramerateLimit(60);

    double dt = 1.5;

    // Inicjalizacja ciał
    std::vector<Body> bodies = {
        Body(400, 300, 0.5, 0, 10),
        Body(450, 350, -0.5, 0.2, 10),
        Body(350, 350, 0.1, -0.3, 10)
    };

    // Pozycje z poprzedniego kroku do metody Verleta
    for (auto& b : bodies) {
        b.x_prev = b.x - b.vx * dt;
        b.y_prev = b.y - b.vy * dt;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Oblicz wszystkie siły i aktualizuj pozycje
        std::vector<sf::Vector2f> forces(bodies.size());

        for (size_t i = 0; i < bodies.size(); ++i) {
            sf::Vector2f totalForce(0.f, 0.f);
            for (size_t j = 0; j < bodies.size(); ++j) {
                if (i != j)
                    totalForce += calculateForce(bodies[i], bodies[j]);
            }
            forces[i] = totalForce;
        }

        // Verlet update
        for (size_t i = 0; i < bodies.size(); ++i) {
            verletStep(bodies[i], forces[i], dt);
        }

        window.clear(sf::Color::Black);
        drawBodies(window, bodies);
        window.display();
    }

    return 0;
}

