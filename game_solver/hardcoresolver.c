#define PY_SSIZE_T_CLEAN
#include <Python.h> /* includes stdio, string, stdlib, and errno */

#include "hardcoresolver.h"
#include "testhardcoresolver.h"
#include "pytypes.h"

PyObject* solve_game(PyObject* self, PyObject* args) {
    double timeout;
    uint32 num_iterations;
    game_tree game;
    game_tree solved_game;
    PyObject* pydict;
    PyObject* pygame;
    PyObject* return_value;

    PyArg_ParseTuple(args, PY_DICTDOUBLE, &pydict, &timeout);

    game = pydict_to_game_tree(pydict);
    solved_game = _solve_game(game, &num_iterations, timeout);
    pygame = game_tree_to_pydict(solved_game);
    return_value = PyTuple_New(2);

    PyTuple_SetItem(return_value, 0, pygame);
    PyTuple_SetItem(return_value, 1, Py_BuildValue(PY_INT, num_iterations));

    return return_value;
}

PyObject* run_test_suite(PyObject* self, PyObject* args) {
    uint32 result;
    PyObject* return_value;

    result = _run_test_suite();
    return_value = Py_BuildValue(PY_INT, result);

    return return_value;
}

static struct PyMethodDef methods[] = {
    { "solve_game", (PyCFunction) solve_game, METH_VARARGS },
    { "run_test_suite", (PyCFunction) run_test_suite, METH_NOARGS },
    { NULL, NULL }
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "_hardcoresolver",
    NULL,
    -1,
    methods
};

PyMODINIT_FUNC PyInit__hardcoresolver(void) {
    return PyModule_Create(&module);
}
