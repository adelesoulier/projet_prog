import numpy as np 
import matplotlib.pyplot as plt
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import numpy as np
import matplotlib as mpl
import matplotlib.animation as animation
from matplotlib.colors import LogNorm
import matplotlib.cbook as cbook

#Création du fond de carte:

long = np.arange(-179.75, 180.25, 0.5)  
lat = np.arange(-89.75, 90.25, 0.5)

GPGP = np.genfromtxt("actualisationGPGP.csv", delimiter=',')

#Nombre d'années durant laquelle on fait la simulation:
years= GPGP.shape[0]
nb_values=GPGP.shape[1]
print(years,nb_values)

##??? inutile
max=np.max(GPGP)
print(max)

longi = np.arange(-179.75, 180.25, 0.5)  
lati = np.arange(-89.75, 90.25, 0.5)
xy = np.meshgrid(longi,lati)
Longi = xy[0].flatten()
Lati = xy[1].flatten()



#Création du fond de carte
orth = ccrs.Orthographic(central_longitude= 180, central_latitude=23, globe=None)
ax = plt.axes(projection=orth)
ax.stock_img()
ax.coastlines()
ax.gridlines()


#p = plt.scatter(Long[i:i+100], Lat[i:i+100], c=Tkns[i:i+100], s=1, cmap=plt.cm.jet,transform=ccrs.PlateCarree())

#ax = plt.axes(projection=ccrs.Orthographic(central_longitude= 0, central_latitude=90, globe=None))
#ax.text()



for i in range(years):
    p = plt.scatter(Long, Lat, c=GPGP[i,:], s=1, cmap=plt.cm.jet,transform=ccrs.PlateCarree())
    date = ax.annotate(f"Year:{i}\n", xy=(7, 7), xycoords='figure points', zorder=12)
    plt.title(label=f"Year:{i}\n", size= 'x-small', verticalalignment='bottom')
    plt.clim(vmin=0, vmax=2)
    cbar = plt.colorbar()
    cbar.set_label('Gradient de saturation [nb de paquets de 10 000 plastiques')
    
    #mettre sur pause le graphique pendant 0.001 scd avant de l'enlever pour en mettre un nouveau sur le fond de carte
    plt.pause(0.0001)
    plt.pyplot.savefig(f'GPG_{i}.png')
    date.remove()
    cbar.remove()
    p.remove()

plt.show()
