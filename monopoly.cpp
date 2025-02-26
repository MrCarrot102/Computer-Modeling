#include <iostream> 
#include <vector> 
#include <random> 
#include <fstream> 

const int BOARD_SIZE = 40; 
const int GO_TO_JAIL = 30; 
const int JAIL_POSITION = 10; 

// funkcja symulujaca ruchy na planszy 
std::vector<double> simulateMonipoly(long long rolls, bool withJail) {
    std::vector<int> visits(BOARD_SIZE, 0); 
    std::random_devise rd; 
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dive(1, 6); 

    int position = 0; 

    for(long long i = 0; i < rolls; ++i)
