# SeaHorse
Quantum Simulation and Optimal Control.

## Included libraries:

### - Eigen:
	* * INCLUDED AS SUBMODULE * *

	Used for fast linear algebra calculations.

### - Spectra:
	* * INCLUDED AS SUBMODULE * *

	Used for generating eigenvectors.
	This is substantially faster than Eigen's default implementation as it allows us to request only a specific number instead of the full specturm.

### - Raylib + Raygui (For GUI related code):
	* * INCLUDED AS SUBMODULE * *

	Used for the graphical interface. This is only used for gui.cpp not main.cpp

## Building:

On first run you need to initialise and update the submodules:

`git init submodules`
`git update submodules`

We use the make build system:
`make`