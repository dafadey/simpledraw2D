#pragma once
#ifdef __cplusplus
void fadey_init(int, int, int); // size X, size Y, number of tiles
void fadey_halt();
void fadey_draw(float* , int , int , int ); // data, size X, size Y, number tile to update
void fadey_draw(double* , int , int , int ); // data, size X, size Y, number tile to update
void fadey_draw_particles(int, float* , int, int, double=1.0, double=1.0, double=1.0); //index, data (x,y format), number of particles, number tile to update, color (bounds are calculated automatically)
void fadey_draw_particles_reset_bounds(int); //resets bounds for this tile

void fadey_draw_1D(int, float* , int, int, double=1.0, double=1.0, double=1.0); // index, data, size X, number tile to update, color
void fadey_close(); // call on finish
#else
void fadey_init_(int*,int*,int*);
void fadey_halt_();
void fadey_draw_real4_(float*, int*,int*,int*);
void fadey_draw_real8_(double*, int*,int*,int*);
void fadey_draw_particles_real4_(int*, float* , int*, int*, double*, double*, double*);
void fadey_draw_particles_reset_bounds_(int*);
void fadey_draw_1D_real4_(int*, float* , int*, int*, double*, double*, double*);
extern void fadey_close_();
#endif
