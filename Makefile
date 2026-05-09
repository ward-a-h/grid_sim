# compiler we are using
COMPILER = gcc

# -Wall shows all warnings, -pthread links the pthreads library
FLAGS = -Wall -pthread

# all .c files that need to be compiled together
SOURCE_FILES = main.c generators.c consumers.c balance_load.c fault.c metrics.c

# name of the final executable that runs
EXECUTABLE_FILE = grid_sim

# running make compiles everything into one executable
all:
	$(COMPILER) $(FLAGS) $(SOURCE_FILES) -o $(EXECUTABLE_FILE)

# running make clean deletes the compiled program for a fresh build
clean:
	rm -f $(EXECUTABLE_FILE)
