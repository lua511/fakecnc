#include <Python.h>
#include <string.h>
#include <map>
using namespace std;

PyObject *pyhal_error_type = NULL;

/************************************************************************/
/* object section                                                       */
/************************************************************************/

struct halcomp
{
	PyObject_HEAD
	char* name;
	int	hal_id;
};

static void pyhal_delete(PyObject *_self) {
	halcomp *self = (halcomp *)_self;
	free(self->name);
	self->name = 0;
	self->ob_type->tp_free(self);
}

static PyObject *pyhal_repr(PyObject *_self) {
	halcomp *self = (halcomp *)_self;
	return PyString_FromFormat("<hal component %s(%d) with %d pins and params>",
		self->name, self->hal_id, 0);
}

static int pyhal_init(PyObject *_self, PyObject *args, PyObject *kw) {
	char *name;
	char *prefix = 0;
	halcomp *self = (halcomp *)_self;

	if (!PyArg_ParseTuple(args, "s|s:hal.component", &name, &prefix)) return -1;

	self->hal_id = 1;

	self->name = strdup(name);
	return 0;
}

static PyObject * whoami(PyObject *_self, PyObject *args)
{
	return PyString_FromString("fakecnc-hal.component");
}

static PyMethodDef hal_methods[] = {
	{ "whoami", whoami, METH_NOARGS,
	"who am i" },
	{ NULL },
};

static PyTypeObject halcomp_type=
{
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"hal.component",           /*tp_name*/
	sizeof(halcomp),         /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	pyhal_delete,              /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	pyhal_repr,                /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,            /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,            /*tp_getattro*/
	0,            /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
	"HAL Component",           /*tp_doc*/
	0,                         /*tp_traverse*/
	0,                         /*tp_clear*/
	0,                         /*tp_richcompare*/
	0,                         /*tp_weaklistoffset*/
	0,                         /*tp_iter*/
	0,                         /*tp_iternext*/
	hal_methods,               /*tp_methods*/
	0,                         /*tp_members*/
	0,                         /*tp_getset*/
	0,                         /*tp_base*/
	0,                         /*tp_dict*/
	0,                         /*tp_descr_get*/
	0,                         /*tp_descr_set*/
	0,                         /*tp_dictoffset*/
	pyhal_init,                /*tp_init*/
	0,                         /*tp_alloc*/
	PyType_GenericNew,         /*tp_new*/
	0,                         /*tp_free*/
	0,                         /*tp_is_gc*/
};

/************************************************************************/
/* pin section                                                          */
/************************************************************************/

// make a simplest object

struct pyhalitem
{
	PyObject_HEAD
	double pin;
	char* name;
};
static PyObject * pyhal_pin_set(PyObject *_self, PyObject *value)
{
	pyhalitem *self = (pyhalitem*)_self;
	if (PyFloat_Check(value))
	{
		self->pin = PyFloat_AsDouble(value);
	}
	else
	{
		return NULL;
	}
	Py_RETURN_NONE;
}
static PyObject* pyhal_pin_get(PyObject * _self, PyObject *)
{
	pyhalitem* self = (pyhalitem*)_self;
	return PyFloat_FromDouble(self->pin);
}

static PyObject * pyhal_pin_get_type(PyObject* _self, PyObject *)
{
	return PyInt_FromLong(2);
}

static PyObject* pyhal_pin_get_dir(PyObject * _self, PyObject *)
{
	return PyInt_FromLong((int)64 | (int)128);
}

static PyObject * pyhal_pin_get_name(PyObject * _self, PyObject *) {
	pyhalitem * self = (pyhalitem *)_self;
	if (!self->name)
		Py_RETURN_NONE;
	return PyString_FromString(self->name);
}

static PyObject * pyhal_pin_is_pin(PyObject * _self, PyObject *) {
	pyhalitem * self = (pyhalitem *)_self;
	return PyBool_FromLong(1);
}

struct PyMethodDef halpin_methods[] =
{
	{ "set", pyhal_pin_set, METH_O, "Set item value" },
	{ "get", pyhal_pin_get, METH_NOARGS, "Get item value" },
	{ "get_type", pyhal_pin_get_type, METH_NOARGS, "Get item type" },
	{ "get_dir", pyhal_pin_get_dir, METH_NOARGS, "Get item direction" },
	{ "get_name", pyhal_pin_get_name, METH_NOARGS, "Get item name" },
	{ "is_pin", pyhal_pin_is_pin, METH_NOARGS, "If item is pin or param" },
	{ NULL },
};

static void pyhalpin_delete(PyObject *_self)
{
	pyhalitem *self = (pyhalitem*)_self;
	if (self->name) free(self->name);
	PyObject_Del(self);
}

static PyObject *pyhalpin_repr(PyObject *_self) {
	pyhalitem *pyself = (pyhalitem *)_self;

	const char * name = "(null)";
	if (pyself->name) name = pyself->name;

	return PyString_FromFormat("<hal param \"%s\" FLOAT-RW>", name);
}

static int pyhalpin_init(PyObject *_self, PyObject *, PyObject *) {
	PyErr_Format(PyExc_RuntimeError,
		"Cannot be constructed directly");
	return -1;
}

static PyTypeObject halpin_type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"hal.item",                /*tp_name*/
	sizeof(pyhalitem),         /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	pyhalpin_delete,           /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	pyhalpin_repr,             /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,        /*tp_flags*/
	"HAL Pin",                 /*tp_doc*/
	0,                         /*tp_traverse*/
	0,                         /*tp_clear*/
	0,                         /*tp_richcompare*/
	0,                         /*tp_weaklistoffset*/
	0,                         /*tp_iter*/
	0,                         /*tp_iternext*/
	halpin_methods,            /*tp_methods*/
	0,                         /*tp_members*/
	0,                         /*tp_getset*/
	0,                         /*tp_base*/
	0,                         /*tp_dict*/
	0,                         /*tp_descr_get*/
	0,                         /*tp_descr_set*/
	0,                         /*tp_dictoffset*/
	pyhalpin_init,             /*tp_init*/
	0,                         /*tp_alloc*/
	PyType_GenericNew,         /*tp_new*/
	0,                         /*tp_free*/
	0,                         /*tp_is_gc*/
};

static PyObject * halecho(PyObject *self, PyObject *args)
{
	const char* command;
	int sts;
	if (!PyArg_ParseTuple(args, "s", &command))
		return NULL;
	sts = system(command);
	if (sts < 0){
		PyErr_SetString(pyhal_error_type, "system command failed");
		return NULL;
	}
	return PyLong_FromLong(sts);
}

PyMethodDef module_methods[] =
{
	{ "halecho", halecho, METH_VARARGS, "the echo" },
	{ NULL },
};

const char *module_doc = "interface to emc2's hal";

typedef	struct  
{
	PyObject_HEAD
	int pid;
	char* name;
}	SimpleObject;


extern "C"
void init_hal(void)
{
	PyObject* m = Py_InitModule3("_hal", module_methods, module_doc);
	pyhal_error_type = PyErr_NewException((char*)"hal.error", NULL, NULL);
	PyModule_AddObject(m, "error", pyhal_error_type);
	PyType_Ready(&halcomp_type);
	PyModule_AddObject(m, "component", (PyObject*)&halcomp_type);
}
