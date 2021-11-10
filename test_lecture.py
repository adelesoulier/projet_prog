import ecco_v4_py as ecco
import numpy as np
import xarray as xr
import matplotlib as plt

#ouverture du fichier .nc ds=nom variable / indiquer le chemin jusqu'au fichier et le .load jsp?
ds = xr.open_dataset("/Users/adele/Desktop/informatique/NVEL_201511.nc").load()

#le print ds.NVEL affiche le tableau avec les composantes N en m/s du courrant

#print(ds)

#print('Voici les valeurs pour NVEL: \n')
#print(ds.NVEL)

print(ds.NVEL.values)