#!/usr/bin/python3.9

import numpy as np
import math
import copy
import os
import time
from psutil import Process
import ctypes

def shift(arr, shft) :
  for i in range(0, len(arr) - shft) :
    arr[i] = arr[i+shft].copy()
  for i in range(len(arr) - shft, len(arr)) :
    arr[i] = np.zeros(len(arr[0]), dtype=np.float32)



class fdtd_plasma :
  def __init__(self, nz, nx, dz, dx, nplasma, nu, V, xPML=None, shft0=None):
    self.nz=nz
    self.nx=nx
    self.dz=dz
    self.dx=dx
    self.ez = np.zeros((nz, nx), dtype=np.float32)
    self.ex = np.zeros((nz, nx), dtype=np.float32)
    self.jz = np.zeros((nz, nx), dtype=np.float32)
    self.jx = np.zeros((nz, nx), dtype=np.float32)
    self.by = np.zeros((nz, nx), dtype=np.float32)
    self.byz = np.zeros((nz, nx), dtype=np.float32)
    self.byx = np.zeros((nz, nx), dtype=np.float32)
    self.plasma = np.zeros((nz, nx), dtype=np.float32)
    self.regular = np.zeros(nx, dtype=np.float32)
    self.PML = np.zeros(nx, dtype=np.float32)
    self.PMLexp = np.zeros(nx, dtype=np.float32)
    self.np = np
    self.dt = math.sqrt(1 / (1/dx**2 + 1 / dz**2))
    self.V = V
    self.nu = nu
    if shft0 is None:
      self.shft = 0
      self.shft0 = 0
    else:
      self.shft0 = int(shft0/dz)
      self.shft = self.shft0
    self.BS = 16
    self.t = 0
    if xPML is None:
      self.xPML = self.nx * self.dx * 0.1
    else:
      self.xPML = xPML

    self.PMLi0 = math.ceil(self.xPML/self.dx)
    self.PMLi1 = self.nx #math.floor((self.nx*self.dx - self.xPML)/self.dx)
    
    for i in range(0, self.PMLi0) :
      self.regular[i] = 0
      self.PML[i] = 1
    for i in range(self.PMLi0, self.PMLi1) :
      self.regular[i] = 1
      self.PML[i] = 0
    for i in range(self.PMLi1, self.nx) :
      self.regular[i] = 0
      self.PML[i] = 1
    
    for i in range(0, self.nx) :
      x = i * self.dx
      
      PMLstrength = 0
      
      if x < self.xPML :
        PMLstrength = (self.xPML-x) / self.xPML
      
      #if x > self.nx * self.dx - self.xPML :
      #  PMLstrength = (x - (self.nx * self.dx - self.xPML)) / self.xPML
      
      self.PMLexp[i] = math.exp( - 50 * PMLstrength**2 * self.dt)
    '''
    for j in range(0, self.nx) :
      for i in range(0, self.nz) :
        self.plasma[i, j] = nplasma if j > self.nx/2 else 0
    '''
    for j in range(0, self.nx) :
      self.plasma[0, j] = nplasma if j > self.nx/2 else 0
    for i in range(1, self.nz) :
      self.plasma[i] = self.plasma[0].copy()



  def test_set(self) :
    D=self.nz * self.dx / 13
    for i in range(0, self.nz) :
      for j in range(0, self.nx) :
        z = (i - 3*self.nz/4) * self.dz
        x = (j - self.nx/3) * self.dx
        self.ez[i, j] = math.cos(z/D*math.pi/2) * math.cos(x/D*math.pi/2) * math.sin((z+x)/D*math.pi*3) if abs(x) < D and abs(z) < D else 0


  def set_t(self, t) :
    self.t = t
    self.shft += int(self.t * self.V / self.dz)

  def step(self, dt) :
    '''
    d ex/dt = d by/dz + jx
    d ez/dt = -d by/dx + jz
    d by/dt = d ex/dz - d ez/dx
    '''
    if self.t * self.V - (self.shft - self.shft0) * self.dz > self.BS * self.dz :
      shift(self.jz, self.BS)
      shift(self.jx, self.BS)
      shift(self.ez, self.BS)
      shift(self.ex, self.BS)
      shift(self.by, self.BS)
      shift(self.byx, self.BS)
      shift(self.byz, self.BS)
      self.shft += self.BS

    nj=8

    #ex
    for i in range(1, self.nz) :
      self.ex[i] += ((self.by[i] - self.by[i-1]) / self.dz - self.jx[i]) * dt
    
    #ez
    for i in range(1, self.nx) :
      self.ez[:, i] = self.PMLexp[i] * self.ez[:, i] + (-(self.by[:, i] - self.by[:, i-1]) / self.dx - self.jz[:, i]) * dt
    
    #By:
    #PML
    for i in range(0,self.nz-1):
      self.byx[i] = np.multiply(self.PMLexp, self.byx[i]) + np.multiply(self.PML, (self.ex[i+1] - self.ex[i]) / self.dz) * dt
    
    #regular
    for i in range(0,self.nz-1):
      self.by[i] += np.multiply(self.regular, (self.ex[i+1] - self.ex[i]) / self.dz) * dt
    
    #PML
    for i in range(0, self.PMLi0):
      self.byz[:, i] -= (self.ez[:, i+1] - self.ez[:, i]) / self.dx * dt
    for i in range(self.PMLi1, self.nx-1):
      self.byz[:, i] -= (self.ez[:, i+1] - self.ez[:, i]) / self.dx * dt
    
    #regular
    for i in range(0, self.nx-1):
      self.by[:, i] -= (self.ez[:, i+1] - self.ez[:, i]) / self.dx * dt


    #add up
    for i in range(0, self.PMLi0):
      self.by[:, i] = self.byx[:, i] + self.byz[:, i] 
    for i in range(self.PMLi1, self.nx):
      self.by[:, i] = self.byx[:, i] + self.byz[:, i] 
        
    self.jx += (np.multiply(self.plasma, self.ex) - self.nu * self.jx) * dt
    self.jz += (np.multiply(self.plasma, self.ez) - self.nu * self.jz) * dt
    
    self.t += dt


