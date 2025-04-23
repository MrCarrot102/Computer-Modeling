#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>
#include "body.hpp"
#include "verlet.hpp"

const double G = 1.0;
const float RADIUS = 10.0f;

void drawBodies(sf::RenderWindow& window, const std::vector<Body>& bodies) {
    for (const auto& body : bodies) {
        sf::CircleShape circle(RADIUS);
        circle.setOrigin(RADIUS, RADIUS);
        circle.setPosition(static_cast<float>(body.x), static_cast<float>(body.y));
        circle.setFillColor(sf::Color::White);
        window.draw(circle);
    }
}

sf::Vector2f calculateForce(const Body& a, const Body& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double distSq = dx * dx + dy * dy;
    double dist = std::sqrt(distSq) + 1e-5; 
    double force = (G * a.mass * b.mass) / distSq;
    return sf::Vector2f(static_cast<float>(force * dx / dist),
                        static_cast<float>(force * dy / dist));
}

double calculateAverageDistance(const std::vector<Body>& bodies) {
    double totalDistance = 0.0;
    size_t n = bodies.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            totalDistance += std::sqrt(dx * dx + dy * dy);
        }
    }
    return totalDistance / (n * (n - 1) / 2); 
}

int main() {
    const unsigned int windowSize = 800;
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Symulacja 3 ciał");
    window.setFramerateLimit(60);

    double dt = 1.0;
    int simulationChoice = 2; 

    std::vector<std::vector<Body>> simulations = {
        {
            Body(400, 300, 0.5, 1.0, 10),
            Body(450, 350, -0.5, 0.2, 10),
            Body(350, 350, 0.1, -0.3, 10)
        },
        {
            Body(400.5, 300.1, 0.5, 1.0, 10),
            Body(450.1, 350, -0.5, 0.2, 10),
            Body(349.9, 350.1, 0.1, -0.3, 10)
        },
        {
            Body(399.8, 300.2, 0.5, 1.0, 10),
            Body(450.2, 350.1, -0.5, 0.2, 10),
            Body(350.1, 350.2, 0.1, -0.3, 10)
        }
    };

    if (simulationChoice < 1 || simulationChoice > simulations.size()) {
        std::cerr << "Niepoprawny wybór symulacji!" << std::endl;
        return 1;
    }

    auto& bodies = simulations[simulationChoice - 1];
    std::vector<sf::Vector2f> forces(bodies.size());

    for (auto& body : bodies) {
        body.x_prev = body.x - body.vx * dt;
        body.y_prev = body.y - body.vy * dt;
    }

    std::ofstream file("distances.txt");
    if (!file.is_open()) {
        std::cerr << "Nie udało się otworzyć pliku!" << std::endl;
        return 1;
    }

    bool simRunning = true;
    while (window.isOpen() && simRunning) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for (size_t i = 0; i < bodies.size(); ++i) {
            sf::Vector2f totalForce(0.f, 0.f);
            for (size_t j = 0; j < bodies.size(); ++j) {
                if (i != j)
                    totalForce += calculateForce(bodies[i], bodies[j]);
            }
            forces[i] = totalForce;
        }

        for (size_t i = 0; i < bodies.size(); ++i) {
            verletStep(bodies[i], forces[i], dt);
        }

        double avgDistance = calculateAverageDistance(bodies);
        file << avgDistance << std::endl;

        window.clear(sf::Color::Black);
        drawBodies(window, bodies);
        window.display();

        static int frameCount = 0;
        frameCount++;
        if (frameCount >= 500) {
            simRunning = false;
        }
    }

    file.close();

    return 0;
}
