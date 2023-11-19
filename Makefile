include ./Makefile.config

###### ALL PROJECTS ######
project_files = $(notdir $(basename $(wildcard projects/*.cpp)))
all : $(project_files)
$(project_files) : % : projects/%.cpp seahorse/lib/libseahorse.a
	@mkdir -p bin
	@echo ${GREEN}[BUILDING]${NC} Release version of $@...
	@ $(call generate_source,projects/$@)
	@ $(CXX) $< $(STD) $(W_FLAGS) $(OPTIMISE_FLAGS) $(INCLUDE_PATHS) $(USE_FFTW) $(LIBSEAHORSE) -o ./bin/$(notdir $@)
	@ $(call clean_up_source)

debug_files = $(patsubst %, %.debug, $(project_files))
debug: $(debug_files)
$(debug_files) : %.debug : projects/%.cpp seahorse/lib/libseahorsed.a
	@mkdir -p bin
	@echo ${GREEN}[BUILDING]${NC} Debug version of $(basename $@)...
	@ $(call generate_source,projects/$(basename $@))
	@ $(CXX) $< $(STD) $(W_FLAGS) $(DEBUG_FLAGS) $(INCLUDE_PATHS) $(USE_FFTW) $(LIBSEAHORSED) -g -o ./bin/$(notdir $(basename $@))_debug
	@ $(call clean_up_source)

####### GUI VERSION #######
# Graphical Interface - (this is also inserted into the seahorse.app)
gui: bin/gui
bin/gui: gui/gui.cpp Makefile seahorse/lib/libseahorse.a libs/raylib/src/libraylib.a
	@mkdir -p bin
	@echo ${GREEN}[BUILDING]${NC} Graphical Version...
	@ $(CXX) $(FRAMEWORKS) $(STD) $(W_FLAGS) $(OPTIMISE_FLAGS) $(INCLUDE_PATHS) $(LIBRAYLIB) $(LIBSEAHORSED) gui/gui.cpp -o ./bin/gui
	@echo ${GREEN}[RUNNING]${NC}
	$(RUN) ./bin/gui $(ARGS)

####### LIBRARIES #######
# We have this checked out at a specific time so don't need to check for changes
libs/raylib/src/libraylib.a :
	@echo ${GREEN}[BUILDING]${NC} Lib Raylib...
	@ (cd libs/raylib/src && make PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE)

# NB This must have EXACTLY the same architecture/optimisation flags set as the main build
seahorse/lib/libseahorse.a : $(shell find ./seahorse/lib -type f ! -name '*.a') Makefile
	@echo ${GREEN}[BUILDING]${NC} Lib Seahorse...
	@ $(CXX) seahorse/lib/seahorse.cpp $(STD) $(W_FLAGS) $(OPTIMISE_FLAGS) $(INCLUDE_PATHS) -c -o seahorse/lib/libseahorse.o
	@ $(RUN) ar rvs seahorse/lib/libseahorse.a seahorse/lib/libseahorse.o
	@ $(RUN) rm seahorse/lib/libseahorse.o

# The debugging version of the library
seahorse/lib/libseahorsed.a : $(shell find ./seahorse/lib -type f ! -name '*.a') Makefile
	@echo ${GREEN}[BUILDING]${NC} Debug Lib Seahorse...
	@ $(CXX) seahorse/lib/seahorse.cpp $(STD) $(W_FLAGS) $(DEBUG_FLAGS) $(INCLUDE_PATHS) -c -o seahorse/lib/libseahorsed.o
	@ $(RUN) ar rvs seahorse/lib/libseahorsed.a seahorse/lib/libseahorsed.o
	@ $(RUN) rm seahorse/lib/libseahorsed.o

####### CLEAN UP #######
.PHONY: clean
clean:
	@echo ${GREEN}[CLEANING]${NC} ...
	@- rm -r bin && mkdir -p bin
	@- find . -type f -name '*.[oa]' | xargs rm