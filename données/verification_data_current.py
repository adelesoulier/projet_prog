import matplotlib.pyplot as plt
from matplotlib.colors import BoundaryNorm
import numpy as np

#Ce code nous a permis de vérifié si nous avions des données complètes.
#En traçant une carte du monde avec les valeurs du courrant nous avons pu
#voir que nous n'avions pas de données manquantes pour le courrant dans les océans.

#lecture d'un des fichier de donnée sur les courrants:
data = np.genfromtxt("/Users/adele/Desktop/informatique/NVELmoyenneconverted_new.csv", delimiter = ",")
lat, longi = data.shape
empty_map= np.zeros((lat,longi))
'''
#dessins des continents:
for i in range(0,longi):
    for j in range(0,lat):
        if np.isnan(data[i,j]):
            empty_map[i,j]+=1
            print('ccc') '''
        
empty_map= np.isnan(data)
        
longi = np.arange(-179.75, 180.25, 0.5)  
lat = np.arange(-89.75, 90.25, 0.5)

fig, ax = plt.subplots()
ax.pcolormesh(longi,lat, empty_map)

plt.show()