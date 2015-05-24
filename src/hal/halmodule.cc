#include <Python.h>
#include <string.h>
#include <map>
using namespace std;

PyObject *pyhal_error_type = NULL;

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

static PyTypeObject halpin_type = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"hal.item",		/* tp_name        */
	sizeof(SimpleObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	0,				/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	0,				/* tp_repr        */
	0,				/* tp_as_number   */
	0,				/* tp_as_sequence */
	0,				/* tp_as_mapping  */
	0,				/* tp_hash        */
	0,				/* tp_call        */
	0,				/* tp_str         */
	0,				/* tp_getattro    */
	0,				/* tp_setattro    */
	0,				/* tp_as_buffer   */
	Py_TPFLAGS_DEFAULT,		/* tp_flags       */
	"Simple objects are simple.",	/* tp_doc         */
};

extern "C"
void init_hal(void)
{
	PyObject* m = Py_InitModule3("_hal", module_methods, module_doc);
	pyhal_error_type = PyErr_NewException((char*)"hal.error", NULL, NULL);
	PyModule_AddObject(m, "error", pyhal_error_type);
	PyType_Ready(&halpin_type);
	PyModule_AddObject(m, "item", (PyObject*)&halpin_type);
}
