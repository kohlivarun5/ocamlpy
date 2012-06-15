(*
   This file is part of ocamlpy, an interface to call OCaml code from
   Python. Written by OCamlPro SAS. Contact us for any question at
   contact@ocamlpro.com.

   ocamlpy is distributed under the terms of the LGPL v3, as described in
   the file COPYING.GPLv3.txt.
 *)

(* The type of an object directly received from Python *)
(* When an OCaml function is called from Python, all objects
   have a refcount increased by 1 during the execution of the
   OCaml function. *)
type pyObject

(* The type of a Python object that we store in an OCaml value. *)
(* The object has a refcount increased by one as long as the OCaml
   value exists, and is automatically decreased when the OCaml object
   is collected. *)
type pyObjectBoxed

(* The type of a value where the first level of Python values has been
  converted to an OCaml value. *)
type pyValue =
  | PyInt of int
  | PyBool of bool
  | PyList of pyObjectBoxed array
  | PyTuple of pyObjectBoxed array
  | PyString of string
  | PyObject of pyObjectBoxed

(* These functions implement conversions between [pyObject],
   [pyObjectBoxed] and [pyValue]. *)
val pyValue_of_pyObject : pyObject -> pyValue
val pyObject_of_pyValue : pyValue -> pyObject

val pyObject_of_pyObjectBoxed : pyObjectBoxed -> pyObject
val pyObjectBoxed_of_pyObject : pyObject -> pyObjectBoxed

val pyObject_decref : pyObject -> unit
val pyObject_incref : pyObject -> unit

val register_callback1 : string -> (pyObject -> pyObject) -> unit
val register_callback2 : string -> (pyObject -> pyObject -> pyObject) -> unit
val register_callback3 : string -> (pyObject -> pyObject -> pyObject -> pyObject) -> unit

(************************************************************************)
(*                                                                      *)
(*                                                                      *)
(*                                                                      *)
(************************************************************************)

(* This section implements a conversion between an almost completely
OCaml value, and PYTHON objects *)

type ocamlValue =
  | Int of int
  | Bool of bool
  | List of ocamlValue list
  | Tuple of ocamlValue array
  | String of string
  | PythonObject of pyObjectBoxed

val ocamlValue_of_pyObject : pyObject -> ocamlValue
val pyObject_of_ocamlValue : ocamlValue -> pyObject

