import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("/home/mateuszsierakowski/Computer-Modeling/Model_Plam/density_data.csv")
plt.plot(data["Density"], data["Iteration"])
plt.xlabel("Iteration")
plt.ylabel("Average B density")
plt.title("Turing Model Density Evolution")
plt.show()

