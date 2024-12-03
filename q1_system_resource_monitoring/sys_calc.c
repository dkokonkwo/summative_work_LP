#include <Python.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "monitor.h"

/**
 * sys_monitor - monitor and logs system performance metrics
 * @self: object
 * @args: arguments
 * Return: Success else failure
 */
static PyObject *sys_monitor(PyObject *self, PyObject *args)
{
    int success;
    int runtime;
    if (!PyArg_ParseTuple(args, "i", &runtime))
        return NULL;
    if (runtime <= 5)
    {
        return PyErr_Format(PyExc_RuntimeError, "Process should run for more than 5 times.");
    }
    success = system_monitor(runtime);
    if (!success)
    {
        return PyErr_Format(PyExc_RuntimeError, "An error occurred.");
    }
    else
    {
        return Py_BuildValue("s", "Done");
    }
}

static PyMethodDef SysMethods[] = {
    {"monitor", sys_monitor, METH_VARARGS, "Logs system performace metrics to text file"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef sysmodule = {
    PyModuleDef_HEAD_INIT,
    "sysmonitor",
    "A module system performance metrics",
    -1,
    SysMethods
};

PyMODINIT_FUNC PyInit_sysmonitor(void)
{
    return PyModule_Create(&sysmodule);
}