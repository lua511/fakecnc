#include <Python.h>
#include <string.h>
#include <map>
using namespace std;

PyObject *pyhal_error_type = NULL;

/************************************************************************
* raw datas                                                       
************************************************************************/

struct halcomp
{
	PyObject_HEAD
	char* name;
	char* prefix;
	int	hal_id;
};

struct halitem
{
	PyObject_HEAD
	double pin;
	char* name;
};

/************************************************************************
fun list
************************************************************************/

static PyObject * halecho(PyObject *self, PyObject *args);
PyMethodDef module_methods[] =
{
	{ "halecho", halecho, METH_VARARGS, "the echo" },
	{ NULL },
};
//--
static void pyhalpin_delete(PyObject *_self);
static PyObject *pyhalpin_repr(PyObject *_self);

static PyObject* pin_whoami(PyObject * _self, PyObject *);
static PyObject * pyhal_pin_set(PyObject *_self, PyObject *value);
static PyObject* pyhal_pin_get(PyObject * _self, PyObject *);
static PyObject * pyhal_pin_get_type(PyObject* _self, PyObject *);
static PyObject* pyhal_pin_get_dir(PyObject * _self, PyObject *);
static PyObject * pyhal_pin_get_name(PyObject * _self, PyObject *);
static PyObject * pyhal_pin_is_pin(PyObject * _self, PyObject *);
struct PyMethodDef halpin_methods[] =
{
	{ "whoami", pin_whoami, METH_NOARGS, "who am i" },
	{ "set", pyhal_pin_set, METH_O, "Set item value" },
	{ "get", pyhal_pin_get, METH_NOARGS, "Get item value" },
	{ "get_type", pyhal_pin_get_type, METH_NOARGS, "Get item type" },
	{ "get_dir", pyhal_pin_get_dir, METH_NOARGS, "Get item direction" },
	{ "get_name", pyhal_pin_get_name, METH_NOARGS, "Get item name" },
	{ "is_pin", pyhal_pin_is_pin, METH_NOARGS, "If item is pin or param" },
	{ NULL },
};
static int pyhalpin_init(PyObject *_self, PyObject *, PyObject *);
//--
static PyTypeObject halpin_type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"hal.item",                /*tp_name*/
	sizeof(halitem),         /*tp_basicsize*/
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

static void pyhal_delete(PyObject *_self);
static PyObject *pyhal_repr(PyObject *_self);

static PyObject * com_whoami(PyObject *self, PyObject *args);
static PyObject *comp_set_prefix(PyObject *_self, PyObject *args);
static PyObject *comp_get_prefix(PyObject *_self, PyObject *args);
static PyObject *pyhal_new_param(PyObject *_self, PyObject *o);
static PyObject *pyhal_exit(PyObject *_self, PyObject *o);
static PyObject *pyhal_ready(PyObject *_self, PyObject *o);
static PyMethodDef hal_methods[] = {
	{ "whoami", com_whoami, METH_NOARGS,"who am i" },
	{ "setprefix", comp_set_prefix, METH_VARARGS,
			"Set the prefix for newly created pins and parameters" },
	{ "getprefix", comp_get_prefix, METH_VARARGS,
			"Get the prefix for newly created pins and parameters" },
	{ "newparam", pyhal_new_param, METH_VARARGS,
			"Create a new parameter" },
	{ "newpin", pyhal_new_param, METH_VARARGS,
			"Create a new pin" },
	{ "getitem", pyhal_new_param, METH_VARARGS,
			"Get existing pin object" },
	{ "exit", pyhal_exit, METH_NOARGS,
			"Call hal_exit" },
	{ "ready", pyhal_ready, METH_NOARGS,
			"Call hal_ready" },
	{ NULL },
};
static int pyhal_init(PyObject *_self, PyObject *args, PyObject *kw);

static PyTypeObject halcomp_type =
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

/************************************************************************
* methods
************************************************************************/

static PyObject * pin_whoami(PyObject *_self, PyObject *args)
{
	return PyString_FromString("fakecnc-hal.pin");
}
static void pyhal_delete(PyObject *_self) {
	halcomp *self = (halcomp *)_self;
	free(self->name);
	self->name = 0;
	free(self->prefix);
	self->prefix = 0;
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
	if (!prefix)
	{
		self->prefix = strdup(prefix);
	}
	else
	{
		self->prefix = 0;
	}
	return 0;
}

static PyObject * com_whoami(PyObject *_self, PyObject *args)
{
	return PyString_FromString("fakecnc-hal.component");
}
static PyObject *comp_set_prefix(PyObject *_self, PyObject *args) {
	char *newprefix;
	halcomp* self = (halcomp*)_self;
	if (!PyArg_ParseTuple(args, "s", &newprefix)) return NULL;

	if (self->prefix)
		free(self->prefix);
	self->prefix = strdup(newprefix);

	if (!self->prefix) {
		PyErr_SetString(PyExc_MemoryError, "strdup(prefix) failed");
		return NULL;
	}
	Py_RETURN_NONE;
}
static PyObject *comp_get_prefix(PyObject *_self, PyObject *args) {
	halcomp* self = (halcomp*)_self;
	if (!PyArg_ParseTuple(args, "")) return NULL;

	if (!self->prefix)
		Py_RETURN_NONE;

	return PyString_FromString(self->prefix);
}
static PyObject *pyhal_new_param(PyObject *_self, PyObject *o) {
	char *name;
	int type, dir;
	halcomp *self = (halcomp *)_self;

	if (!PyArg_ParseTuple(o, "sii", &name, &type, &dir))
		return NULL;
	
	halcomp* pypin = PyObject_New(halcomp, &halpin_type);
	if (name)
		pypin->name = strdup(name);
	else
		pypin->name = NULL;
	return (PyObject *)pypin;
}
static PyObject *pyhal_exit(PyObject *_self, PyObject *o) {
	halcomp *self = (halcomp *)_self;
	if (self->name)
		free(self->name);
	if (self->prefix)
		free(self->prefix);
	Py_RETURN_NONE;
}
static PyObject *pyhal_ready(PyObject *_self, PyObject *o) {
	Py_RETURN_NONE;
}

/************************************************************************/
/* pin section                                                          */
/************************************************************************/

// make a simplest object

//strcut halcomp ,goes up

static PyObject * pyhal_pin_set(PyObject *_self, PyObject *value)
{
	halitem *self = (halitem*)_self;
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
	halitem* self = (halitem*)_self;
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
	halcomp * self = (halcomp *)_self;
	if (!self->name)
		Py_RETURN_NONE;
	return PyString_FromString(self->name);
}

static PyObject * pyhal_pin_is_pin(PyObject * _self, PyObject *) {
	halcomp * self = (halcomp *)_self;
	return PyBool_FromLong(1);
}

static void pyhalpin_delete(PyObject *_self)
{
	halitem *self = (halitem*)_self;
	if (self->name) free(self->name);
	PyObject_Del(self);
}

static PyObject *pyhalpin_repr(PyObject *_self) {
	halitem *pyself = (halitem *)_self;

	const char * name = "(null)";
	if (pyself->name) name = pyself->name;

	return PyString_FromFormat("<hal param \"%s\" FLOAT-RW>", name);
}

static int pyhalpin_init(PyObject *_self, PyObject *, PyObject *) {
	PyErr_Format(PyExc_RuntimeError,
		"Cannot be constructed directly");
	return -1;
}

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
	PyType_Ready(&halpin_type);
	PyModule_AddObject(m, "component", (PyObject*)&halcomp_type);
	PyModule_AddObject(m, "component", (PyObject*)&halpin_type);
}
