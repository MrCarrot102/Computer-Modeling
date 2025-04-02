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

Life::Life(int w, int h, int seed){
    width = w; 
    height = h; 
    init(); 
    for(int i = 0; i < seed; i++){
        board[rand() % height][rand() % width] = true; 
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
    height = h; 
    width = w; 
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


class DataLogger{
    private:
        std::string base_filename; 
        std::vector<double> time_points; 
        std::vector<double> density_values; 
        int current_size; 

    public: 
        DataLogger(const std::string& filename_base)
            : base_filename(filename_base), current_size(0){}

        void record(double time, double density){
            time_points.push_back(time); 
            density_values.push_back(density); 
            current_size++;
        }
        void saveToCSV() const {
            std::string filename = base_filename + ".csv";
            std::ofstream file(filename); 

            if(!file.is_open()){
                throw std::runtime_error("Cannot open file: " + filename); 
            }
            file << "time,density\n";
            
            for(int i = 0; i < current_size; ++i){
                file << time_points[i] << "," << density_values[i] << "\n";
            }
        }

        void plotAndSave(const std::string& title = "") const {
            plt::figure(); 
            plt::plot(time_points, density_values);
            plt::title(title.empty() ? base_filename : title); 
            plt::xlabel("time (ticks)");
            plt::ylabel("density");
            plt::grid(true);
            
            std::string plot_filename = base_filename + "_plot.png";
            plt::save(plot_filename);
            plt::close();
        }

        void clear(){
            time_points.clear();
            density_values.clear();
            current_size = 0;
        }
};

// zadanie 2 
class Experiment {
    private:
        int size = 100;
        double p0; 
        
        DataLogger logger; 
        std::vector<double> density_values;
    public: 
        Experiment(double probability) : p0(probability), life(size, size), logger("density_p0_"+std::to_string(probability)) {
            for(int y = 0; y < size; y++){
                for(int x = 0; x < size; x++){
                    if(rand()/(double)RAND_MAX < p0){
                        life.setCell(x,y,true);
                    }
                }
            }
        }
        Life life;     
        void run(int max_ticks = 1000){
            std::ofstream data("density_p0_" + std::to_string(p0) + ".csv");
            for(int t = 0; t < max_ticks; t++){
                int alive = life.doTick().size();
                double density = alive / (double)(size*size);

                logger.record(t, density); 

                data << t << ", "<<density<<"\n";
                if(t&100 == 0){
                    std::cout << "p0="<<p0<<" t=" << t 
                                        << " density="<<density << std::endl;
                }
            }

            logger.saveToCSV();
            logger.plotAndSave("density evoution for p0=" + std::to_string(p0));
        }

        void analyzeResults(){
            std::vector<double> final_densities; 
            for(int t = 900; t < 1000; t++){
                final_densities.push_back(density_values[t]);
            }
            
            double avg = std::accumulate(final_densities.begin(),
        final_densities.end(), 0.0) / final_densities.size();

        std::cout << "srednia gestosc dla p0=" << p0 << ": " << avg << "\n";
        }
    };


class ErrorAnalysis{
    public:
        static void run(int L, double p0, int N = 100, int Tmax = 1000){
            std::vector<double> final_densities; 

            for(int n = 0; n < N; n++){
                Experiment exp(p0);
                exp.life = Life(L,L); 

                for(int t = 0; t < Tmax; t++){
                    auto cells = exp.life.doTick();
                    if(t==Tmax-1){
                        double density = cells.size() / (double)(L*L);
                        final_densities.push_back(density);
                    }
                }
            }
            double mean = calculateMean(final_densities); 
            double std__err = calculateStdErr(final_densities); 

            std::cout << "L=" << L << " | średnia: " << mean << " | Błąd: " << std__err << "\n";
        }

        private: 
            static double calculateMean(const std::vector<double>& data) {
                return std::accumulate(data.begin(), data.end(), 0.0) / data.size(); 
            }

            static double calculateStdErr(const std::vector<double>& data){
                double mean = calculateMean(data); 
                double sq_sum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0); 
                return std::sqrt(sq_sum / data.size() - mean * mean); 
            }
};


int main(int argc, char* argv[]){
    std::vector<double> probabilities = {0.05, 0.1, 0.3, 0.6, 0.75, 0.8, 0.95};

    for(double p0 : probabilities){
        Experiment exp(p0);
        exp.run(1000);
        exp.analyzeResults(); 
    }

    std::vector<int> sizes = {10,100,200,500,1000};
    for(int L : sizes){
        ErrorAnalysis::run(L, 0.3); 
    }

    App app(100,100); 
    app.start(); 

    return 0; 
}