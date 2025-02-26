#include <iostream>
#include <random> 
#include <vector> 
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp; _

void Monopoly(int rolls, bool jail, std::vector<double>& probabilites) { 
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

    for(int i = 0; i < board_size; ++i){
        probabilites[i] = static_cast<double>(visits[i]) / rolls; 
    }
}

void plot_probabilites(const std::vector<double>& probabilites, const std::string& title) { 
    std::vector<int> x(probabilites.size());
    for(int i = 0; i < probabilites.size(); ++i)
        x[i] = i; 

    plt::bar(x, probabilites); 
    plt::title(title);
    plt::xlabel("Pole planszy");
    plt::ylabel("Prawdopodobieństwo p(i)");
    plt::show();
}





int main() {
    int a = 100, b = 1'000'000; 
    
    std::vector<double> probabilites1(40,0.0);
    std::vector<double> probabilites2(40,0.0);
    // symulacja bez więzienia 
    Monopoly(a, false, probabilites1);
    plot_probabilites(probabilites_no_jail, "Ruch po planszy bez więzienia (100 rzutów)"); 



    return 0; 

}


