#include <SFML/Graphics.hpp>
#include <vector> 
#include <fstream> 
#include <iostream> 
#include <cmath> 

const int WIDTH = 200; 
const int HEIGHT = 200; 
const float DA = 1.0f; 
const float DB = 0.5f; 
const float FEED = 0.055f; 
const KILL = 0.062f; 
const float DT = 1.0f; 
const float DX = 1.0f; 
const int STEPS_PER_FRAME = 10; 

struct Grid{
    std::vector<std::vector<float>> A, B; 

    Grid(int w, int h) : A(w, std::vector<float>(h, 1.0f)), B(w, std::vector<float>(h, 0.0f)){
        // maly obszar B w srodku 
        for(int i = w / 2 - 10; i < w / 2 + 10; i++){
            for(int j = h / 2 - 10; j < h / 2 + 10; j++)
                B[i][j] = 1.0f; 
        }
    }

    float laplacian(const std::vector<std::vector<float>>& arr, int x, int y){
        float sum = -arr[x][y] * 4.0f; 
        if ( x > 0 ) sum += arr[x-1][y]; 
        if ( x < WIDTH - 1 ) sum += arr[x+1][y]; 
        if ( y > 0 ) sum += arr[x][y-1]; 
        if ( y < HEIGHT ) sum += arr[x][y+1];
        return sum; 
    }

    void update(){
        std::vector<std::vector<float>> newA = A, newB = B; 

        for(int x = 1; x < WIDTH; x++){
            for(int y = 1; y < HEIGHT; y++){
                float LA = laplacian(A, x, y); 
                float LB = laplacian(B, x, y); 

                float reaction = A[x][y] * B[x][y] * B[x][y]; 
                newA[x][y] = A[x][y] + (DA * LA - reaction + FEED * (1 - A[x][y])) * DT; 
                newB[x][y] = B[x][y] + (DB * LB + reaction - (KILL + FEED) * B[x][y] )) * DT; 

                newA[x][y] = std::clamp(newA[x][y], 0.0f, 1.0f); 
                newB[x][y] = std::clamp(newB[x][y], 0.0f, 1.0f);
            }
        }
        A = newA; 
        B = newB; 
    }
};