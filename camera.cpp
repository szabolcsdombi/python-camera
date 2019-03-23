#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#define v_xyz(obj) &obj.x, &obj.y, &obj.z
#define v_xyzw(obj) &obj.x, &obj.y, &obj.z, &obj.w

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

Camera * meth_interpolate(PyObject * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"camera1", "camera2", "coeff", NULL};

    Camera * camera1;
    Camera * camera2;
    float coeff;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!f", keywords, Camera_type, &camera1, Camera_type, camera2, &coeff)) {
        return 0;
    }

    Camera * camera = (Camera *)PyObject_New(Camera, Camera_type);

    float h_diff = camera2->h - camera1->h;
    if (h_diff > glm::pi<float>()) {
        h_diff -= glm::pi<float>() * 2.0f;
    }
    if (h_diff < -glm::pi<float>()) {
        h_diff += glm::pi<float>() * 2.0f;
    }

    camera->position = glm::mix(camera1->position, camera2->position, coeff);
    camera->h = glm::mod(camera1->h + h_diff * coeff, glm::pi<float>() * 2.0f);
    camera->v = glm::clamp(glm::mix(camera1->v, camera2->v, coeff), -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);
    camera->fov = glm::mix(camera1->fov, camera2->fov, coeff);

    return camera;
}

PyObject * Camera_meth_move(Camera * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"forward", "left", "up", NULL};

    float move_forward = 0.0f;
    float move_left = 0.0f;
    float move_up = 0.0f;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|fff", keywords, &move_forward, &move_left, &move_up)) {
        return 0;
    }

    glm::vec3 forward = glm::vec3(cosf(self->h) * cosf(self->v), sinf(self->h) * cosf(self->v), sinf(self->v));
    glm::vec3 left = glm::vec3(-sinf(self->h), cosf(self->h), 0.0f);

    self->position = self->position + forward * move_forward + left * move_left;
    self->position.z += move_up;

    Py_RETURN_NONE;
}

PyObject * Camera_meth_turn(Camera * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"left", "up", NULL};

    float turn_left = 0.0f;
    float turn_up = 0.0f;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ff", keywords, &turn_left, &turn_up)) {
        return 0;
    }

    self->h = glm::mod(self->h + glm::radians(turn_left), 2.0f * glm::pi<float>());
    self->v = glm::clamp(self->v + glm::radians(turn_up), -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);

    Py_RETURN_NONE;
}

PyObject * Camera_meth_focus(Camera * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"target", NULL};

    glm::vec3 target;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(fff)", keywords, v_xyz(target))) {
        return 0;
    }

    glm::vec3 forward = target - self->position;
    self->h = atan2f(forward.y, forward.x);
    self->v = atan2f(forward.z, sqrtf(forward.x * forward.x + forward.y * forward.y));
    Py_RETURN_NONE;
}

PyObject * Camera_meth_matrix(Camera * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"ratio", "near", "far", NULL};

    float ratio = 1.0f;
    float znear = 0.1f;
    float zfar = 1000.0f;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|fff", keywords, &ratio, &znear, &zfar)) {
        return 0;
    }

    glm::vec3 forward = glm::vec3(cosf(self->h) * cosf(self->v), sinf(self->h) * cosf(self->v), sinf(self->v));
    glm::vec3 up = glm::vec3(-cosf(self->h) * sinf(self->v), -sinf(self->h) * sinf(self->v), cosf(self->v));
    glm::mat4 matrix = glm::perspective(glm::radians(self->fov), ratio, znear, zfar);
    matrix *= glm::lookAt(self->position, self->position + forward, up);
    return PyBytes_FromStringAndSize((char *)&matrix, sizeof(matrix));
}

PyObject * Camera_meth_project(Camera * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"point", "viewport", "near", "far", NULL};

    glm::vec3 point;
    glm::ivec4 viewport;
    float znear = 0.1f;
    float zfar = 1000.0f;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(fff)(iiii)|ff", keywords, v_xyz(point), v_xyzw(viewport), &znear, &zfar)) {
        return 0;
    }

    float ratio = (float)viewport.z / viewport.w;
    glm::vec3 forward = glm::vec3(cosf(self->h) * cosf(self->v), sinf(self->h) * cosf(self->v), sinf(self->v));
    glm::vec3 up = glm::vec3(-cosf(self->h) * sinf(self->v), -sinf(self->h) * sinf(self->v), cosf(self->v));
    glm::mat4 proj = glm::perspective(glm::radians(self->fov), ratio, znear, zfar);
    glm::mat4 view = glm::lookAt(self->position, self->position + forward, up);
    glm::vec3 result = glm::project(point, view, proj, viewport);
    return Py_BuildValue("fff", result.x, result.y, result.z);
}

