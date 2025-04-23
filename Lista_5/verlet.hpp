#pragma once
#include "body.hpp"
#include <SFML/Graphics.hpp>

void verletStep(Body& b, const sf::Vector2f& force, double dt) {
    double ax = force.x / b.mass;
    double ay = force.y / b.mass;

    double x_new = 2 * b.x - b.x_prev + ax * dt * dt;
    double y_new = 2 * b.y - b.y_prev + ay * dt * dt;

    b.x_prev = b.x;
    b.y_prev = b.y;
    b.x = x_new;
    b.y = y_new;
}
 
