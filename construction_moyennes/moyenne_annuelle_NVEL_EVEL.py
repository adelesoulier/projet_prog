'''Fichier python ayant permis de réaliser une moyenne annuelle
des courrants marins à la surface dans le Pacifique. Nous avons
pris les données de l'année la plus récente et les 12 mois disponibles'''

#importation des modules:
import ecco_v4_py as ecco
import numpy as np
import xarray as xr

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
#EVEL: Estward Component of Velocity (m/s)
##################################

#ouverture des fichier .nc:

ds1 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201501.nc.part").load()
ds2 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201502.nc.part").load()
ds3 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201503.nc.part").load()
ds4 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201504.nc.part").load()
ds5 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201505.nc.part").load()
ds6 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201506.nc.part").load()
ds7 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201507.nc.part").load()
ds8 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201508.nc.part").load()
ds9 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201509.nc.part").load()
ds10 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201510.nc.part").load()
ds11 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201511.nc").load()
ds12 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/EVEL_201512.nc").load()


#GÉNÉRER LE TABLEAU 2D (LATTITUDE, LONGITUDE) DES DONNÉES À LA SURFACE:
evel1=np.transpose(ds1.EVEL[0,0,:, :].values)
evel2=np.transpose(ds2.EVEL[0,0,:, :].values)
evel3=np.transpose(ds3.EVEL[0,0,:, :].values)
evel4=np.transpose(ds4.EVEL[0,0,:, :].values)
evel5=np.transpose(ds5.EVEL[0,0,:, :].values)
evel6=np.transpose(ds6.EVEL[0,0,:, :].values)
evel7=np.transpose(ds7.EVEL[0,0,:, :].values)
evel8=np.transpose(ds8.EVEL[0,0,:, :].values)
evel9=np.transpose(ds9.EVEL[0,0,:, :].values)
evel10=np.transpose(ds10.EVEL[0,0,:, :].values)
evel11=np.transpose(ds11.EVEL[0,0,:, :].values)
evel12=np.transpose(ds12.EVEL[0,0,:, :].values)
#On transpose les arrays pour avoir latitudes = colonnes, longitudes = lignes


#moyenne:
evelmoy=np.zeros((evel1.shape[0],evel1.shape[1]))
evels=[evel1, evel2, evel3, evel4, evel5, evel6, evel7, evel8, evel9, evel10, evel11, evel12]
for long in range(evel1.shape[0]):
    for lat in range(evel2.shape[1]):
      for i in range(0,len(evels)):
        if evels[i][long,lat]=='nan':
           evelmoy[long,lat]='nan'
        else:
          evelmoy[long,lat]+=evels[i][long,lat]
      evelmoy[long,lat]=evelmoy[long,lat]/12

#CONVERTION DES M/S  EN ° / HEURE:
'''Longueur en mètres de 1 ° de latitude = toujours 111,32 km = 111,32*10^3m'''
'''EVLE: composante lattitudinale de la vitesse'''
evelmoyconverted=np.copy(evelmoy)

for long in range(evelmoy.shape[0]):
    for lat in range(evelmoy.shape[1]):
      evelmoyconverted[long,lat]=evelmoy[long,lat]*3600/(111.32*10**3)

#On crée le fichier csv voulu (long, lat)
np.savetxt("EVELmoyenneconverted.csv", evelmoyconverted, delimiter="," )

##################################
#NVEL: Northward Component of Velocity (m/s)
##################################

#ouverture des fichier .nc:

ds1 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201501.nc.part").load()
ds2 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201502.nc.part").load()
ds3 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201503.nc.part").load()
ds4 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201504.nc.part").load()
ds5 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201505.nc.part").load()
ds6 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201506.nc.part").load()
ds7 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201507.nc.part").load()
ds8 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201508.nc").load()
ds9 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201509.nc.part").load()
ds10 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201510.nc.part").load()
ds11 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201511.nc.part").load()
ds12 = xr.open_dataset("/Users/adele/Desktop/informatique/projet_data/construction_moyennes/NVEL_201512.nc.part").load()

#GÉNÉRER LE TABLEAU 2D (LATTITUDE, LONGITUDE) DES DONNÉES À LA SURFACE:
nvel1=np.transpose(ds1.NVEL[0,0,:, :].values)
nvel2=np.transpose(ds2.NVEL[0,0,:, :].values)
nvel3=np.transpose(ds3.NVEL[0,0,:, :].values)
nvel4=np.transpose(ds4.NVEL[0,0,:, :].values)
nvel5=np.transpose(ds5.NVEL[0,0,:, :].values)
nvel6=np.transpose(ds6.NVEL[0,0,:, :].values)
nvel7=np.transpose(ds7.NVEL[0,0,:, :].values)
nvel8=np.transpose(ds8.NVEL[0,0,:, :].values)
nvel9=np.transpose(ds9.NVEL[0,0,:, :].values)
nvel10=np.transpose(ds10.NVEL[0,0,:, :].values)
nvel11=np.transpose(ds11.NVEL[0,0,:, :].values)
nvel12=np.transpose(ds12.NVEL[0,0,:, :].values)
#On transpose les arrays pour avoir latitudes = colonnes, longitudes = lignes

#moyenne:
nvelmoy=np.zeros((nvel1.shape[0],nvel2.shape[1]))
nvels=[nvel1, nvel2, nvel3, nvel4, nvel5, nvel6, nvel7, nvel8, nvel9, nvel10, nvel11, nvel12]
for long in range(nvel1.shape[0]):
    for lat in range(nvel2.shape[1]):
      for i in range(0,len(nvels)):
        if nvels[i][long,lat]=='nan':
          nvelmoy[long,lat]='nan'
        
        else:
          nvelmoy[long,lat]+=nvels[i][long,lat]
      
      nvelmoy[long,lat]=nvelmoy[long,lat]/12


#CONVERTION M/S  EN ° / HEURES:'''
'''Longueur en mètres de 1 ° de longitude = 40075 km * cos (latitude [rad]) / 360
NVEL: composante longitudinale de la vitesse'''

nvelmoyconverted=np.copy(nvelmoy)
for long in range(nvelmoy.shape[0]):
    for lat in range(nvelmoy.shape[1]):
      nvelmoyconverted[long,lat]=nvelmoy[long,lat]*3600/(40075* 10**3 * np.cos(lat*np.pi/180)/360)


#On crée le fichier csv voulu (long, lat)
np.savetxt("NVELmoyenneconverted.csv", evelmoyconverted, delimiter="," )


