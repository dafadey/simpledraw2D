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

	float* arr2 = static_cast<float*>(malloc(nx*ny*4));
	for(int j=0;j<ny;j++) {
	  for(int i=0;i<nx;i++) {
	    float x=(i-nx/3)*.1;
	    float y=(j-ny/3)*.1;
	    float x2=(i-2*nx/3)*.1;
	    float y2=(j-2*ny/3)*.1;
	    arr2[i+j*nx] = 1./(.3+(x*x+y*y)) - 1./(.3+1.7*(x2*x2+y2*y2));
	  }
	}
	fadey_draw(arr2, nx, ny, 1); //simpledraw2D
	std::cin >> blah;

	
	fadey_close(); //simpledraw2D
	std::cout << "bye\n";
}
