#importation des modules:
import sys 
sys.path.append('/home/maelle/ECCOv4-py')
import ecco_v4_py as ecco
import numpy as np
import xarray as xr
import matplotlib as plt

#indexation du tableau 4D: (time, depth, latitude, longitude) , type de variables: float32
'''
Coordinates:
  * latitude        (latitude) float64 -89.75 -89.25 -88.75 ... 89.25 89.75   #pas de O.5° entre les valeurs
  * depth           (depth) float32 5.0 15.0 25.0 ... 5.461e+03 5.906e+03     #pas de 10m entre les valeurs
  * longitude       (longitude) float64 -179.75 -179.25 -178.75 ... 179.25 179.75  #pas de O.5° entre les valeurs
  * time            (time) datetime64[ns] (mois + année d'aquisition des données) 
  => ces coordonnées permettent d'indexer les valeurs du tableau de donnée, chaque case correspond
  à une lat,long,prof et temp donné
  => cependant les indexs des tableaux sont des entiers donc pour avoir un point à (lat x; long y)
  indice lat: (x + 89.75)/0.5
  indice longitude: (x + 179.8)/0.5 
  indice profondeur: 0
  indice temps: 0
  ''' 

##################################
#NVEL 01
##################################

#ouverture du fichier .nc:

#ds=nom variable (ds pour data set)'''
#indiquer le chemin jusqu'au fichier en argument
ds = xr.open_dataset("/home/maelle/ECCOv4-py/myspace/nvel/NVEL_201501.nc").load()

#GÉNÉRER LE TABLEAU 2D (LATTITUDE, LONGITUDE) DES DONNÉES À LA SURFACE:
data=ds.NVEL[0,0,:, :].values
'''print(data)
print(data.shape)'''

#On transpose la matrice. Après cette ligne, on aura latitudes = colonnes, longitudes = lignes
dataT = np.transpose(data)
'''print(dataT[120,100])'''

#On crée le fichier csv voulu (long, lat)
with open('nvel/nvel_01.csv', 'w') as f:
  for long in range(dataT.shape[0]):
    for lat in range(dataT.shape[1]):
      if lat > 0:
        f.write(',')
      f.write(f'{dataT[long,lat]}')
    f.write('\n')
    #apparemment on peut faire la même chose avec np.savetxt('nvel/nvel_01.csv', dataT, delimiter=',')...

#faire un test: est-ce que le fichier csv est juste?
"""
dac = np.genfromtxt('nvel/nvel_01.csv', delimiter = ",")

print('data:')
print(data[100,638])
print('dataT:')
print(dataT[638,100])
print('csv:')
print(dac[638,100])
"""
