open Mlpy

let ocaml_print pyo =
  let vv = ocamlValue_of_pyObject pyo in
  let b = Buffer.create 10000 in
  let rec iter ov =
    match ov with
        Int n -> Printf.bprintf b "%d" n
      | Bool bool -> Printf.bprintf b "%b" bool
      | String s -> Printf.bprintf b "\"%s\"" (String.escaped s)
      | List l ->
        Printf.bprintf b "[";
        List.iter (fun elem -> iter elem; Printf.bprintf b ", ") l;
        Printf.bprintf b "]";
      | Tuple l ->
        Printf.bprintf b "[";
        Array.iter (fun elem -> iter elem; Printf.bprintf b ", ") l;
        Printf.bprintf b "]";
      | PythonObject _ -> Printf.bprintf b "abstract"
  in
  iter vv;
  let s = Buffer.contents b in
  pyObject_of_ocamlValue (String s)

let _ =
  Callback.register "ocaml_print_x1" ocaml_print

let ocaml_add p1 p2 =
  let pv =
  match pyValue_of_pyObject p1, pyValue_of_pyObject p2 with
      PyInt n1, PyInt n2 -> PyInt (n1 + n2)
    | PyString n1, PyString n2 -> PyString (n1 ^ n2)
    | _ -> PyString "cannot add"
  in
  pyObject_of_pyValue pv

let _ =
  Callback.register "ocaml_add_x2" ocaml_add
