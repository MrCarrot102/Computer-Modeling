#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

const double dt = 0.0015;
const double m = 1.0;
const double G = 1.0;
const int n_steps = 2000;
const int N = 3;
const int W = 800;
const int H = 800;

struct Body {
    double x, y;
    double vx, vy;
    double xm1, ym1; 
    sf::Color color;
    sf::CircleShape shape;

    Body(double x, double y, double vx, double vy, sf::Color col) 
        : x(x), y(y), vx(vx), vy(vy), xm1(0), ym1(0), color(col) {
        shape.setRadius(5);
        shape.setFillColor(col);
    }
};

class ThreeBodySimulation {
private:
    std::vector<Body> bodies;
    int step = 0;
    std::vector<double> avg_dist_history;
    std::vector<int> steps;

public:
    ThreeBodySimulation(const std::vector<double>& x0, 
                       const std::vector<double>& y0,
                       const std::vector<double>& vx0,
                       const std::vector<double>& vy0) {

        sf::Color colors[] = {sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
        for (int i = 0; i < N; ++i) {
            bodies.emplace_back(x0[i], y0[i], vx0[i], vy0[i], colors[i]);
        }
    }

    double avg_distance() {
        double d01 = sqrt(pow(bodies[0].x - bodies[1].x, 2) + pow(bodies[0].y - bodies[1].y, 2));
        double d02 = sqrt(pow(bodies[0].x - bodies[2].x, 2) + pow(bodies[0].y - bodies[2].y, 2));
        double d12 = sqrt(pow(bodies[1].x - bodies[2].x, 2) + pow(bodies[1].y - bodies[2].y, 2));
        return (d01 + d02 + d12) / 3.0;
    }

    void simulation_step() {
        step++;
        std::vector<double> fx(N, 0.0), fy(N, 0.0);

  
        for (int i = 0; i < N; ++i) {
            for (int j = i + 1; j < N; ++j) {
                double dx = bodies[i].x - bodies[j].x;
                double dy = bodies[i].y - bodies[j].y;
                double d = sqrt(dx*dx + dy*dy);
                if (d != 0) {
                    double rx = dx / d;
                    double ry = dy / d;
                    double F = G * m * m / (d*d);
                    double Fx = rx * F;
                    double Fy = ry * F;
                    fx[i] -= Fx;
                    fy[i] -= Fy;
                    fx[j] += Fx;
                    fy[j] += Fy;
                }
            }
        }


        if (step == 1) {
            for (int b = 0; b < N; ++b) {
                bodies[b].vx += (fx[b] / m) * dt;
                bodies[b].vy += (fy[b] / m) * dt;
                bodies[b].xm1 = bodies[b].x;
                bodies[b].ym1 = bodies[b].y;
                bodies[b].x += bodies[b].vx * dt;
                bodies[b].y += bodies[b].vy * dt;
            }
        } else {
            // Metoda Verlet
            for (int b = 0; b < N; ++b) {
                double new_x = 2 * bodies[b].x - bodies[b].xm1 + (dt*dt * fx[b] / m);
                double new_y = 2 * bodies[b].y - bodies[b].ym1 + (dt*dt * fy[b] / m);
                
                bodies[b].xm1 = bodies[b].x;
                bodies[b].ym1 = bodies[b].y;
                bodies[b].x = new_x;
                bodies[b].y = new_y;
                bodies[b].vx = (bodies[b].x - bodies[b].xm1) / (2 * dt);
                bodies[b].vy = (bodies[b].y - bodies[b].ym1) / (2 * dt);
            }
        }


        avg_dist_history.push_back(avg_distance());
        steps.push_back(step);
    }

    void draw(sf::RenderWindow& window) {
        for (auto& body : bodies) {
            body.shape.setPosition(98 * body.x + W/2 - 5, 98 * body.y + H/2 - 5);
            window.draw(body.shape);
        }
    }

    void plot_results(const std::string& title) {
        plt::named_plot(title, steps, avg_dist_history);
    }

    const std::vector<double>& get_avg_dist_history() const { return avg_dist_history; }
    const std::vector<int>& get_steps() const { return steps; }
};

int main() {

    std::vector<double> x0_base = {0.5 - 0.2, 0.5 + 0.2, 0.5};
    std::vector<double> y0_base = {0.5 - 0.2, 0.5 - 0.2, 0.5 + 0.2};
    std::vector<double> vx0_base = {0.93240737 / 2.0, 0.93240737 / 2.0, -0.93240737};
    std::vector<double> vy0_base = {0.86473146 / 2.0, 0.86473146 / 2.0, -0.86473146};

    double perturbation = 0.001;

    ThreeBodySimulation sim1(x0_base, y0_base, vx0_base, vy0_base);
    
    auto x0_2 = x0_base;
    x0_2[0] += perturbation;
    ThreeBodySimulation sim2(x0_2, y0_base, vx0_base, vy0_base);
    
    auto y0_3 = y0_base;
    y0_3[1] += perturbation;
    ThreeBodySimulation sim3(x0_base, y0_3, vx0_base, vy0_base);

    sf::RenderWindow window(sf::VideoMode(W, H), "Problem trzech ciał");
    window.setFramerateLimit(60);

    sf::Text stepText;
    stepText.setCharacterSize(20);
    stepText.setFillColor(sf::Color::Blue);
    stepText.setPosition(10, H - 40);

    // Główna pętla
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sim1.get_steps().size() < n_steps) {
            sim1.simulation_step();
            sim2.simulation_step();
            sim3.simulation_step();

            stepText.setString("Krok: " + std::to_string(sim1.get_steps().back()));
        }

        window.clear(sf::Color::White);
        sim1.draw(window);
        sim2.draw(window);
        sim3.draw(window);
        window.draw(stepText);
        window.display();
    }

    plt::figure_size(1000, 600);
    sim1.plot_results("Konfiguracja referencyjna");
    sim2.plot_results("Wariant z modyfikacją położenia X");
    sim3.plot_results("Wariant z modyfikacją położenia Y");
    plt::xlabel("Iteracje symulacji");
    plt::ylabel("Średnia odległość pomiędzy obiektami");
    plt::title("Analiza wrażliwości układu trzech ciał na warunki początkowe");
    plt::legend();
    plt::grid(true);
    plt::show();
    return 0;

}
