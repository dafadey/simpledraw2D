#include <windows.h>
#include <stdio.h>
#include "simpledraw2D.h"

void add(float* arr, int nx, int ny, float x0, float y0, float w, float a)
{
	int i,j;
	float x,y;
	printf("adding %f %f %f %f\n", x0, y0, w, a);
	for(j=0;j<ny;j++) {
	  for(i=0;i<nx;i++) {
	    x = (float) i / (float) nx - x0;
	    y = (float) j / (float) ny - y0;
	    arr[i+j*nx] += a/(.3f + (x*x + y*y) / (w*w));
	  }
	}
}

#define CMDSZ 128

void eraseCMD(char* cmd){
	int i;
	for(i=0; i<CMDSZ; i++)
		cmd[i] = 0;
}

void getCMD(char* cmd) {
	char c;
	int i;
	c = 0;
	i=0;
	while(i<CMDSZ) {
		c = getchar();
		if(c==0 || c==13 || c==10)
			break;
		cmd[i]=c;
		i += 1;
	}
}

void readFloats(float** fs, const char* cmd)
{
	char c;
	int i, ii, id;
	char item[CMDSZ];
	i=0;
	ii=0;
	id=0;
	eraseCMD(item);
	while(i<CMDSZ){
		c = cmd[i];
		if(c==0 || c==13 || c==10)
			break;
		if(cmd[i] == ' ') {
			if(id<4) {
				*fs[id] = (float) atof(item);
				id += 1;
			}
			eraseCMD(item);
			ii=0;
		}
		else {
			item[ii] = c;
			ii += 1;
		}
		i += 1;
	}
	if(id<4)
		*fs[id] = (float) atof(item);

}

//extern void __stdcall fadey_init_(int*, int*, int*);
//extern void __stdcall fadey_draw_real4_(float*, int*, int*, int*);
//extern void __stdcall fadey_close_();

int main()
{
	float* arr;
	float* args[4];
	float x0, y0, w, a;
	char cmd[CMDSZ];
	int i,nx,ny,count,id0;
	args[0] = &x0;
	args[1] = &y0;
	args[2] = &w;
	args[3] = &a;
	eraseCMD(cmd);
	printf("Hi!\n");
	printf("fadey_init=%p\n", fadey_init_);
	printf("fadey_draw=%p\n", fadey_draw_real4_);
	printf("fadey_close=%p\n", fadey_close_);
	nx=128;
	ny=131;
	count = 3;
	id0=0;
	fadey_init_(&nx,&ny,&count);
	arr = (float*) malloc(nx*ny*4);
	for(i=0; i<nx*ny;i++)
	  arr[i] = .0;
	fadey_draw_real4_(arr, &nx, &ny, &id0);
	while(cmd!="exit")
	{
	  getCMD(cmd);
	  readFloats(args, cmd);      
	  add(arr, nx, ny, x0, y0, w, a);
  	  fadey_draw_real4_(arr, &nx, &ny, &id0);
	}
	
	fadey_close_();
	printf("bye\n");
	return 0;
}
