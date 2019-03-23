#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "glm/glm.hpp"

#define v_xyz(obj) &obj.x, &obj.y, &obj.z

struct Camera {
    PyObject_HEAD
    glm::vec3 position;
    float h, v;
    float fov;
};

PyTypeObject * Camera_type;

Camera * meth_camera(PyObject * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"position", "target", "fov", NULL};

    glm::vec3 position, target;
    float fov = 45.0f;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(fff)(fff)|f", keywords, v_xyz(position), v_xyz(target), &fov)) {
        return 0;
    }

    Camera * camera = (Camera *)PyObject_New(Camera, Camera_type);

    glm::vec3 forward = target - position;
    camera->position = position;
    camera->h = atan2f(forward.y, forward.x);
    camera->v = atan2f(forward.z, sqrtf(forward.x * forward.x + forward.y * forward.y));
    camera->fov = fov;

    return camera;
}

PyType_Slot Camera_slots[] = {
    {0},
};

PyType_Spec Camera_spec = {"camera.Camera", sizeof(Camera), 0, Py_TPFLAGS_DEFAULT, Camera_slots};

PyMethodDef module_methods[] = {
    {"camera", (PyCFunction)meth_camera, METH_VARARGS | METH_KEYWORDS, 0},
    {0},
};

PyModuleDef module_def = {PyModuleDef_HEAD_INIT, "camera", 0, -1, module_methods, 0, 0, 0, 0};

extern "C" PyObject * PyInit_camera() {
    PyObject * module = PyModule_Create(&module_def);
    Camera_type = (PyTypeObject *)PyType_FromSpec(&Camera_spec);
    return module;
}
