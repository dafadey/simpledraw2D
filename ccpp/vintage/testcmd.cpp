#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

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


typedef void (__stdcall *fadey_init_t)(int*,int*,int*);

typedef void (__stdcall *fadey_draw_real4_t)(float*, int*,int*,int*);

typedef void (__stdcall *fadey_close_t)();

fadey_init_t fadey_init = 0;

fadey_draw_real4_t fadey_draw_real4 = 0;

fadey_close_t fadey_close = 0;


int main()
{
	HINSTANCE hGetProcIDDLL = LoadLibrary("C:\\Users\\dan\\simpledraw2D\\MSVC2015x86\\Release\\simpledraw2D.dll");
	std::cout << "hGetProcIDDLL=" << hGetProcIDDLL << '\n';
	
	fadey_init = (fadey_init_t) GetProcAddress(hGetProcIDDLL, "_fadey_init_@12");
	fadey_draw_real4 = (fadey_draw_real4_t) GetProcAddress(hGetProcIDDLL, "_fadey_draw_real4_@16");
	fadey_close = (fadey_close_t) GetProcAddress(hGetProcIDDLL, "_fadey_close_");

	std::cout << "fadey_init=" << fadey_init << '\n';
	std::cout << "fadey_draw_real4" << fadey_draw_real4 << '\n';
	std::cout << "fadey_close=" << fadey_close << '\n';

	std::string cmd;
	int nx=128;
	int ny=131;
	int count = 3;
	int id0=0;
	fadey_init(&nx,&ny,&count);
	float* arr = static_cast<float*>(malloc(nx*ny*4));
	for(int i=0; i<nx*ny;i++)
	  arr[i] = .0;
	fadey_draw_real4(arr, &nx, &ny, &id0);
	while(cmd!="exit")
	{
	  std::getline(std::cin, cmd);
	  std::stringstream ss(cmd);
	  float x0, y0, w, a;
	  ss >> x0;
	  ss >> y0;
	  ss >> w;
	  ss >> a;
	  add(arr, nx, ny, x0, y0, w, a);
  	  fadey_draw_real4(arr, &nx, &ny, &id0);
	}
	
	fadey_close();
	std::cout << "bye\n";
	return 0;
}
