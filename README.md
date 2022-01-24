# IRI2016 C++ Wrapper by AndoLab

## Install
0. install gfortran. `sudo apt install gfortran`

1. download IRI files (00_iri.tar) from [IRI site](http://irimodel.org/IRI-2016/),
and CCIR and URSI files (00_ccir-ursi.tar) from [COMMON_FILES in IRI site](http://irimodel.org/COMMON_FILES/), and the solar and magnetic indices files [apf107.dat](https://chain-new.chain-project.net/echaim_downloads/apf107.dat) and [ig_rz.dat](https://chain-new.chain-project.net/echaim_downloads/ig_rz.dat).
Then, extract tar files in the same directory of the sources, i.e. `tar xvf 00_ccir-ursi.tar`, etc.

2. patch
```Tcsh
% patch -p1 < iri2016_cpp.patch
```

3. Edit Makefile if necessary, especially PREFIX, and then type `make ; make install`


4. make test_main
```Tcsh
% ./test_main
```


## Example of Usage

* namespace: AndoLab

```C++:exmple
#include <iri2016.h>

AndoLab::iri2016 iri;
iri.set_coord(35.0f, 142.0f); /* geographic coordinate */
iri.set_datetime(2015, 10, 1, 12, 0); /* UT */
iri.set_height(80.0f, 400.0f, 1.0f);

float *Ne = new float [320];
iri.get_Ne(Ne);
```

## Original Model
IRI does not provide Ne at the altitude about 60km, which is important for VLF/LF propagation.
To overcome the problem, we create an original model. It use the Danilov model up to the altitude
where the IRI provides Ne.

### usage
```C++:exmple
#include <iri2016.h>

AndoLab::iri2016 iri;
iri.set_coord(35.0f, 142.0f); /* geographic coordinate */
iri.set_datetime(2015, 10, 1, 12, 0); /* UT */

constexpr float Alt_min { 60.0f };  /* in km */
constexpr float Alt_max { 110.0f }; /* in km */
constexpr float Alt_stp { 1.0f };   /* in km */
constexpr int N int( (Alt_max - Alt_min)/Alt_stp ) + 1; /* Points */
float *Ne = new float [ N ];
original_model(N, Alt_min, Alt_stp, iri, Ne);
```

## TODO
* implement options

## Version
0.0.1 (August 8, 2021): Initial commit

## Resources
[IRI Official](http://irimodel.org/)
