#include <SFML/Graphics.hpp> 
#include <vector> 
#include <fstream> 
#include <iostream> 
#include <cmath>


const int WIDTH = 200; 
const int HEIGHT = 200; 
const float DA = 1.0f; 
const float DB = 0.5f; 
const float FEED = 0.0367f; 
const float KILL = 0.0649f; 
const float DT = 1.0f; 
const int STEPS_PER_FRAME = 10; 


struct Grid{ 
    std::vector<std::vector<float>> A, B; 

    Grid(int w, int h) : A(w, std::vector<float>(h, 1.0f)), B(w, std::vector<float>(h, 0.0f)){
        for(int i = w / 2 - 10; i < w / 2 + 10; i++){
            for(int j =  h / 2 - 10; j < h / 2 + 10; j++)
                B[i][j] = 1.0f; 
        }
    }

    float laplacian(const std::vector<std::vector<float>>& arr, int x, int y){
        float sum = arr[x][y] * -1.0f; 

        if (x > 0) sum += arr[x-1][y] * 0.2f; 
        if (x < WIDTH - 1) sum += arr[x+1][y] * 0.2f;
        if (y > 0) sum += arr[x][y-1] * 0.2f; 
        if (y < HEIGHT - 1) sum += arr[x][y+1] * 0.2f; 

        if (x > 0 && y > 0) sum += arr[x-1][y-1] *0.05f; 
        if (x > 0 && y < HEIGHT - 1) sum += arr[x-1][y+1] * 0.05f; 
        if (x < WIDTH - 1 && y > 0) sum += arr[x+1][y-1] * 0.05f; 
        if (x < WIDTH - 1 && y < HEIGHT -1) sum += arr[x+1][y+1] * 0.05f; 

        return sum; 

    }

    void update() {
        std::vector<std::vector<float>> newA = A, newB = B; 
        
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                float LA = laplacian(A, x, y); 
                float LB = laplacian(B, x, y); 

                float reaction = A[x][y] * B[x][y] *B[x][y]; 

                newA[x][y] = A[x][y] + (DA * LA - reaction + FEED * (1 - A[x][y])) * DT;
                newB[x][y] = B[x][y] + (DB * LB + reaction - (KILL + FEED) * B[x][y]) * DT; 

                newA[x][y] = std::clamp(newA[x][y], 0.0f, 1.0f);
                newB[x][y] = std::clamp(newB[x][y], 0.0f, 1.0f);
            }
        }
        A = newA; 
        B = newB;
    }
};


void saveDesityToCSV(const Grid& grid, int iteration){ 
    std::ofstream file("density_data.csv", std::ios::app);
    float totalDensity = 0.0f; 

    for(int x = 0; x < WIDTH; x++){
        for(int y = 0; y < HEIGHT; y++){
            totalDensity += grid.B[x][y];
        }
    }

    file << iteration << "," << totalDensity / (WIDTH * HEIGHT) << "\n";
}


int main(){ 
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Turing Pattern Simulation");
    Grid grid(WIDTH, HEIGHT);

    std::ofstream file("density_data.csv");
    file << "Iteration,Density\n";

    while(window.isOpen()){
        sf::Event event; 
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed)
                window.close(); 
        }

        for(int i = 0; i < STEPS_PER_FRAME; i++){
            grid.update(); 
        }

        saveDesityToCSV(grid, STEPS_PER_FRAME); 

        sf::Image image; 
        image.create(WIDTH, HEIGHT); 
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                float av = std::clamp(grid.A[x][y] * 255, 0.0f, 255.0f);
                float bv = std::clamp(grid.B[x][y] * 255, 0.0f, 255.0f);
                image.setPixel(x, y, sf::Color(av, bv, 128)); 
            }
        }

        sf::Texture texture; 
        texture.loadFromImage(image); 
        sf::Sprite sprite(texture); 

        window.clear(); 
        window.draw(sprite); 
        window.display(); 
    }
    return 0;
}