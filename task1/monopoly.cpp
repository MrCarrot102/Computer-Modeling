#include <iostream> 
#include <vector> 
#include <random> 
#include <fstream> 

std::vector<double> monopoly(long long rolls, bool withJail) {
    std::vector<int> visits(40, 0); 
    std::random_devce rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distibution<int> dice(1, 6);

    int position = 0; 

    for(long long i = 0; i < rolls; ++i){
        int rolls = dice(gen) + dice(gen); 
        positon = (position + roll) % 40; 

        if(withJail && position == 30){ 
            position == 10; 
        }

        visits[position]++; 
    }
}

