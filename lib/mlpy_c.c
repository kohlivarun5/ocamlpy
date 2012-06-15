/*
   This file is part of ocamlpy, an interface to call OCaml code from
   Python. Written by OCamlPro SAS. Contact us for any question at
   contact@ocamlpro.com.

   ocamlpy is distributed under the terms of the LGPL v3, as described in
   the file COPYING.GPLv3.txt.
 */
#include <string.h>

#include <python2.7/Python.h>
#include <python2.7/boolobject.h>
#include <python2.7/intobject.h>
#include <python2.7/listobject.h>
#include <python2.7/tupleobject.h>
#include <python2.7/stringobject.h>

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/custom.h>
#include <caml/callback.h>

#define ML_INT_OBJECT 0
#define ML_BOOL_OBJECT 1
#define ML_LIST_OBJECT 2
#define ML_TUPLE_OBJECT 3
#define ML_STRING_OBJECT 4
#define ML_ANY_OBJECT 5

value mlpy_type_objects(value array_v)
{
  PyTypeObject* array = (PyTypeObject*) array_v;
  array[ML_INT_OBJECT] = PyInt_Type;
  array[ML_BOOL_OBJECT] = PyBool_Type;
  array[ML_LIST_OBJECT] = PyList_Type;
  array[ML_TUPLE_OBJECT] = PyTuple_Type;
  array[ML_STRING_OBJECT] = PyString_Type;

  return Val_unit;
}

void mlpy_finalize_pyObject(value pyo_v){
  PyObject *pyo = (PyObject*) pyo_v;
  Py_DECREF(pyo);
}

struct custom_operations mlpy_ops = {
  "_py",
  mlpy_finalize_pyObject,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default
};

struct custom_operations mlpy_ops;
void mlpy_init(void)
{
  static int need_init = 1;
  if (need_init){
    need_init = 0;
    caml_register_custom_operations(&mlpy_ops);
  }
}

value mlpy_pyObjectBoxed_of_pyObject(value pyo_v)
{
  PyObject *pyo = (PyObject*)pyo_v;
  value res_v;

  Py_INCREF(pyo);
  res_v = caml_alloc_custom(&mlpy_ops, sizeof(value), 1, 1000);
  Field(res_v, 1) = pyo_v;
  return res_v;
}

value mlpy_pyValue_of_pyObject(value pyo_v)
{
  CAMLparam0 (); /* pyo_v is in the Python heap */
  CAMLlocal1(tmp_v);
  value res_v;
  int tag = 0;

  mlpy_init();
  PyObject *pyo = (PyObject*) pyo_v;
  if(PyBool_Check(pyo)) { /* before PyInt_Check, because bool is a subtype of int */
    tag = ML_BOOL_OBJECT;
    if( ((PyBoolObject *)pyo)->ob_ival )
      tmp_v = Val_true;
    else
      tmp_v = Val_false;
  } else
    if(PyInt_Check(pyo)) {
      tag = ML_INT_OBJECT;
      tmp_v = Val_long(PyInt_AS_LONG(pyo));
    } else
      if(PyList_Check(pyo)) {
          int i;
          int len = PyList_Size(pyo);

          tag = ML_LIST_OBJECT;
          tmp_v = caml_alloc(len, 0);
          for(i = 0; i < len; i++) Field(tmp_v, i) = Val_unit;
          for(i = 0; i < len; i++){
            PyObject *elem = PyList_GetItem(pyo, i);
            value elem_v = mlpy_pyObjectBoxed_of_pyObject((value) elem);
            caml_initialize(&Field(tmp_v, i), elem_v);
          }
      } else
        if(PyTuple_Check(pyo)) {
          int i;
          int len = PyTuple_Size(pyo);

          tag = ML_TUPLE_OBJECT;
          tmp_v = caml_alloc(len, 0);
          for(i = 0; i < len; i++) Field(tmp_v, i) = Val_unit;
          for(i = 0; i < len; i++){
            PyObject *elem = PyTuple_GetItem(pyo, i);
            value elem_v = mlpy_pyObjectBoxed_of_pyObject((value) elem);
            caml_initialize(&Field(tmp_v, i), elem_v);
          }
        } else
          if(PyString_Check(pyo)) {
            int len = PyString_Size(pyo);
            char * s = PyString_AsString(pyo);

            tag = ML_STRING_OBJECT;
            tmp_v = caml_alloc_string(len);
            memmove(String_val(tmp_v), s, len);
          } else {
            tag = ML_ANY_OBJECT;
            fprintf(stderr, "Unknown basic type [%s]\n", (char *)((Py_TYPE(pyo))->tp_name));
            tmp_v = mlpy_pyObjectBoxed_of_pyObject((value) pyo);
          }

  res_v = caml_alloc_small(1, tag);
  Field(res_v, 0) = tmp_v;
  CAMLreturn(res_v);
}

