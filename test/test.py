
# load the dynamic library (bytecode)
from ctypes import *
cdll.LoadLibrary("./libmlpy.so")
libmlpy = CDLL("./libmlpy.so")
# initialize ocaml runtime
libmlpy.mlpy_startup()
# tell ctypes that callbacks return PyObjects
libmlpy.mlpy_callback1.restype = py_object
libmlpy.mlpy_callback2.restype = py_object
libmlpy.mlpy_callback3.restype = py_object

# call a registered ocaml function with one argument
libmlpy.mlpy_callback1(c_char_p("ocaml_print_x1"), py_object( [1, 2, 3] ))
libmlpy.mlpy_callback1(c_char_p("ocaml_print_x1"), py_object( [True, { 1:2 }, "123"] ))

# call a registered ocaml function with two arguments
libmlpy.mlpy_callback2(c_char_p("ocaml_add_x2"), py_object(1), py_object(10))
libmlpy.mlpy_callback2(c_char_p("ocaml_add_x2"), py_object("1"), py_object("10"))


