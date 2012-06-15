
PyObject* mlpy_print_PyObject(PyObject* pyo_v)
{
  static int need_init = 1;
  static value f = Val_unit;

  if( need_init ){
    f = *caml_named_value("print_PyObject");
    need_init = 0;
  }
  return (PyObject*)caml_callback(f, (value)pyo_v);
}
