#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream> 
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <thread> 
#include <chrono> 
#include <fstream> 
#include <stdexcept>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

class Life{
    private: 
        int ticks; 
        bool pass;
        int height; 
        int width; 
        bool **board; 
        bool **buffer; 
        std::vector<std::pair<float, float>> liveCoords; 
        void init();
        void lives(int x, int y); 
        void dies(int x, int y); 
        bool isAlive(bool **curr, int x, int y); 
        int checkNeighbors(bool **curr, int x, int y);
        void evaluatePosition(bool **curr, int x, int y); 

    public: 
        Life(int w = 100, int h = 100, int seed = 1337); 
        Life(const Life& life); 
        ~Life(); 
        std::vector<std::pair<float, float>> doTick();
        Life& operator=(const Life& life);
        void setCell(int x, int y, bool state){
           if ( x < 0 || x >= width || y < 0 || y >= height) return;

           board[y][x] = state;
           buffer[y][x] = state; 
         }
        std::vector<std::pair<float, float>> getLiveCells(){
            std::vector<std::pair<float, float>> result; 
            bool** current = pass ? buffer : board; 
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    if(current[y][x]){
                        result.push_back(std::make_pair(x, y));
                    }
                }
            }
            return result; 
        }
        void syncBuffers(); 

        void saveToFile(const std::string& filename){
            std::ofstream file(filename);
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    file << (board[y][x] ? "O" : ".");
                }
                file << "\n";
            }
        }

        void loadFromFile(const std::string& filename){
            std::ifstream file(filename); 
            std::string line; 
            int y = 0;
            while(getline(file, line)){
                for(int x = 0; x < width && x < line.size(); x++){
                    setCell(x , y, line[x] == 'O');
                }
                y++;
            }
        }
};

void Life::syncBuffers(){
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            buffer[y][x] = board[y][x];
        }
    }
    pass = false; 
}

void Life::init(){
    board = new bool*[height];
    buffer = new bool*[height];
    for(int y = 0; y < height; y++){
        board[y] = new bool[width]();
        buffer[y] = new bool[width]();
    }
}

Life::Life(int w, int h, int seed) : width(w), height(h){
    init(); 
    if(seed > 0){
        for(int i = 0; i < seed; i++){
            board[rand() % height][rand() % width] = true; 
        }
    }

    pass = true; 
}

Life::Life(const Life& life){
    width = life.width; 
    height = life.height; 
    init();
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            board[y][x] = life.board[y][x];
            buffer[y][x] = life.buffer[y][x]; 
        }
    }
}


Life& Life::operator=(const Life& life){
    if(this == &life) return *this;
    for(int i = 0; i < height; i++){
        delete [] board[i]; 
        delete [] buffer[i];
    }
    delete [] board;
    delete [] buffer; 

    width = life.width;
    height = life.height; 
    init(); 
    for(int y = 0; y < height; y++){
        for(int x = 0; x <width; x++){
            board[y][x] = life.board[y][x];
            buffer[y][x] = life.buffer[y][x];
        }
    }
    return *this; 
}

Life::~Life(){
    if(board){
        for(int i = 0; i < height; i++){
            delete[] board[i]; 
        }
        delete[] board; 
    }
    if(buffer){
        for(int i = 0; i < height; i++){
            delete[] buffer[i];
        }
        delete[] buffer;
    }
}

std::vector<std::pair<float, float>> Life::doTick(){
    liveCoords.clear(); 
    bool **currentGeneration = pass ? board:buffer; 
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            evaluatePosition(currentGeneration, x, y);
        }
    }
    pass = !pass; 
    ticks++; 
    return liveCoords; 
} 

bool Life::isAlive(bool **curr, int x, int y){
    return curr[y][x]; 
}

