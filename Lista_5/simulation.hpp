#pragma once 
#include "verlet.hpp"
#include <fstream> 


float average_distace(const std::vector<Body>& bodies){
    float total = 0.0f; 
    total += std::hypot(bodies[0].position.x - bodies[1].position.x, 
            bodies[0].position.y - bodies[1].position.y);
    total += std::hypot(bodies[0].position.x - bodies[2].position.x, 
            bodies[0].position.y - bodies[2].position.y); 
    total += std::hypot(bodies[1].position.x - bodies[2].position.x, 
            bodies[1].position.y - bodies[2].position.y); 
    return total / 3.0f; 
}

void run_simulation(std::vector<Body> bodies, const std::string& filename, int steps, float dt){
    std::ofstream out(filename); 
    compute_accelerations(bodies); 
    for(int i = 0; i < steps; ++i){
        verlet_step(bodies, dt); 
        out << average_distace(bodies) << "\n";
    }
    out.close(); 
}

