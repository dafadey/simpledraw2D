#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <simpledraw2D.h> //simpledraw2D

int main()
{
	printf("hi!\n");
	int nx=111;
	int ny=111;
	int count=3;
	int tile=0;
	fadey_init_(&nx,&ny,&count); //simpledraw2D
	float* arr = malloc(nx*ny*4);
	for(int j=0;j<ny;j++) {
	  for(int i=0;i<nx;i++) {
	    float x=(i-nx/2)*.1;
	    float y=(j-ny/2)*.1;
	    arr[i+j*nx] = 1./(.3+(x*x+y*y));
	  }
	}
	fadey_draw_real4_(arr, &nx, &ny, &tile); //simpledraw2D
	getchar();
	fadey_close_(); //simpledraw2D
	printf("bye\n");
}
