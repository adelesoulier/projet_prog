import numpy as np 
import matplotlib.pyplot as plt
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import numpy as np
import matplotlib as mpl
import matplotlib.animation as animation
from matplotlib.colors import LogNorm
import matplotlib.cbook as cbook


#Ouverture des données:
GPGP = np.genfromtxt("/Users/adele/Desktop/actualisationGPGP.csv", delimiter=',')
GPGP_highlight=np.copy(GPGP)
GPS_points = np.genfromtxt("/Users/adele/Desktop/gps_inputs.csv", delimiter=',')

#Paramètres de la simulation:
years= GPGP.shape[0]
nb_values=GPGP.shape[1]
saturation=np.max(GPGP)
longi = np.arange(-179.75, 180.25, 0.5)  
lati = np.arange(-89.75, 90.25, 0.5)
xy = np.meshgrid(longi,lati)
Longi = xy[0].flatten()
Lati = xy[1].flatten()


#Création du fond de carte
fig = plt.figure()
orth = ccrs.PlateCarree(central_longitude= 180, globe=None)
ax = plt.axes(projection=orth)
ax.stock_img()

for i in range(1,GPS_points.shape[0]):
    plt.scatter(x=GPS_points[i,2], y=GPS_points[i,3],color="black",s=1, alpha=0.5, transform= ccrs.PlateCarree())
   
'''
ax.add_feature(cf.OCEAN)
ax.coastlines()
ax.gridlines()
ax.add_feature(cf.BORDERS)
'''



#Mise en évidence des points pour l'affichage:
for year in range (0,years):
    for paquet in range(0,nb_values) : #indice i du tableau 1D
        if GPGP[year,paquet]!=0:
            for i in range(0,3):
                for j in range(0,3):
                    x=paquet%720
                    y=paquet//720
                    xvoisin=x-1+i
                    yvoisin=y-1+j
                    #vérification que nous ne sommes pas dans des lat et long qui n'existent pas:
                    if (yvoisin<0 or yvoisin>=360 or xvoisin<0 or xvoisin>=720):
                        break
                    #vérification que nous ne sommes pas dans le GPG:
                    if ( yvoisin>230 and yvoisin<=260 and xvoisin>=40 and xvoisin<100 ):
                        break
    
                    if (GPGP[year,yvoisin*720+xvoisin]==0):
                        GPGP_highlight[year,yvoisin*720+xvoisin]=GPGP[year,paquet]

#ANNIMATION:
time=np.arange(0,years,1)
def init():
    pass

plot=0
date=0
simu=0
stats=0
cbar=0
infof=0

def animation_GPGP(years):
#for i in range(months):
    global plot,date,simu,stats,cbar, infof
   
   #Remmettre tout à 0 à chaque fois:
    if years != 0:
        date.remove()
        simu.remove()
        #stats.remove()
        cbar.remove()
        plot.remove()
       
    # Selectionner les données différentes de 0:  
    sel = np.where(GPGP_highlight[years,:] > 0)
        

    plot = plt.scatter(Longi[sel], Lati[sel], c=GPGP_highlight[years,sel], s=1, cmap=plt.cm.jet,transform=ccrs.PlateCarree())
    date = ax.annotate(f"Année:{years}\n", xy=(7, 7), xycoords='figure points', zorder=12)
    plt.title(label=f" Simulation du GPGP : The Great Pacific Garbage Patch", size= 'x-small', verticalalignment='bottom')
    plt.clim(vmin=0, vmax=2000)

    #paramètres de la légende:
    cbar = plt.colorbar(orientation='horizontal', shrink=0.75)
    cbar.set_label('\nGradient de saturation [nb de paquets de 10 000 plastiques par case de 0.5° x 0.5°]',  size='small')
    cbar.ax.tick_params(labelsize=7) 
    

    simu = ax.annotate(f"Durée de la simulation: {years} ans", xy=(7, 30), xycoords='figure points', zorder=12, size='x-small')

    
animGPGP = animation.FuncAnimation(fig,animation_GPGP,init_func=init, frames= time, interval=5000, repeat =False)  
#plt.savefig('simulation_GPGP.mp4')
plt.show()

