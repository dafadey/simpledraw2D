#include <iostream>
#include <sstream>
#include <string>

#include "simpledraw2D.h"

void add(float* arr, int nx, int ny, float x0, float y0, float w, float a)
{
  std::cout << "adding " << x0 << ' ' << y0 << ' ' << w << ' ' << a << '\n';

	for(int j=0;j<ny;j++) {
	  for(int i=0;i<nx;i++) {
	    float x=float(i)/float(nx)-x0;
	    float y=float(j)/float(ny)-y0;
	    arr[i+j*nx] += a/(.3+(x*x+y*y)/(w*w));
	  }
	}
}

int main()
{
  std::string cmd;
	int nx=128;
	int ny=131;
	fadey_init(nx,ny,3);
	float* arr = static_cast<float*>(malloc(nx*ny*4));
	for(int i=0; i<nx*ny;i++)
	  arr[i] = .0;
	fadey_draw(arr, nx, ny, 0);
  while(cmd!="exit")
  {
	  getline(std::cin, cmd);
	  std::stringstream ss(cmd);
	  float x0, y0, w, a;
	  ss >> x0;
	  ss >> y0;
	  ss >> w;
	  ss >> a;
	  add(arr, nx, ny, x0, y0, w, a);
  	fadey_draw(arr, nx, ny, 0);
  }
	
	fadey_close();
	std::cout << "bye\n";
}
