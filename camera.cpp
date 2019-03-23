#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

PyMethodDef module_methods[] = {
    {0},
};

PyModuleDef module_def = {PyModuleDef_HEAD_INIT, "camera", 0, -1, module_methods, 0, 0, 0, 0};

extern "C" PyObject * PyInit_camera() {
    PyObject * module = PyModule_Create(&module_def);
    return module;
}