PyObject * Camera_meth_unproject(Camera * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"point", "viewport", "near", "far", NULL};

    glm::vec3 point;
    glm::ivec4 viewport;
    float znear = 0.1f;
    float zfar = 1000.0f;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(fff)(iiii)|ff", keywords, v_xyz(point), v_xyzw(viewport), &znear, &zfar)) {
        return 0;
    }

    float ratio = (float)viewport.z / viewport.w;
    glm::vec3 forward = glm::vec3(cosf(self->h) * cosf(self->v), sinf(self->h) * cosf(self->v), sinf(self->v));
    glm::vec3 up = glm::vec3(-cosf(self->h) * sinf(self->v), -sinf(self->h) * sinf(self->v), cosf(self->v));
    glm::mat4 view = glm::perspective(glm::radians(self->fov), ratio, znear, zfar);
    glm::mat4 proj = glm::lookAt(self->position, self->position + forward, up);
    glm::vec3 result = glm::unProject(point, view, proj, viewport);
    return Py_BuildValue("fff", result.x, result.y, result.z);
}

PyObject * Camera_get_position(Camera * self) {
    return Py_BuildValue("fff", self->position.x, self->position.y, self->position.z);
}

PyObject * Camera_get_target(Camera * self) {
    glm::vec3 target = self->position + glm::vec3(cosf(self->h) * cosf(self->v), sinf(self->h) * cosf(self->v), sinf(self->v));
    return Py_BuildValue("fff", target.x, target.y, target.z);
}

void Camera_tp_dealloc(Camera * camera) {
    Py_TYPE(camera)->tp_free(camera);
}

PyMethodDef Camera_methods[] = {
    {"move", (PyCFunction)Camera_meth_move, METH_VARARGS | METH_KEYWORDS, 0},
    {"turn", (PyCFunction)Camera_meth_turn, METH_VARARGS | METH_KEYWORDS, 0},
    {"focus", (PyCFunction)Camera_meth_focus, METH_VARARGS | METH_KEYWORDS, 0},
    {"matrix", (PyCFunction)Camera_meth_matrix, METH_VARARGS | METH_KEYWORDS, 0},
    {"project", (PyCFunction)Camera_meth_project, METH_VARARGS | METH_KEYWORDS, 0},
    {"unproject", (PyCFunction)Camera_meth_unproject, METH_VARARGS | METH_KEYWORDS, 0},
    {0},
};

PyGetSetDef Camera_getset[] = {
    {"position", (getter)Camera_get_position, 0, 0},
    {"target", (getter)Camera_get_target, 0, 0},
    {0},
};

PyMemberDef Camera_members[] = {
    {"fov", T_FLOAT, offsetof(Camera, fov), 0, 0},
    {0},
};

PyType_Slot Camera_slots[] = {
    {Py_tp_methods, Camera_methods},
    {Py_tp_members, Camera_members},
    {Py_tp_getset, Camera_getset},
    {Py_tp_dealloc, Camera_tp_dealloc},
    {0},
};

PyType_Spec Camera_spec = {"camera.Camera", sizeof(Camera), 0, Py_TPFLAGS_DEFAULT, Camera_slots};

PyMethodDef module_methods[] = {
    {"camera", (PyCFunction)meth_camera, METH_VARARGS | METH_KEYWORDS, 0},
    {"interpolate", (PyCFunction)meth_interpolate, METH_VARARGS | METH_KEYWORDS, 0},
    {0},
};

PyModuleDef module_def = {PyModuleDef_HEAD_INIT, "camera", 0, -1, module_methods, 0, 0, 0, 0};

extern "C" PyObject * PyInit_camera() {
    PyObject * module = PyModule_Create(&module_def);
    Camera_type = (PyTypeObject *)PyType_FromSpec(&Camera_spec);
    return module;
}
