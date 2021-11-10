import sys

"""from numpy.lib.npyio import genfromtxt"""
from xarray.core.coordinates import DatasetCoordinates 
sys.path.append('/home/maelle/ECCOv4-py')
import ecco_v4_py as ecco
import numpy as np
import xarray as xr
import matplotlib as plt

ds = xr.open_dataset("/home/maelle/ECCOv4-py/myspace/nvel/NVEL_201501.nc").load()
data=ds.NVEL[0,0,:, :].values
'''dataT=ds.NVEL[0,0,:, :].values'''
dataT2 = np.transpose(data)

dac = np.genfromtxt('nvel/nvel_01.csv', delimiter = ",")

print('data:')
print(data[100,638])
print('dataT2:')
print(dataT2[638,100])
print('csv:')
print(dac[638,100])