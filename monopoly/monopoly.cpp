#include <iostream>
#include <random> 
#include <vector> 


void Monopoly(int rolls, bool jail) { 
    const int board_size = 40; 
    std::vector<int> visits(board_size, 0); 
    int position = 0; 

    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> dice(1, 6); 


    for(int i = 0; i < rolls; ++i){
        int roll = dice(gen) + dice(gen); 

        position = (position + roll) % board_size; 

        if(jail && position == 30) { position = 10; } 

        // std::cout << position << "\n";
        
        visits[position]++; 
    }
}

int main() {
    int a = 100, b = 1'000'000; 

    Monopoly(a, false); 
}


