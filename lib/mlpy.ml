(*
   This file is part of ocamlpy, an interface to call OCaml code from
   Python. Written by OCamlPro SAS. Contact us for any question at
   contact@ocamlpro.com.

   ocamlpy is distributed under the terms of the LGPL v3, as described in
   the file COPYING.GPLv3.txt.
 *)

type pyObject
type pyTypeObject

type pyCustomOperations
type pyObjectBoxed = {
  py_ops : pyCustomOperations;
  pyObject : pyObject;
}

type pyValue =
  | PyInt of int
  | PyBool of bool
  | PyList of pyObjectBoxed array
  | PyTuple of pyObjectBoxed array
  | PyString of string
  | PyObject of pyObjectBoxed

external pyObjectBoxed_of_pyObject : pyObject -> pyObjectBoxed =
    "mlpy_pyObjectBoxed_of_pyObject"

external pyValue_of_pyObject : pyObject -> pyValue = "mlpy_pyValue_of_pyObject"
external pyObject_of_pyValue : pyValue -> pyObject = "mlpy_pyObject_of_pyValue"

external pyObject_decref : pyObject -> unit = "mlpy_pyObject_decref"
external pyObject_incref : pyObject -> unit = "mlpy_pyObject_incref"

let pyObject_of_pyObjectBoxed pyb = pyb.pyObject

(************************************************************************)
(*                                                                      *)
(*                                                                      *)
(*                                                                      *)
(************************************************************************)

type ocamlValue =
  | Int of int
  | Bool of bool
  | List of ocamlValue list
  | Tuple of ocamlValue array
  | String of string
  | PythonObject of pyObjectBoxed

let rec of_pyObject pyo =
  let pyv = pyValue_of_pyObject pyo in
  match pyv with
    | PyInt n -> Int n
    | PyBool b -> Bool b
    | PyString s -> String s
    | PyTuple array ->
      Tuple (Array.map of_pyObjectBoxed array)
    | PyList array ->
      List (Array.to_list (Array.map  of_pyObjectBoxed array))
    | PyObject pyb -> PythonObject pyb

and of_pyObjectBoxed pyb = of_pyObject pyb.pyObject

let rec to_pyObject v =
  let pyv =
    match v with
        Int n -> PyInt n
      | Bool b -> PyBool b
      | String s -> PyString s
      | Tuple array -> PyTuple (Array.map to_pyObjectBoxed array)
      | List list ->
        PyList (Array.map to_pyObjectBoxed  (Array.of_list list))
      | PythonObject pyb -> PyObject pyb
  in
  pyObject_of_pyValue pyv

and to_pyObjectBoxed v =
  pyObjectBoxed_of_pyObject (to_pyObject v)

let ocamlValue_of_pyObject = of_pyObject
let pyObject_of_ocamlValue = to_pyObject

let register_callback1 name ( f : pyObject -> pyObject ) =
  Callback.register name f

let register_callback2 name ( f : pyObject -> pyObject -> pyObject ) =
  Callback.register name f

let register_callback3 name ( f : pyObject -> pyObject -> pyObject -> pyObject ) =
  Callback.register name f
