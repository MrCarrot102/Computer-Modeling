import matplotlib.pyplot as plt
import numpy as np 

# stałe 
board_size = 40 # liczba pól na planszy 
simulations = [100, 1_000_000] # liczba rzutów 

def simulate_monopoly(rolls, jail_rule = False):
    visits = np.zeros(board_size)
    position = 0
    
    for i in range(rolls): 
        roll = np.random.randint(1, 7) + np.random.randint(1,7) # rzut dwiema kostkami 
        position = (position + roll) % board_size # cykliczny ruch po planszy 
        
        if(jail_rule and position == 30):
            position = 10 # przenoszenie na pole nr 10 (więzienie)
            visits[30] += 1 # dodawanie danych że gracz był na polu nr 30 aby trafić do więzienia 
        
        visits[position] += 1 
    return visits / rolls # normalizacja (prawdopodobieństwo obsadzenia pola)

def plot_distribution(standard, jail, rolls): 
    fig, axs = plt.subplots(2, 1, figsize=(10, 10))

    y_max = max(max(standard), max(jail))  # Maksymalna wartość prawdopodobieństwa na obu wykresach

    # wykres 1: bez więzienia
    axs[0].plot(range(board_size), standard, marker='o', linestyle='-', color='blue')
    axs[0].set_title(f"Rozkład pól w Monopoly BEZ więzienia ({rolls} rzutów)")
    axs[0].set_xlabel("Pole planszy")
    axs[0].set_ylabel("Prawdopodobieństwo p(i)")
    axs[0].set_ylim(0, y_max)  # Ustawienie tej samej skali osi Y
    axs[0].grid(True, linestyle="--", alpha=0.7)

    # wykres 2: z więzieniem
    axs[1].plot(range(board_size), jail, marker='s', linestyle='-', color='red')
    axs[1].set_title(f"Rozkład pól w Monopoly Z więzieniem ({rolls} rzutów)")
    axs[1].set_xlabel("Pole planszy")
    axs[1].set_ylabel("Prawdopodobieństwo p(i)")
    axs[1].set_ylim(0, y_max)  # Ustawienie tej samej skali osi Y
    axs[1].grid(True, linestyle="--", alpha=0.7)

    plt.tight_layout()
    plt.savefig(f"monopoly_{rolls}.png")  # Zapis wykresu
    plt.show()

    
for rolls in simulations: 
    standard = simulate_monopoly(rolls, jail_rule=False)
    jail = simulate_monopoly(rolls, jail_rule=True)
    plot_distribution(standard, jail, rolls)

                                

