# SeaHorse
Quantum Simulation and Optimal Control.

## Included libraries

<b>These are included as submodules. </b>
* <ins>Eigen:</ins>
  
	Used for fast linear algebra calculations.

* <ins>Spectra:</ins>

	Used for generating eigenvectors.
	This is substantially faster than Eigen's default implementation as it allows us to request only a specific number instead of the full specturm.

* <ins>Raylib + Raygui:</ins>

	Used for the graphical interface.
	This is only included for gui.cpp not main.cpp

## Installing
On first run you need to initialise and update the submodules:

`git submodule init`

`git submodule update`

## Building
We use the make build system (with a little bit of cmake behind the scenes). This is designed to separate the build of user programs and the seahorse library, which massively decreases compile times.

Any *.cpp in the projects file can be built using:

`make *` 

which links the static library. For debugging 

`make *.debug`

Which will include seahorse to enhance debugging.

Both will create an executable in /bin.
