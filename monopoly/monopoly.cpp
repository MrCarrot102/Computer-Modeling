#include <iostream> 
#include <random> 
#include <vector> 



void monopoly(int rolls, bool jail, std::vector<double>& probabilities) {
    const int board_size = 40; 
    int position = 0; 
    std::vector<int> visits(board_size,0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dice(1,6); 

    for(int i = 0; i < rolls; ++i){
        int roll = dice(gen) + dice(gen);
        position = (position + roll) % board_size; 

        if(jail && position == 30) 
            position = 10; 
    

    
        visits[position]++;
    }

    for(int i = 0; i < board_size; ++i)
        probabilities[i] = static_cast<double>(visits[i])/rolls; 
}


