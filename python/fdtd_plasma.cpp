#define FL_DBL float
#define THS 8
#include <iostream>

extern "C"
void stepTE(int nz, int nx, FL_DBL* bx, FL_DBL* bz, FL_DBL* eyx, FL_DBL* eyz, FL_DBL* ey, FL_DBL* jy, FL_DBL* PMLexp, FL_DBL* plasma, int PMLi0, int PMLi1, FL_DBL nu, FL_DBL dz, FL_DBL dx, FL_DBL dt) {
  FL_DBL dz_1 = 1.0/dz;
  FL_DBL dx_1 = 1.0/dx;
  
  //ex:
  #pragma omp parallel for num_threads(THS)
  for(int i=1; i<nz; i++) {
    for(int j=0; j<nx; j++)
      bx[i*nx+j] += (ey[i*nx+j] - ey[(i-1)*nx+j]) * dz_1 * dt;
  }

  //ez:
  #pragma omp parallel for num_threads(THS)
  for(int i=0; i<nz; i++) {
    for(int j=1; j<nx; j++)
      bz[i*nx + j] = PMLexp[j] * bz[i*nx + j] - (ey[i*nx + j] - ey[i*nx + j-1]) * dx_1 * dt;
  }

  //By:
  #pragma omp parallel for num_threads(THS)
  for(int i=0; i<nz-1; i++) {
    for(int j=0; j<PMLi0; j++) {
      eyx[i*nx+j] = PMLexp[j] * eyx[i*nx+j] + (bx[(i+1)*nx+j] - bx[i*nx+j]) * dz_1 * dt;
      eyz[i*nx+j] -= (bz[i*nx+j+1] - bz[i*nx+j]) * dx_1 * dt;
    }
    
    for(int j=PMLi0; j<PMLi1; j++)
      ey[i*nx+j] += (-(bz[i*nx + j+1] - bz[i*nx + j]) / dx + (bx[(i+1)*nx + j] - bx[i*nx + j]) / dz - jy[i*nx + j]) * dt;

    for(int j=PMLi1; j<nx; j++)
      eyx[i*nx+j] = PMLexp[j] * eyx[i*nx+j] + (bx[(i+1)*nx+j] - bx[i*nx+j]) * dz_1 * dt;
    for(int j=PMLi1; j<nx-1; j++)
      eyz[i*nx+j] -= (bz[i*nx+j+1] - bz[i*nx+j]) * dx_1 * dt;

  }

  #pragma omp parallel for num_threads(THS)
  for(int i=0; i<nz; i++) {
    for(int j=0; j<PMLi0; j++)
      ey[i*nx+j] = eyx[i*nx+j] + eyz[i*nx+j];
    for(int j=PMLi1; j<nx; j++)
      ey[i*nx+j] = eyx[i*nx+j] + eyz[i*nx+j];
  }

  //jy:
  #pragma omp parallel for num_threads(THS)
  for(int i=0; i<nz; i++) {
    for(int j=0; j<nx; j++)
      jy[i*nx+j] += (plasma[i*nx+j] * ey[i*nx+j] - nu * jy[i*nx+j]) * dt;
  }

  //*t+=dt;
}
