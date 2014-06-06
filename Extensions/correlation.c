#include <Python.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <complex.h>
#include <numpy/arrayobject.h>


//#include "/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/Python.framework/Versions/2.6/Extras/lib/python/numpy/core/include/numpy/arrayobject.h"
#undef I


static PyObject* correlation1 (PyObject* self, PyObject *arg)
{
//  Importing numpy array types
    _import_array();

//  Declaring basic variables
    double  Frequency;
	int Increment = 13;

//  Interface with python
    PyObject *VQ_obj, *Time_obj;

    if (!PyArg_ParseTuple(arg, "dOO|i", &Frequency,&VQ_obj,&Time_obj,&Increment))  return NULL;

    PyObject *VQ_array = PyArray_FROM_OTF(VQ_obj, NPY_CDOUBLE, NPY_IN_ARRAY);
    PyObject *Time_array = PyArray_FROM_OTF(Time_obj, NPY_DOUBLE, NPY_IN_ARRAY);

    if (VQ_array == NULL || Time_array == NULL ) {
         Py_XDECREF(VQ_array);
         Py_XDECREF(Time_array);
         return NULL;
    }

    double _Complex *VQ    = (double _Complex * )PyArray_DATA(VQ_array);
    double *Time    = (double*)PyArray_DATA(Time_array);
    int NumberOfData = (int)PyArray_DIM(VQ_array, 0);


//  Starting correlation calculation
	double _Complex Correl = 0;

	for (int i = 0; i< NumberOfData; i += Increment) {
		for (int j = 0; j< (NumberOfData-i-Increment); j++) {
			switch ('R') {
				case 'T': //	Trapezoid Integration
					Correl += (conj(VQ[j])*VQ[j+i+Increment]*cexp(_Complex_I*Frequency*(Time[i+Increment]-Time[0]))
					             +conj(VQ[j])*VQ[j+i]*cexp(_Complex_I*Frequency*(Time[i]-Time[0])))/2.0*(Time[i+Increment] - Time[i]);

					break;
				case 'R': //	Rectangular Integration
					Correl += conj(VQ[j])*VQ[j+i]*cexp(_Complex_I*Frequency*(Time[i] - Time[0]))*(Time[i+Increment] - Time[i]);
					break;
				default:
				    puts ("No correlation method selected");
				    exit(0);
					break;
			}
		}
	}
    return Py_BuildValue("d", creal(Correl)/NumberOfData);
}


static PyObject* correlation2 (PyObject* self, PyObject *arg )
{
//  Importing numpy array types
    _import_array();

//  Declaring basic variables
    double  Frequency;
    double DTime;
 	int Increment =13;   //Default value for Increment

//  Interface with python
    PyObject *VQ_obj;

    if (!PyArg_ParseTuple(arg, "dOd|i", &Frequency,&VQ_obj,&DTime,&Increment))  return NULL;

    PyObject *VQ_array = PyArray_FROM_OTF(VQ_obj, NPY_CDOUBLE, NPY_IN_ARRAY);

    if (VQ_array == NULL ) {
         Py_XDECREF(VQ_array);
         return NULL;
    }

    double _Complex *VQ    = (double _Complex * )PyArray_DATA(VQ_array);
    int NumberOfData = (int)PyArray_DIM(VQ_array, 0);


//  Starting correlation calculation
	double _Complex Correl = 0;

	for (int i = 0; i< NumberOfData; i += Increment) {
		for (int j = 0; j< (NumberOfData-i-Increment); j++) {
			Correl += conj(VQ[j])*VQ[j+i]*cexp(_Complex_I*Frequency*(i*DTime));
		}
	}
    return Py_BuildValue("d", creal(Correl)*DTime/(NumberOfData/Increment));

}


static char extension_docs[] =
    "correlation( ): Calculation of the correlation\n";


static PyMethodDef extension_funcs[] = {
    {"correlation", (PyCFunction)correlation1, METH_VARARGS, NULL},
    {"correlation2", (PyCFunction)correlation2,METH_VARARGS, extension_docs},
    {NULL}
};

void initcorrelation(void)
{
    Py_InitModule3("correlation", extension_funcs,
                   "Fast Correlation Functions ");
}