#
# Example 7
#
# 100 root system
# Chemotropism ,
# proof of concept, with a static nutrient concentration
#
import py_rootbox as rb
import math
import random
import numpy as np
import matplotlib.pyplot as plt

name = "maize"

#
# Plant and root parameter from a file
#
allRS = [ ]

#
# Creates N root systems
#
N=10
for i in range(0,N-1):
	rootsystem = rb.RootSystem();
	rootsystem.openFile(name);
	allRS.append(rootsystem);


# "manually" set tropism to hydrotropism for the first ten root types
for i in range(0,10):
    rootsystem.getRootTypeParameter(i+1).tropismT = rb.TropismType.chemo;
    rootsystem.getRootTypeParameter(i+1).tropismN = 2; # N
    rootsystem.getRootTypeParameter(i+1).tropismS = 0.5; # sigma

# check if it worked
# for i in range(0,10):
#     print(rootsystem.getRootTypeParameter(i+1))

print (rootsystem.getRootSystemParameter().seedPos)

#
# Static soil property
#
sideBox = rb.SDF_PlantBox(30,30,50)
#rootsystem.setGeometry(sideBox)  # for vizualisation

#nutrientBox = rb.SDF_PlantBox(30,30,5)
#nutrientBox2 = rb.SDF_RotateTranslate(nutrientBox, rb.Vector3d(0,0,-20))
nutrientBox = rb.SDF_PlantBox(10,10,10)
nutrientBox2 = rb.SDF_RotateTranslate(nutrientBox, rb.Vector3d(-5,0,-10))


maxS = 0.7 # maximal saturation
minS = 0.1 # minimal saturation
slope = 20 # [cm] linear gradient between min and ma
soilprop = rb.SoilPropertySDF(nutrientBox2, maxS, minS, slope)

# set the soil properties before calling initialize
#rootsystem.setSoil(soilprop);

#
# Initialize
#
#rootsystem.initialize(4,5); # it is important to call initialize() after setGeometry()

#
# Simulate
#
simtime = 60; # e.g. 30 or 60 days
for rs in allRS:
	rs.setGeometry(sideBox)
	for i in range(0,10):
	    rs.getRootTypeParameter(i+1).tropismT = rb.TropismType.chemo;
	    rs.getRootTypeParameter(i+1).tropismN = 2; # N
	    rs.getRootTypeParameter(i+1).tropismS = 0.5; # sigma
	print (rs.getRootSystemParameter().seedPos)
	rs.setSoil(soilprop)
	rs.setSeed(random.random())
	rs.initialize()
	rs.simulate(simtime)
	s=rs.getSegments()
	print(s)
#
# Export results as single vtp files
#
c = 0
for rs in allRS:
      c += 1 # root system number
      vtpname = "results/"+name+str(c)+".vtp";
      rs.write(vtpname, rb.OutputType.polylines);
#
# Export container geometry as Paraview Python script (run file in Paraview by Tools->Python Shell, Run Script)
#
rs.write(name + "_example7.py",0);

#
# Compute vertical RLD distribution in layers
#
nl = 50; # number of layers
vRLD=np.zeros((N,nl)); # N rows, nl columns
depth=100;
c=0
for rs in allRS:
      c += 1 # root system number
      analysis = rb.AnalysisSDF(rs)
      RLD = analysis.distribution(rb.ScalarType.length,0,depth,nl,1)
      vRLD[c,:]=RLD

z=np.linspace(0,depth*(-1),nl)   # depth*-1 is the (negativ) z coordinate
mean=np.mean(vRLD,axis=0)
std=np.std(vRLD,axis=0)
plt.figure(figsize=(3.8,3))
plt.plot(mean,z,'k-',linewidth=2)
plt.fill_betweenx(z,mean+std,mean-std,color='#b9cfe7',edgecolor='')
plt.show()

#print("Finished with a total of " + str(rootsystem.getNumberOfNodes()) + " nodes\n")