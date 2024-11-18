#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <numpy/arrayobject.h>
#include <structmember.h>

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <limits>
#include <chrono>
#include <map>
#include <mutex>

extern "C"
void fadey_init_(int* Nx_ptr, int* Ny_ptr, int* count_ptr);
extern "C"
void fadey_draw_real4_(float* DataArr, int* Nx_ptr, int* Ny_ptr, int* count_ptr);
extern "C"
void fadey_draw_real8_(double* DataArr, int* Nx_ptr, int* Ny_ptr, int* count_ptr);
extern "C"
void fadey_close_();

static PyObject* init(PyObject* self, PyObject** args) {
  int nx=PyLong_AsLong(args[0]);
  int ny=PyLong_AsLong(args[1]);
  int count=PyLong_AsLong(args[2]);
  
  fadey_init_(&nx, &ny, &count);
	return Py_None;
}

static PyObject* draw(PyObject* self, PyObject* args, PyObject* kwargs) {
  std::cout << "hi! it is python draw\n" << std::flush;
  PyObject* pyarr{};
  int id;
	const char* kwlist[] = {"array", "id", NULL};
  const char* argtypes = "|Oi";
	std::cout << "argtypes=\"" << argtypes << "\"\n";
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, argtypes, const_cast<char**>(kwlist), &pyarr, &id)) 
		std::cout << "parse error\n" << std::flush;
  std::cout << "---------" << pyarr << '\n' << std::flush;
  int dim = PyArray_NDIM(pyarr);
  if(dim!=2)
    return Py_None;
	std::cout << "array dim is " << dim << '\n';
	std::cout << "PyArray_Type=" << PyArray_TYPE(pyarr) << " NPY_FLOAT32=" << NPY_FLOAT32 << '\n';
	int nx = PyArray_DIM(pyarr, 0);
	int ny = PyArray_DIM(pyarr, 1);
	std::cout << "got array from python, size is " << nx << 'x' << ny << '\n' << std::flush;
	PyArray_Descr* descr = PyArray_DESCR(pyarr);
	std::cout << "descr_ptr=" << descr << '\n' << std::flush;
	if(PyArray_TYPE(pyarr) == NPY_FLOAT32) {
	  float* arr = (float*) PyArray_DATA(pyarr);
    fadey_draw_real4_(arr, &nx, &ny, &id);
  } else {
	  double* arr = (double*) PyArray_DATA(pyarr);
    fadey_draw_real8_(arr, &nx, &ny, &id);
  }
	return Py_None;
}

static PyObject* finish(PyObject* self) {
  fadey_close_();
	return Py_None;
}

static PyMethodDef simpledraw2Dmethods[] = {
	{"init", (PyCFunction) init, METH_FASTCALL, "provides number of available GPUs"},
	{"draw", (PyCFunction) draw, METH_VARARGS | METH_KEYWORDS, "provides given GPU temperature"},
	{"finish", (PyCFunction) finish, METH_NOARGS, "provides number of available GPUs"},
	{NULL, NULL, 0, NULL}
};

static PyModuleDef simpledraw2D = {
    PyModuleDef_HEAD_INIT,
    .m_name = "simpledraw2D",
    .m_doc = "simple draw library to show 2D arrays with OpenGL",
    .m_size = -1,
    simpledraw2Dmethods
};

PyMODINIT_FUNC
PyInit_simpledraw2D(void)
{
  std::cout << "---> PyInit_simpledraw2D\n" << std::flush;
  PyObject* mod = PyModule_Create(&simpledraw2D);
  import_array();
  return mod;
}
