
Made by Francisco huelsz prince A01019512

### Compilation
To run make sure the folder `build/` exists.

Inside the folder run `cmake ..` to generate a makefile.

Finally, run `make` to create the executables.

Cmake sets up 3 targets. The difference is only ht implementation of `simulation()`

Using preprocessor definitions only the correct implementation is compiled for each target.

The produces executables are:
1. gol_sync - linear implementation.
2. gol_thr - Threaded implementation with pthread and manual subdivision of tasks.
3. gol_omp - Threaded implementation with OMP.

### Running

Create a folder `frames/` inside `build/`, or wherever the compiled executable is located, so that the program can output the results.

Executing is the same, use:

`./gol_<option> <N # of frames> <source file>`

The program will produce N frames in side `frames/` named `frame_XXXX.png`.

### Extras

A complimentary GIF maker (`gif_maker.py`) is provided that can generate GIF movies from the frames.