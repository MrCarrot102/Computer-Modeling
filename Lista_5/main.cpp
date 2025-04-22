#include <iostream>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

const double G = 6.67430e-11;
const double AU = 149.6e9; // 1 jednostka astronomiczna [m]
const double year = 365.25 * 86400; // 1 rok [s]
const double dt = 0.01 * year; // 3.65 dnia
const int steps = 10000; 
struct Body {
    double mass;
    sf::Vector2<double> position;
    sf::Vector2<double> velocity;
    sf::Vector2<double> acceleration;
    sf::Color color;

    Body(double m, double x, double y, double vx, double vy, sf::Color c)
        : mass(m), position(x, y), velocity(vx, vy), acceleration(0, 0), color(c) {}
};


void computeAccelerations(std::vector<Body>& bodies) {
    const double softening = 1e8; // [m]
    
    for(auto &body : bodies) body.acceleration = {0, 0};

    for(size_t i = 0; i < bodies.size(); ++i) {
        for(size_t j = i + 1; j < bodies.size(); ++j) {
            double dx = bodies[j].position.x - bodies[i].position.x;
            double dy = bodies[j].position.y - bodies[i].position.y;
            double r = std::sqrt(dx*dx + dy*dy + softening*softening);
            double r3 = r * r * r;
            
            double force = G / r3;

            bodies[i].acceleration.x += force * dx * bodies[j].mass;
            bodies[i].acceleration.y += force * dy * bodies[j].mass;

            bodies[j].acceleration.x -= force * dx * bodies[i].mass;
            bodies[j].acceleration.y -= force * dy * bodies[i].mass;
        }
    }
}

void verletStep(std::vector<Body>& bodies) {
    // Krok 1: Aktualizacja pozycji
    for(auto& body : bodies) {
        body.position.x += body.velocity.x * dt + 0.5 * body.acceleration.x * dt * dt;
        body.position.y += body.velocity.y * dt + 0.5 * body.acceleration.y * dt * dt;
    }

    // Zapisz stare przyspieszenia
    std::vector<sf::Vector2<double>> old_accelerations;
    for(const auto& body : bodies) {
        old_accelerations.push_back(body.acceleration);
    }

    // Krok 2: Oblicz nowe przyspieszenia
    computeAccelerations(bodies);

    // Krok 3: Aktualizacja prędkości
    for(size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].velocity.x += 0.5 * (old_accelerations[i].x + bodies[i].acceleration.x) * dt;
        bodies[i].velocity.y += 0.5 * (old_accelerations[i].y + bodies[i].acceleration.y) * dt;
    }
}

double calculateAverageDistance(const std::vector<Body>& bodies) {
    double total_distance = 0;
    int count = 0;

    for(size_t i = 0; i < bodies.size(); ++i) {
        for(size_t j = i + 1; j < bodies.size(); ++j) {
            double dx = bodies[j].position.x - bodies[i].position.x;
            double dy = bodies[j].position.y - bodies[i].position.y;
            total_distance += std::sqrt(dx*dx + dy*dy);
            count++;
        }
    }
    return total_distance / count;
}

void runSimulation(const std::vector<Body>& initialBodies, const std::string& title) {
    std::vector<Body> bodies = initialBodies;
    std::vector<double> time_points;
    std::vector<double> avg_distances;

    sf::RenderWindow window(sf::VideoMode(800, 800), title);
    window.setFramerateLimit(60);

    auto scale = [](double val, double min, double max) {
        return (val - min) / (max - min) * 700 + 50;
    };

    for(int step = 0; step < steps && window.isOpen(); ++step) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        verletStep(bodies);

        if (step % 10 == 0) {
            time_points.push_back(step * dt / 86400); // Poprawione: 86400 zamiast 8640
            avg_distances.push_back(calculateAverageDistance(bodies));
        }

        window.clear(sf::Color::Black);

        // Poprawione obliczanie min/max
        double min_x = bodies[0].position.x, max_x = bodies[0].position.x;
        double min_y = bodies[0].position.y, max_y = bodies[0].position.y;
        for(const auto& body : bodies) {
            min_x = std::min(min_x, body.position.x);
            max_x = std::max(max_x, body.position.x);
            min_y = std::min(min_y, body.position.y);
            max_y = std::max(max_y, body.position.y);
        }

        // Dodanie marginesów
        double margin = 1e10;
        min_x -= margin; max_x += margin;
        min_y -= margin; max_y += margin;

        for(const auto& body : bodies) {
            sf::CircleShape shape(10);
            shape.setFillColor(body.color);
            double x = scale(body.position.x, min_x, max_x);
            double y = scale(body.position.y, min_y, max_y);
            shape.setPosition(x, y);
            window.draw(shape);
        }

        window.display();
    }

    // Wykres z obsługą błędów
    try {
        if(!time_points.empty() && !avg_distances.empty()) {
            plt::figure();
            plt::title(title + " - Średnia odległość między ciałami");
            plt::xlabel("Czas (dni)");
            plt::ylabel("Średnia odległość (m)");
            plt::plot(time_points, avg_distances);
            plt::grid(true);
            
            std::string filename = title + "_distance.png";
            std::replace(filename.begin(), filename.end(), ' ', '_');
            plt::save(filename);
            plt::close();
        } else {
            std::cerr << "Brak danych do wykresu dla " << title << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd przy generowaniu wykresu: " << e.what() << std::endl;
    }
}
int main(){
    // rozne konfiguracje poczatkowe (efekt motyla)
    std::vector<std::vector<Body>> initialConditions = {
        // Konfiguracja 1
        {
            Body(1.989e30, 0, 0, 0, 0, sf::Color::Yellow), // Słońce
            Body(5.972e24, 149.6e9, 0, 0, 29.78e3, sf::Color::Blue), // Ziemia
            Body(6.417e23, 227.9e9, 0, 0, 24.07e3, sf::Color::Red) // Mars
        },
        // Konfiguracja 2 (lekko zmieniona pozycja Ziemi)
        {
            Body(1.989e30, 0, 0, 0, 0, sf::Color::Yellow),
            Body(5.972e24, 149.6e9, 1e6, 0, 29.78e3, sf::Color::Blue),
            Body(6.417e23, 227.9e9, 0, 0, 24.07e3, sf::Color::Red)
        },
        // Konfiguracja 3 (lekko zmieniona prędkość Marsa)
        {
            Body(1.989e30, 0, 0, 0, 0, sf::Color::Yellow),
            Body(5.972e24, 149.6e9, 0, 0, 29.78e3, sf::Color::Blue),
            Body(6.417e23, 227.9e9, 0, 0, 24.07e3 + 100, sf::Color::Red)
        }
    };

    // symulacje dla kazdej konfuguracji 
    for( size_t i = 0; i < initialConditions.size(); ++i){
        runSimulation(initialConditions[i], "Konfiguracja " + std::to_string(i+1));
    }

    return 0; 
}

