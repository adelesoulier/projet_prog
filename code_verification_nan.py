from typing import ClassVar
import matplotlib.pylab as plt
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import numpy as np
import matplotlib as mpl
import ecco_v4_py as ecco

def exacto025lat(lat):
    lat = (lat + 89.75)*2
    lat = round(lat, 0)
    return int(lat)
def exacto025long(longi):
    longi = (longi + 179.75)*2
    longi = round(longi, 0)
    return int (longi)

NVEL=np.loadtxt("/Users/adele/Desktop/NVELmoyenneconverted_new.csv", delimiter=',')
EVEL = np.loadtxt("/Users/adele/Desktop/RENDU FINALS PROJET PAS TOCHER/CSV DATA/EVELmoyenneconverted_new.csv", delimiter=',')
#gps_inputs= np.loadtxt('/Users/adele/Desktop/coordonée GPS modifiées.csv', delimiter=',' )

print (exacto025long(151.001385),exacto025lat(-34.0731))
long,lat=exacto025long(151.001385),exacto025lat(-34.0731)
print(EVEL[lat,long])
#TEST POUR VOIR OÙ SONT LES NAINS

#LONG PUIS LAT

'''longi = np.arange(-179.75, 180.25, 0.5)  
lati = np.arange(-89.75, 90.25, 0.5)'''

'''for i in range (0,48):
    lat=gps_inputs[i,1]
    long=gps_inputs[i,0]
    exact_long=exacto025long(long)
    exact_lat=exacto025lat(lat)
    print(f'courant au point gps {i} : {ds[exact_lat,exact_long]}') 
    
    if np.isnan(ds[exact_lat,exact_long]):
        
     print(f'nan au point GPS {i}')
        '''


