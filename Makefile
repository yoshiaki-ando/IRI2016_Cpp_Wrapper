PREFIX = $(HOME)
LIB_DIR = $(PREFIX)/lib
INC_DIR = $(PREFIX)/include
DATA_DIR = $(PREFIX)/share/iri2016

OPTS = -O3 -Wall -fPIC
TEST_OPTS = -O3 -Wall -fPIC

LIB_SUFFIX = _18
#LIB_SUFFIX = _20
TARGET = libiri2016$(LIB_SUFFIX).so

FORTRAN_OBJS = cira.o igrf.o iridreg.o iriflip.o irifun.o irirtam.o irisub.o iritec.o
OBJS = iri2016.o original_model.o

.PHONY: all install clean test

all: $(TARGET)

libiri2016$(LIB_SUFFIX).so: $(FORTRAN_OBJS) $(OBJS)
	g++ -shared -o $@ $(FORTRAN_OBJS) $(OBJS)

%.o: %.for
	gfortran -c $< $(OPTS) -cpp -DDATA_DIR=\'$(DATA_DIR)\'

%.o: %.cpp iri2016.h
	g++ -c $< $(OPTS)

test: test_main

test_main.o: test_main.cpp
	g++ -c $< $(TEST_OPTS) -I$(INC_DIR)

test_main: test_main.o
	g++ -o $@ test_main.o -Wl,-R$(LIB_DIR) -L$(LIB_DIR) -liri2016$(LIB_SUFFIX) -lgfortran

install: $(TARGET)
	@if [ ! -d $(LIB_DIR) ]; then \
	  mkdir -p $(LIB_DIR); \
	fi
	cp $(TARGET) $(LIB_DIR)
	@if [ ! -d $(INC_DIR) ]; then \
	  mkdir -p $(INC_DIR); \
	fi
	cp iri2016.h $(INC_DIR)
	@if [ ! -d $(DATA_DIR) ]; then \
    mkdir -p $(DATA_DIR); \
  fi
	cp apf107.dat dgrf*.dat ig_rz.dat igrf*.dat mcsat*.dat *.asc $(DATA_DIR)

clean:
	rm -rf $(TARGET) *.o test_main
