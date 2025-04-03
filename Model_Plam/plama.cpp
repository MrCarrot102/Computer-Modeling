#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include "matplotlibcpp.h" // Dodajemy bibliotekę matplotlibcpp

namespace plt = matplotlibcpp;

class Plama
{
public:
    int width, height;
    std::vector<std::vector<bool>> tab;
    const std::vector<std::pair<int, int>> neighborOffsets = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0},
                                                              {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    sf::RenderWindow window;

    Plama(int w = 100, int h = 100)
        : width(w), height(h), window(sf::VideoMode(w * 10, h * 10), "Plama Simulation")
    {
        tab.resize(height, std::vector<bool>(width, false));
        randomizeTab(tab);
    }

    void randomizeTab(std::vector<std::vector<bool>> &tab)
    {
        for (auto &row : tab)
        {
            for (int i = 0; i < row.size(); i++)
            {
                row[i] = rand() % 2;
            }
        }
    }

    int countNeighbourNum(const std::vector<std::vector<bool>> &tab, int x, int y)
    {
        int count = 0;
        int numRow = tab.size();
        int numCol = tab[0].size();

        for (const auto &offset : neighborOffsets)
        {
            int newX = (x + offset.first + numRow) % numRow;
            int newY = (y + offset.second + numCol) % numCol;
            if (tab[newX][newY])
                count++;
        }
        return count;
    }

    void checkLife(std::vector<std::vector<bool>> &tab)
    {
        std::vector<std::vector<bool>> tmp = tab;
        for (int row = 0; row < tab.size(); row++)
        {
            for (int col = 0; col < tab[row].size(); col++)
            {
                int neighbourCount = countNeighbourNum(tab, row, col);
                if (neighbourCount >= 6 && neighbourCount <= 9 || neighbourCount == 4)
                    tmp[row][col] = true;
                else if (neighbourCount >= 0 && neighbourCount <= 3 || neighbourCount == 5)
                    tmp[row][col] = false;
            }
        }
        tab = tmp;
    }

    void loop(int iterations)
    {
        std::vector<int> x;
        std::vector<int> y;

        while (window.isOpen() && iterations > 0)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            drawFrame();
            window.display();

            checkLife(tab);

            // Zapisujemy dane o liczbie żywych komórek
            int liveCells = 0;
            for (const auto &row : tab)
            {
                liveCells += std::count(row.begin(), row.end(), true);
            }
            x.push_back(iterations);
            y.push_back(liveCells);

            iterations--;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // Po zakończeniu symulacji, rysujemy wykres
        plotGraph(x, y);
    }

    void drawFrame()
    {
        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                sf::RectangleShape cell(sf::Vector2f(10.f, 10.f));
                cell.setPosition(col * 10.f, row * 10.f);

                if (tab[row][col])
                    cell.setFillColor(sf::Color::White); // Alive
                else
                    cell.setFillColor(sf::Color::Black); // Dead

                window.draw(cell);
            }
        }
    }

    // Funkcja rysująca wykres
    void plotGraph(const std::vector<int> &x, const std::vector<int> &y)
    {
        plt::plot(x, y);
        plt::title("Wykres gestosci zywych komorek w czasie");
        plt::xlabel("Czas [Iteracja]");
        plt::ylabel("Liczba zywych komorek");
        plt::grid(true);
        plt::save("wykres.png");
    }
};

int main()
{
    Plama plama(100, 100); // 100x100 grid
    plama.loop(100000); // Run 1000 iterations

    return 0;
}
