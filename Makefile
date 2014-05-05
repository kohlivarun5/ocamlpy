OCAMLLIB=`ocamlc -where`

OCAML_INCLUDE=$(OCAMLLIB)

LIB_SOURCES_C=lib/mlpy_c.c
LIB_SOURCES_MLI= lib/mlpy.mli
LIB_SOURCES_ML= lib/mlpy.ml
LIB_SOURCES_O=$(LIB_SOURCES_C:.c=.o)

TEST_SOURCES_ML=test/test.ml

all: libmlpy.so

libmlpy.so:  Makefile $(LIB_SOURCES_O)
	ocamlc -c -I lib $(LIB_SOURCES_MLI) $(LIB_SOURCES_ML)
	ocamlc -c -I lib $(TEST_SOURCES_ML)
	ocamlc -output-obj -o libmlpy.o $(LIB_SOURCES_ML:.ml=.cmo) $(TEST_SOURCES_ML:.ml=.cmo)
	gcc -v -g -shared -o 'libmlpy.so' 'libmlpy.o' $(LIB_SOURCES_O) $(OCAMLLIB)/libcamlrun_shared.so -lm  -ldl -lcurses -lpthread -I$(OCAML_INCLUDE) 

clean:
	rm -f *~ core *.o *.so *.cm? lib/*~ lib/*.o lib/*.cm? test/*~ test/*.o test/*.cm?

.c.o:
	gcc -v -g -Wall -c -fPIC -o $*.o $*.c -I$(OCAML_INCLUDE) 