class fdtdTE_plasma :
  def __init__(self, nz, nx, dz, dx, nplasma, nu, V, xPML=None, shft0=None):
    self.nz=nz
    self.nx=nx
    self.dz=dz
    self.dx=dx
    self.bz = np.zeros((nz, nx), dtype=np.float32)
    self.bx = np.zeros((nz, nx), dtype=np.float32)
    self.jy = np.zeros((nz, nx), dtype=np.float32)
    self.ey = np.zeros((nz, nx), dtype=np.float32)
    self.eyz = np.zeros((nz, nx), dtype=np.float32)
    self.eyx = np.zeros((nz, nx), dtype=np.float32)
    self.plasma = np.zeros((nz, nx), dtype=np.float32)
    self.regular = np.zeros(nx, dtype=np.float32)
    self.PML = np.zeros(nx, dtype=np.float32)
    self.PMLexp = np.zeros(nx, dtype=np.float32)
    self.np = np
    self.dt = math.sqrt(1 / (1/dx**2 + 1 / dz**2))
    self.V = V
    self.nu = nu
    if shft0 is None:
      self.shft = 0
      self.shft0 = 0
    else:
      self.shft0 = int(shft0/dz)
      self.shft = self.shft0
    self.BS = 16
    self.t = 0
    if xPML is None:
      self.xPML = self.nx * self.dx * 0.1
    else:
      self.xPML = xPML
      
    self.plasmaPos = 3/4*self.nx*self.dx

    self.PMLi0 = math.ceil(self.xPML/self.dx)
    self.PMLi1 = self.nx #math.floor((self.nx*self.dx - self.xPML)/self.dx)
    
    #self.fdtdlib=ctypes.cdll.LoadLibrary("./libfdtd.so")
    
    for i in range(0, self.PMLi0) :
      self.regular[i] = 0
      self.PML[i] = 1
    for i in range(self.PMLi0, self.PMLi1) :
      self.regular[i] = 1
      self.PML[i] = 0
    for i in range(self.PMLi1, self.nx) :
      self.regular[i] = 0
      self.PML[i] = 1
    
    for i in range(0, self.nx) :
      x = i * self.dx
      
      PMLstrength = 0
      
      if x < self.xPML :
        PMLstrength = (self.xPML-x) / self.xPML
      
      #if x > self.nx * self.dx - self.xPML :
      #  PMLstrength = (x - (self.nx * self.dx - self.xPML)) / self.xPML
      
      self.PMLexp[i] = 1 - PMLstrength * 0.03 #math.exp( - 333 * PMLstrength**2 * self.dt)

    for j in range(0, self.nx) :
      self.plasma[0, j] = nplasma if j*self.dx > self.plasmaPos else 0
    for i in range(1, self.nz) :
      self.plasma[i] = self.plasma[0].copy()



  def test_set(self) :
    D=self.nz * self.dx / 13
    for i in range(0, self.nz) :
      for j in range(0, self.nx) :
        z = (i - 3*self.nz/4) * self.dz
        x = (j - self.nx/3) * self.dx
        self.bz[i, j] = math.cos(z/D*math.pi/2) * math.cos(x/D*math.pi/2) * math.sin((z+x)/D*math.pi*3) if abs(x) < D and abs(z) < D else 0

  def set_t(self, t) :
    self.t = t
    self.shft += int(self.t * self.V / self.dz)

  def step(self, dt) :
    '''
    d bx/dt = d ey/dz
    d bz/dt = -d ey/dx
    d ey/dt = d bx/dz - d bz/dx + jy
    '''
    if self.t * self.V - (self.shft - self.shft0) * self.dz > self.BS * self.dz :
      shift(self.jy, self.BS)
      shift(self.bz, self.BS)
      shift(self.bx, self.BS)
      shift(self.ey, self.BS)
      shift(self.eyx, self.BS)
      shift(self.eyz, self.BS)
      self.shft += self.BS

    #ex
    for i in range(1, self.nz) :
      self.bx[i] += (self.ey[i] - self.ey[i-1]) / self.dz * dt
    
    #ez
    for i in range(1, self.nx) :
      self.bz[:, i] = self.PMLexp[i] * self.bz[:, i] - (self.ey[:, i] - self.ey[:, i-1]) / self.dx * dt
    
    #By:
    #PML
    for i in range(0,self.nz-1):
      self.eyx[i] = np.multiply(self.PMLexp, self.eyx[i]) + np.multiply(self.PML, (self.bx[i+1] - self.bx[i]) / self.dz) * dt
    
    #regular
    for i in range(0,self.nz-1):
      self.ey[i] += np.multiply(self.regular, (self.bx[i+1] - self.bx[i]) / self.dz - self.jy[i]) * dt
    
    #PML  
    for i in range(0, self.PMLi0):
      self.eyz[:, i] -= (self.bz[:, i+1] - self.bz[:, i]) / self.dx * dt
    for i in range(self.PMLi1, self.nx-1):
      self.eyz[:, i] -= (self.bz[:, i+1] - self.bz[:, i]) / self.dx * dt
    
    #regular
    for i in range(0, self.nx-1):
      self.ey[:, i] -= (self.bz[:, i+1] - self.bz[:, i]) / self.dx * dt

    #add up
    for i in range(0, self.PMLi0):
      self.ey[:, i] = self.eyx[:, i] + self.eyz[:, i] 
    for i in range(self.PMLi1, self.nx):
      self.ey[:, i] = self.eyx[:, i] + self.eyz[:, i] 
        
    self.jy += (np.multiply(self.plasma, self.ey) - self.nu * self.jy) * dt
    
    self.t += dt

  def stepC(self, dt) :
    '''
    d bx/dt = d ey/dz
    d bz/dt = -d ey/dx
    d ey/dt = d bx/dz - d bz/dx + jy
    '''
    if self.t * self.V - (self.shft - self.shft0) * self.dz > self.BS * self.dz :
      shift(self.jy, self.BS)
      shift(self.bz, self.BS)
      shift(self.bx, self.BS)
      shift(self.ey, self.BS)
      shift(self.eyx, self.BS)
      shift(self.eyz, self.BS)
      self.shft += self.BS
    
    f = self.fdtdlib.stepTE
    
    #void stepTE(int nz, int nx, FL_DBL* bx, FL_DBL* bz, FL_DBL* eyx, FL_DBL* eyz, FL_DBL* ey, FL_DBL* jy, FL_DBL* PMLexp, FL_DBL* plasma, int PMLi0, int PMLi1, FL_DBL nu, FL_DBL dz, FL_DBL dx, FL_DBL dt) 
    
    f.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_int, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    
    f(ctypes.c_int(self.nz), ctypes.c_int(self.nx), self.bx.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), self.bz.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), self.eyx.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), self.eyz.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), self.ey.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), self.jy.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), self.PMLexp.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), self.plasma.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), ctypes.c_int(self.PMLi0), ctypes.c_int(self.PMLi1), ctypes.c_double(self.nu), ctypes.c_double(self.dz), ctypes.c_double(self.dx), ctypes.c_double(dt))
    
    self.t += dt
