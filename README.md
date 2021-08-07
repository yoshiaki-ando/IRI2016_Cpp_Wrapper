# IRI2016 C++ Wrapper by AndoLab

## Install
1. download IRI files (00_iri.tar) from [IRI site](http://irimodel.org/IRI-2016/),
and CCIR and URSI files (00_ccir-ursi.tar) from [COMMON_FILES in IRI site](http://irimodel.org/COMMON_FILES/), and the solar and magnetic indices files [apf107.dat](https://chain-new.chain-project.net/echaim_downloads/apf107.dat) and [ig_rz.dat](https://chain-new.chain-project.net/echaim_downloads/ig_rz.dat).
Then, extract tar files in the same directory of the sources.

2. patch
```Tcsh
% patch -p1 < iri2016_cpp.patch
```

3. make ; make install
<br>
Edit Makefile if necessary, especially PREFIX

4. make test_main
```Tcsh
% ./test_main
```


## Example of Usage

* namespace: AndoLab

```C++:exmple
#include <iri2016.h>

AndoLab::iri2016 iri;
AndoLab::iri.set_coord(35.0f, 142.0f); /* geographic coordinate */
AndoLab::iri.set_datetime(2015, 10, 1, 12, 0); /* UT */
AndoLab::iri.set_height(80.0f, 400.0f, 1.0f);

float *Ne = new float [320];
AndoLab::iri.get_Ne(Ne);
```

## TODO
* implement options

## Version
0.0.1 (August 8, 2021): Initial commit

## Resources
[IRI Official](http://irimodel.org/)
