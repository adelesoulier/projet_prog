
#CODE DE BASE POUR LA LECTURE DES FICHIERS .nc
#---------------------------------------------#
'''NVEL = Northward Component of Velocity (m/s)'''

#importation des modules:
import ecco_v4_py as ecco
import numpy as np
import xarray as xr
import matplotlib as plt

#ouverture du fichier .nc:

#ds=nom variable (ds pour data set)'''
#indiquer le chemin jusqu'au fichier en argument
ds = xr.open_dataset("/Users/adele/Desktop/informatique/NVEL_201511.nc").load()

#Afficher l'objet entier:
'''print('Voici le datasat : \n\n')
print(ds)'''

#Afficher le tableau 4D des données d'intérêt (NVEL OU WVEL)
'''print('Voici les valeurs pour NVEL: \n')
print(ds.NVEL)'''

#idexation du tableau 4D: (time, depth, latitude, longitude) , type de variables: float32
'''
Coordinates:
  * latitude        (latitude) float64 -89.75 -89.25 -88.75 ... 89.25 89.75   #pas de O.5° entre les valeurs
  * depth           (depth) float32 5.0 15.0 25.0 ... 5.461e+03 5.906e+03     #pas de 10m entre les valeurs
  * longitude       (longitude) float64 -179.8 -179.2 -178.8 ... 179.2 179.8  #pas de O.5° entre les valeurs
  * time            (time) datetime64[ns] (mois + année d'aquisition des données) 
  => ces coordonnées permettent d'indexer les valeurs du tableau de donnée, chaque case correspond
  à une lat,long,prof et temp donné
  => cependant les indexs des tableaux sont des entiers donc pour avoir un point à (lat x; long y)
  indice latitude: (x + 89.75)/0.5
  indice longitude: (x + 179.75)/0.5 
  indice profondeur: 0
  indice temps: 0
  ''' 

#Afficher le tableau 4D sans les infos sur les données (juste les valeurs numériques)
'''print(ds.NVEL.values)'''

#Afficher une case du tableau 4D sans les infos sur les données (juste les valeurs numériques)
'''print(ds.NVEL[0,0, x, y].values)'''

#Afficher le tableaux des longitudes ou lattitudes:
'''print(ds.NVEL.longitude.values)'''
'''print(ds.NVEL.latitude.values)'''


#GÉNÉRER LE TABLEAU 2D (LATTITUDE, LONGITUDE) DES DONNÉES À LA SURFACE:
data=ds.NVEL[0,0,:, :].values
print(data)
