import matplotlib.pylab as plt
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import numpy as np
import matplotlib as mpl
import ecco_v4_py as ecco
import xarray as xr

#Ce code nous a permis de vérifier si nous avions des données complètes.
#En traçant une carte du monde avec les valeurs du courrant dessus nous avons pu
#voir que nous n'avions pas de données manquantes pour le courrant dans les océans,
#ainsi qu'un aperçus global de ces données.

#lecture d'un des fichier de donnée sur les courrants:
'''
data = np.genfromtxt("/Users/adele/Desktop/informatique/NVELmoyenneconverted_new.csv", delimiter = ",")


long = np.arange(-179.75, 180.25, 0.5)  
lat = np.arange(-89.75, 90.25, 0.5)
lon2d, lat2d = np.meshgrid(long, lat)
plt.figure()
plt.contour(lon2d,lat2d,data)
ax = plt.subplot(111, projection=ccrs.LambertConformal())

#ADD IT IN A MAP OF THE WORLD:
plt.figure()
ax=plt.axes(projection=ccrs.PlateCarree())
ax.set_global()
#ax.coastlines()
ax.contourf(long,lat,data)
plt.show()
'''

def sample_data():
    """
    Returns ``(x, y, u, v, crs)`` of some vector data
    computed mathematically. The returned crs will be a rotated
    pole CRS, meaning that the vectors will be unevenly spaced in
    regular PlateCarree space.

    """
    orth = ccrs.PlateCarree(central_longitude= 180, globe=None)
    Longi = np.arange(-179.75, 180.25, 0.5)  
    Lati = np.arange(-89.75, 90.25, 0.5)
    xy = np.meshgrid(Longi,Lati)        ##//LONGI = X LATI = Y
    longi = xy[0].flatten()
    lati = xy[1].flatten()

    N = 10 * (2 * np.cos(2 * np.deg2rad(longi) + 3 * np.deg2rad(lati + 30)) ** 2)
    E = 20 * np.cos(6 * np.deg2rad(longi))

    return longi, lati, N, E, orth


N = np.genfromtxt("/Users/adele/Desktop/informatique/NVELmoyenneconverted_new.csv", delimiter = ",")
E= np.genfromtxt("/Users/adele/Desktop/informatique/EVELmoyenneconverted_new.csv", delimiter = ",")


orth = ccrs.PlateCarree(central_longitude= 180, globe=None)
ax = plt.axes(projection=orth)

#ax.add_feature(cfeature.OCEAN, zorder=0)    
#ax.add_feature(cfeature.LAND, zorder=0, edgecolor='black')
ax.set_global()
#ax.gridlines()

lati, longi, N, E, vector_crs = sample_data()
ax.quiver(longi, lati, N, E, transform=vector_crs)
plt.show()