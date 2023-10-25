# SeaHorse
Quantum Simulation and Optimal Control



Building:

	- Eigen:

	- Spectra:
		Straight from github

	- Raylib (For GUI related code):

		Run commands to get and build raylib with raygui:

			`git clone https://github.com/raysan5/raylib`
			`git clone https://github.com/raysan5/raygui`
			`cd raylib/src`
			`make PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE`

		Now move these files to SeaHorse/libs/raylib
			raylib/src/raylib.h
			raylib/src/libraylib.a
			raygui/src/raygui.h