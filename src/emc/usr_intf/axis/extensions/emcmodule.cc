#include <Python.h>
// this file is most following python doc
//https://docs.python.org/2/extending/extending.html?highlight=pymodinit_func

static PyObject *cncError;

static PyObject * hellolinuxcnc(PyObject *self, PyObject *args)
{
	const char* command;
	int sts;
	if (!PyArg_ParseTuple(args, "s", &command))
		return NULL;
	sts = system(command);
	if (sts < 0){
		PyErr_SetString(cncError, "system command failed");
		return NULL;
	}
	return PyLong_FromLong(sts);
}


static PyMethodDef LinuxcncMethods[] = {
	{ "hellocnc", hellolinuxcnc, METH_VARARGS, "execute a shell command" },
	{NULL,NULL,0,NULL},
};

PyMODINIT_FUNC
initlinuxcnc(void)
{
	PyObject *m;
	m = Py_InitModule("linuxcnc", LinuxcncMethods);
	if (m == NULL)
		return;
	char errfun[] = "linuxcnc.error";
	cncError = PyErr_NewException(errfun, NULL, NULL);
	Py_INCREF(cncError);
	PyModule_AddObject(m, "error", cncError);
}
