# 🌊 Stain Simulation (Model_Plam)

This project simulates the **spreading of a stain** (or a diffusion-like process) on a surface using **SFML** for visualization and **Matplotlib** for analysis. The model demonstrates how a stain evolves over time based on defined parameters.

## 🎥 Simulation Previ

![Simulation Preview](https://github.com/user-attachments/assets/0e8c8621-1910-45ee-9314-d39492ccc64d)

## ⚙️ Dependencies
- **SFML** – Graphics rendering  
- **Python3** – For data visualization  
- **NumPy** – Numerical computations  

## 🔧 Installation
Ensure all dependencies are installed, then compile using:
```sh
g++ plama.cpp -o plama -I/usr/include/python3.13 -I/path/to/sfml/include -L/path/to/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system -lpython3.13