value mlpy_pyObject_of_pyValue(value pyv_v)
{
  int tag = Tag_val(pyv_v);
  mlpy_init();

  switch(tag){
  case ML_INT_OBJECT : {
    value arg_v = Field(pyv_v, 0);
    PyObject *pyo = PyInt_FromLong(Long_val(arg_v));
    return (value)pyo;
  }
  case ML_BOOL_OBJECT : {
    value arg_v = Field(pyv_v, 0);
    PyObject *pyo = PyBool_FromLong(Bool_val(arg_v));
    return (value)pyo;
  }
  case ML_STRING_OBJECT : {
    value arg_v = Field(pyv_v,0);
    int size = caml_string_length(arg_v);
    PyObject *pyo = PyString_FromStringAndSize(String_val(arg_v), size);
    return (value)pyo;
  }
  case ML_LIST_OBJECT : {
    int i;
    value arg_v = Field(Field(pyv_v,0), 1);
    int size = Wosize_val(arg_v);
    PyObject *pyo = PyList_New(size);
    for(i=0; i < size; i++)
      PyList_SetItem(pyo, i, (PyObject*)Field(arg_v, i));
    return (value)pyo;
  }
  case ML_TUPLE_OBJECT : {
    int i;
    value arg_v = Field(Field(pyv_v,0), 1);
    int size = Wosize_val(arg_v);
    PyObject *pyo = PyTuple_New(size);
    for(i=0; i < size; i++)
      PyTuple_SetItem(pyo, i, (PyObject*)Field(arg_v, i));
    return (value)pyo;
  }
  case ML_ANY_OBJECT : {
    value arg_v = Field(Field(pyv_v,0), 1);
    return arg_v;
  }
  default: exit(2);
  }
}

value mlpy_pyObject_decref(value pyo_v)
{
  PyObject *pyo = (PyObject*) pyo_v;
  Py_INCREF(pyo);
  return Val_unit;
}

value mlpy_pyObject_incref(value pyo_v)
{
  PyObject *pyo = (PyObject*) pyo_v;
  Py_DECREF(pyo);
  return Val_unit;
}

PyObject* mlpy_startup(void)
{
  char *argv[2];
  argv[0] = "python";
  argv[1] = NULL;
  caml_startup(argv);
  return Py_False;
}

PyObject* mlpy_callback1(char *name, PyObject* pyo_v)
{
  value f = *caml_named_value(name);
  return (PyObject*)caml_callback(f, (value)pyo_v);
}

PyObject* mlpy_callback2(char *name, PyObject* pyo1_v, PyObject* pyo2_v)
{
  value f = *caml_named_value(name);
  return (PyObject*)caml_callback2(f, (value)pyo1_v, (value)pyo2_v);
}

PyObject* mlpy_callback3(char *name, PyObject* pyo1_v, PyObject* pyo2_v, PyObject* pyo3_v)
{
  value f = *caml_named_value(name);
  return (PyObject*)caml_callback3(f, (value)pyo1_v, (value)pyo2_v, (value)pyo3_v);
}

