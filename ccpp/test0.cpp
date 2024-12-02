#include <iostream>
#include <string>

#include "simpledraw2D.h" //simpledraw2D

int main()
{
  std::string blah;
	int nx=111;
	int ny=111;
	fadey_init(nx,ny,3); //simpledraw2D
	float* arr = static_cast<float*>(malloc(nx*ny*4));
	for(int j=0;j<ny;j++) {
	  for(int i=0;i<nx;i++) {
	    float x=(i-nx/2)*.1;
	    float y=(j-ny/2)*.1;
	    arr[i+j*nx] = 1./(.3+(x*x+y*y));
	  }
	}
	fadey_draw(arr, nx, ny, 0); //simpledraw2D
	std::cin >> blah;

	fadey_close(); //simpledraw2D
  return 0;
}
