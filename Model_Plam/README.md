# 🌊 Stain Simulation (Model_Plam)

This project simulates the **spreading of a stain** (or a diffusion-like process) on a surface using **SFML** for visualization and **Matplotlib** for analysis. The model demonstrates how a stain evolves over time based on defined parameters.

## 🎥 Simulation Preview

https://github.com/user-attachments/assets/21c9d54a-51e7-4069-b142-4a06a71a7f16

## ⚙️ Dependencies
- **SFML** – Graphics rendering  
- **Python3** – For data visualization  
- **NumPy** – Numerical computations  

## 🔧 Installation
Ensure all dependencies are installed, then compile using:
```sh
g++ plama.cpp -o plama -I/usr/include/python3.13 -I/path/to/sfml/include -L/path/to/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system -lpython3.13