int Life::checkNeighbors(bool **curr, int x, int y){
    int lc = 0; 
    int dx[8] = {-1, 0, 1, 1, 1, -1, 0, -1};
    int dy[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
    for(int i = 0; i < 8; i++){
        int nx = x + dx[i]; 
        int ny = y + dy[i];

        if(nx >= 0 && nx < width && ny >= 0 && ny < height)
            lc += isAlive(curr, nx, ny); 

    }
    return lc; 
}

void Life::lives(int x, int y){
    if(!pass){
        board[y][x] = true; 
    } else {
        buffer[y][x] = true; 
    }
    liveCoords.push_back(std::make_pair((float)x, (float)y)); 
}

void Life::dies(int x, int y){
    if (!pass){
        board[y][x] = false; 
    } else {
        buffer[y][x] = false; 
    }
}

void Life::evaluatePosition(bool** generation, int x, int y){
    int lc = checkNeighbors(generation, x, y);
    if(isAlive(generation, x, y)){
        if(lc == 2 || lc == 3){
            lives(x,y);
        } else {
            dies(x,y); 
        }
    } else {
        if(lc == 3){ 
            lives(x,y); 
        } else {
            dies(x,y);
        }
    }
}

class App{
    private: 
        void sleep(); 
        void drawLiveCells(); 
        void render(); 
        void handleEvent(sf::Event& event); 
        void saveDisplay();
        int width, height; 
        Life life; 
        bool isRecording; 
        int tick; 
        sf::RenderWindow* window; 
        sf::RenderTexture* texture; 
        bool isPaused = false; 
        bool isDrawing = false; 
        bool isErasing = false; 

    public: 
        App(int w = 100, int h = 100); 
        void start();
};

App::App(int w, int h){
    height = std::min(h, 300); 
    width = std::min(w, 300);  
    life = Life(width, height); 
    isRecording = false; 
    tick = 0; 
}

void App::start(){
    sf::Event event; 
    window = new sf::RenderWindow(sf::VideoMode(width*10, height*10), "The game of life"); 
    texture = new sf::RenderTexture(); 
    texture->create(width*10, height*10); 
    window->setFramerateLimit(60); 
    while(window->isOpen()){
        while(window->pollEvent(event)){
            handleEvent(event);
        }
        render();
        tick++;
    }
    delete window; 
    delete texture; 
}

    void App::handleEvent(sf::Event& event){
    if(event.type == sf::Event::Closed){ window->close();}
    if(event.type == sf::Event::KeyPressed){
        switch(event.key.code){
            case sf::Keyboard::R:
                life = Life(width, height); 
                break; 
            case sf::Keyboard::S:
                isRecording = !isRecording; 
                break; 
            case sf::Keyboard::Space:
                if(isPaused){
                    life.syncBuffers();
                }
                isPaused = !isPaused; 
                break; 
            case sf::Keyboard::C:
                life = Life(width, height, 0);
                break; 
            case sf::Keyboard::Q:
            case sf::Keyboard::Escape:
                window->close();
                break; 
            default:
                break;
        }
    }
    if(isPaused && event.type == sf::Event::MouseButtonPressed){
        if(event.mouseButton.button == sf::Mouse::Left){
            isDrawing = true;
        }
    }
    if(event.type == sf::Event::MouseButtonReleased){
        isDrawing = false; 
    }
    if(isPaused && isDrawing && event.type == sf::Event::MouseMoved){
        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        int cellX = mousePos.x / 10;
        int cellY = mousePos.y / 10; 
        if(cellX >= 0 && cellX < width && cellY >= 0 && cellY < height){
            life.setCell(cellX, cellY, true);
        }
    }
    if (isPaused && event.type == sf::Event::MouseButtonPressed){
        if(event.mouseButton.button == sf::Mouse::Left){
            isDrawing = true;
        } else if (event.mouseButton.button == sf::Mouse::Right){
            isErasing = true; 
        }
    }
    if(isPaused && isErasing && event.type == sf::Event::MouseMoved){
        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        int cellX = mousePos.x / 10; 
        int cellY = mousePos.y / 10; 
        if(cellX >= 0 && cellX < width && cellY >= 0 && cellY < height){
            life.setCell(cellX, cellY, true); 
        }
        if(cellX >= 0 && cellX < width && cellY >= 0 && cellY < height){
            if(isDrawing){
                life.setCell(cellX, cellY, true);
            } else if(isErasing){
                life.setCell(cellX, cellY, false);
            }
        }
    }
}


void App::sleep(){
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
}

void App::drawLiveCells(){
    float XSCALE = 10.0, YSCALE = 10.0; 
    sf::RectangleShape rect; 
    rect.setSize(sf::Vector2f(XSCALE, YSCALE));
    rect.setFillColor(sf::Color::White);
    auto coords = life.doTick();
    texture->clear(sf::Color::Black);
    for(auto m : coords){
        rect.setPosition(m.first*XSCALE, m.second*YSCALE); 
        texture->draw(rect); 
    }
    texture->display();
}

void App::render(){
    if(!isPaused){ 
        drawLiveCells();
    } else {
        if (isDrawing) {
             /*   sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
                int cellX = mousePos.x / 10; 
                int cellY = mousePos.y / 10; 
                if(cellX >= 0 && cellX < width && cellY >= 0 && cellY < height){
                    life.setCell(cellX, cellY, true); 
                }*/
            }
            texture->clear(sf::Color::Black);
            sf::RectangleShape rect(sf::Vector2f(10,10));
            rect.setFillColor(sf::Color::White); 

            auto coords = life.getLiveCells();
            for (auto m : coords){
                rect.setPosition(m.first * 10, m.second * 10);
                texture->draw(rect); 
                }
            texture->display(); 
        }

        window->clear();
        sf::Sprite sprite(texture->getTexture());
        window->draw(sprite); 
        window->display();
        if(!isPaused){
            sleep(); 
        }
}

void App::saveDisplay(){
    std::string name = "tick" + std::to_string(tick) + ".png";
    sf::Image image = texture->getTexture().copyToImage();
    image.saveToFile(name); 
}


class Experiment {
    private:
        int size = 100;
        double p0; 
        Life life;
        std::vector<double> time_points;
        std::vector<double> density_values;
        
    public: 
        Experiment(double probability)
            : p0(probability),
              life(size, size, 0)
        {
            srand(time(0));
            for(int y = 0; y < size; y++){
                for(int x = 0; x < size; x++){
                    if(rand() / (double)RAND_MAX < p0){
                        life.setCell(x,y,true);
                    }
                }
            }
        }
         
        void run(int max_ticks = 1000){
            for(int t = 0; t < max_ticks; t++){
                int alive = life.doTick().size();
                double density = alive / (double)(size*size);
                
                time_points.push_back(t);
                density_values.push_back(density);
                
                if(t % 100 == 0){
                    std::cout << "p0=" << p0 << " t=" << t 
                              << " density=" << density << std::endl;
                }
            }
        }
    
        const std::vector<double>& getTimePoints() const { return time_points; }
        const std::vector<double>& getDensityValues() const { return density_values; }
        double getP0() const { return p0; }
    };

    class ErrorAnalysis {
        public:
        static void runGridSizeExperiment(double p0 = 0.3, int N = 100, int Tmax = 1000) {
            std::vector<int> grid_sizes = {10, 100, 200, 500, 1000};
            std::vector<double> sem_values;
            std::vector<double> mean_densities;
        
            for (int L : grid_sizes) {
                std::vector<double> final_densities;
        
                for (int n = 0; n < N; n++) {
                    Life life(L, L, 0);
                    initializeRandomGrid(life, L, p0);
        
                    for (int t = 0; t < Tmax; t++) {
                        life.doTick();
                    }
        
                    double density = life.getLiveCells().size() / static_cast<double>(L * L);
                    final_densities.push_back(density);
                }
        
                double mean = calculateMean(final_densities);
                double sem = calculateStdDev(final_densities) / std::sqrt(N);
        
                mean_densities.push_back(mean);
                sem_values.push_back(sem);
        
                std::cout << "L=" << L << " | Średnia gęstość: " << mean 
                          << " | SEM: " << sem << "\n";
            }
        
            // Rysowanie wykresu - POPRAWIONE
            plt::figure();
            
            // Konwersja vector<int> na vector<double> dla osi X
            std::vector<double> grid_sizes_double(grid_sizes.begin(), grid_sizes.end());
            
            // Najpierw punkty
            plt::plot(grid_sizes_double, mean_densities, "o-");
            
            // Następnie słupki błędów
            plt::errorbar(grid_sizes_double, mean_densities, sem_values);
            
            // Opcjonalne ustawienia logarytmiczne
            //plt::xscale("log");
            //plt::yscale("log");
            
            plt::xlabel("Rozmiar siatki (L)");
            plt::ylabel("Średnia gęstość");
            plt::title("Zależność gęstości od rozmiaru siatki (p0=" + std::to_string(p0) + ")");
            plt::grid(true);
            
            // Zapis wykresu
            plt::save("grid_size_error_plot.png");
            plt::close();
        }
        
        private:
            static void initializeRandomGrid(Life& life, int L, double p0) {
                for (int y = 0; y < L; y++) {
                    for (int x = 0; x < L; x++) {
                        if (rand() / static_cast<double>(RAND_MAX) < p0) {
                            life.setCell(x, y, true);
                        }
                    }
                }
            }
        
            static double calculateMean(const std::vector<double>& data) {
                return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
            }
        
            static double calculateStdDev(const std::vector<double>& data) {
                double mean = calculateMean(data);
                double variance = 0.0;
                for (double x : data) {
                    variance += (x - mean) * (x - mean);
                }
                return std::sqrt(variance / data.size());
            }
        };

        int main(int argc, char* argv[]){
            std::vector<double> probabilities = {0.05, 0.1, 0.3, 0.6, 0.75, 0.8, 0.95};
            std::vector<Experiment> experiments;
            
            // Przeprowadź wszystkie eksperymenty
            for(double p0 : probabilities){
                Experiment exp(p0);
                exp.run(1000);
                experiments.push_back(exp);
            }
        
            // Stwórz jeden wspólny wykres
            plt::figure();
            plt::title("Ewolucja gęstości dla różnych p0");
            plt::xlabel("Czas (ticki)");
            plt::ylabel("Gęstość");
            plt::grid(true);
            
            // Dodaj dane z każdego eksperymentu
            for(const auto& exp : experiments){
                plt::plot(exp.getTimePoints(), 
                         exp.getDensityValues(),
                         {{"label", "p0=" + std::to_string(exp.getP0())}});
            }
            
            // Dodaj legendę i zapisz wykres
            plt::legend();
            plt::save("all_densities_plot.png");
            plt::close();
        
            // Analiza błędów (pozostała część kodu)
            ErrorAnalysis::runGridSizeExperiment(0.3, 100, 1000);
        
            // Uruchomienie wizualizacji (opcjonalne)
            App app(100,100); 
            app.start(); 
        
            return 0; 
        }