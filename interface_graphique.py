import numpy as np 
import matplotlib.pyplot as plt
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import numpy as np
import matplotlib.animation as animation

#Ouverture des données:
GPGP = np.genfromtxt("/Users/adele/Desktop/projet_prog/3ans_final.csv", delimiter=',')
GPGP_highlight=np.copy(GPGP)
GPS_points = np.genfromtxt("/Users/adele/Desktop/test de code/gps_inputs.csv", delimiter=',')

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

'''
#ax.add_feature(cf.OCEAN)
#ax.coastlines()
#ax.gridlines()
#ax.add_feature(cf.BORDERS)
'''

#Ajout des noms des pays et du continent:
ax.text(-152, 42, 'The GPGP', transform=ccrs.PlateCarree(), size=5)
names=[['The GPGP',42, -152],['Australie',-25, 140],['Canada',55,-110],['Chile',-27,-66],['Chine',35,105],['Colombia', 4,-72],['Costa Rica',10,-83],['Ecuador',1,-77],['Salvador',14,-89],['Guatemala',19,-95],['Honduras',16	,-86],['Hong Kong',	23.5,	114],['Indonésie',	-1.5,	120],['Japon',33,138],['North Korea',45,124],['South korea',37,	128],['Malaisie',7,95],['Mexique',	25,	-100],['Nouvelle calédonie',-19,	165],['nicaragua'	,13,	-84],['Panama',8,-76],['Pérou'	,-10,-75],['Philiphines',13,124],['Russia',51,135],['Singapour',1,95],['USA',40,-100],['Vietnam',14,100]]

for i in range (0,len(names)):
    ax.text(names[i][2], names[i][1], names[i][0], transform=ccrs.PlateCarree(), size=5)

#Point d'inputs des déchets dans l'océan:
for i in range(1,GPS_points.shape[0]):
    plt.scatter(x=GPS_points[i,2], y=GPS_points[i,3],color="black",s=1, alpha=0.5, transform= ccrs.PlateCarree())
    
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

#Interval de temps pour la simmulation:
time=np.arange(0,years,1)

def init():
    pass

plot=0
date=0
simu=0
stats=0
cbar=0
infof=0

# Fonction pour l'annimation:
def animation_GPGP(years):
    global plot,date,simu,stats,cbar, infof
   
   #Remmettre tout à 0 à chaque fois:
    if years != 0:
        date.remove()
        simu.remove()
        cbar.remove()
        plot.remove()
       
    # Selectionner les données différentes de 0 afin de plot seulement ces dernières:  
    sel = np.where(GPGP_highlight[years,:] > 0)
        
    #paramètres du graph:
    plot = plt.scatter(Longi[sel], Lati[sel], c=GPGP_highlight[years,sel], s=1, cmap=plt.cm.jet,transform=ccrs.PlateCarree())
    date = ax.annotate(f"Année:{years}\n", xy=(7, 7), xycoords='figure points', zorder=12)
    plt.title(label=f" Simulation du GPGP : The Great Pacific Garbage Patch", size= 'x-small', verticalalignment='bottom')
    plt.clim(vmin=0, vmax=200)

    #paramètres de la légende:
    cbar = plt.colorbar(orientation='horizontal', shrink=0.75)
    cbar.set_label('\nGradient de saturation [nb de paquets de 10 000 plastiques par case de 0.5° x 0.5°]',  size='small')
    cbar.ax.tick_params(labelsize=7) 
    

    simu = ax.annotate(f"Durée de la simulation: {years} ans", xy=(7, 30), xycoords='figure points', zorder=12, size='x-small')

    
animGPGP = animation.FuncAnimation(fig,animation_GPGP,init_func=init, frames= time, interval=5000, repeat =False)  
plt.show()


#sauvegarde de l'annimation sous forme de gif:
'''
f = r"/Users/adele/Desktop/animGPGP.gif" 
writergif = animation.PillowWriter(fps=30) 
animGPGP.save(f, writer=writergif)
'''
