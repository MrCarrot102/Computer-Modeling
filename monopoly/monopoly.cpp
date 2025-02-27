#include <iostream>
#include <random> 
#include <vector> 
#include <fstream> 

void Monopoly(int rolls, bool jail){
    int board_size = 40;
    int jail_position = 30; 
    std::vector<int> visits(board_size, 0);
    
    int position = 0; 

    std::ofstream file("data.csv");

    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> dice(1, 6); 

    for(int i = 0; i < rolls; i++){
        int roll = dice(gen) + dice(gen); 
        
        position = (position + roll) % board_size; 
        
        if(jail && position == jail_position){
            visits[jail_position]++;
            position = 10;
        }
        visits[position]++; 
    }

    for(int i = 0; i < board_size; i++){
        file << i << "," << visits[i] << "\n";
    }
    file.close(); 
}

int main(){
    int rolls1 = 100, rolls2 = 1'000'000; 
    
    // checking for jail = false 
    Monopoly(rolls1, false); 
   // Monopoly(rolls2, false); 
    
    // for jail = true
    //Monopoly(rolls2, true);
//    Monopoly(rolls2, true);

    return 0; 
}



