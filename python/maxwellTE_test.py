#!/usr/bin/python3.10
from fdtd_plasma import *

from OpenGL.GL import * #simpledraw2D
import simpledraw2D as sd2D #simpledraw2D
sd2D.init(13,13,1) #simpledraw2D

fdtd = fdtdTE_plasma(nz=512, nx=512, dz=.001, dx=.001, nplasma=3000., V=math.sqrt(0.0), nu=0.0)

print("dt="+str(fdtd.dt))

ns=133

source_xi = int(fdtd.nx*0.3)
Z = fdtd.nz * fdtd.dz * 0.1

sig1=np.zeros(fdtd.nz, dtype=np.float32)
sig2=np.zeros(fdtd.nz, dtype=np.float32)

for i in range(0,ns):
  for j in range(0,8):
    
    for zi in range(0, fdtd.nz) :
      z = (fdtd.shft + zi) * fdtd.dz
      z_loc = z - fdtd.V * fdtd.t - fdtd.nz * fdtd.dz * 0.7
      sig1[zi] = (math.cos(z_loc*math.pi/Z) * 0.5 + 0.5) * math.sin(z_loc * math.pi/Z) if abs(z_loc) < Z else 0
      fdtd.bz[zi, source_xi] += (math.cos(z_loc*math.pi/Z) * 0.5 + 0.5) * math.sin(z_loc * math.pi/Z) * fdtd.dt * 0.5 if abs(z_loc) < Z else 0
    fdtd.step(dt = fdtd.dt * 0.5)
    for zi in range(0,fdtd.nz) :
      sig2[zi] = (fdtd.ey[zi,source_xi] - fdtd.ey[zi,source_xi-1]) / fdtd.dx

  sd2D.draw(fdtd.ey, 0); #simpledraw2D
  
  if i==0 :
	  sd2D.halt() #simpledraw2D
  
  print(str(i)+"/"+str(ns))

sd2D.finish() #simpledraw2D
print("all done")
