# A simple makefile for compiling and creating a library file.
#
# On my VAIO laptop, the part:
#	`pkg-config opencv --cflags --libs`
# expands to
#	-I/usr/include/opencv  -lml -lcvaux -lhighgui -lcv -lcxcore
#
# On my MacBook Air, pkg-config opencv --cflags --libs
# expands to
#	-I/usr/include/opencv -lopencv_calib3d -lopencv_contrib
#	--lopencv_core -lopencv_features2d -lopencv_flann
#	--lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml
#	--lopencv_objdetect -lopencv_ocl -lopencv_photo
#	--lopencv_stitching -lopencv_superres -lopencv_ts
#	--lopencv_video -lopencv_videostab
#
# Du Huynh
# Jan-May 2010

.SUFFIXES: .c .cpp .o

HEAD_FILES = debug.h covImage.h
OBJ_FILES = covImage.o debug.o utils.o
LIB_FILE = 

CC = gcc
CXX = g++

# debugging flag
FLAG =

# flags for compilation and linking 
CFLAGS = `pkg-config --cflags opencv` -I$(OPENCV_HOME)/include
LFLAGS = `pkg-config --libs opencv` 

$(LIB_FILE): $(OBJ_FILES)
	ar rvs $(LIB_FILE) $(OBJ_FILES)
	@echo 
	@echo Note: $(LIB_FILE) should be copied manually to an appropriate library directory.

.c.o:
	$(CC) -ggdb -c $< $(CFLAGS) $(CFLAGS_BLOB) -lm

.cpp.o:
	$(CXX) -ggdb -c $< $(CFLAGS) $(CFLAGS_BLOB) -lm


# --------------------------------------------------------------------
# ----- Programs for testing various functions in this directory -----
# --------------------------------------------------------------------

all: Test Test2

Test: Test.cpp $(HEAD_FILES) $(OBJ_FILES)
	$(CXX) -ggdb -o $@ $@.cpp $(OBJ_FILES) $(CFLAGS) $(CFLAGS_BLOB) $(LFLAGS) -lm

Test2: Test2.cpp $(HEAD_FILES) $(OBJ_FILES)
	$(CXX) -ggdb -o $@ $@.cpp $(OBJ_FILES) $(CFLAGS) $(CFLAGS_BLOB) $(LFLAGS) -lm

Test_small: Test_small.cpp $(HEAD_FILES) $(OBJ_FILES)
	$(CXX) -ggdb -o $@ $@.cpp $(OBJ_FILES) $(CFLAGS) $(CFLAGS_BLOB) $(LFLAGS) -lm

Test_modes: Test_modes.cpp $(HEAD_FILES) $(OBJ_FILES)
	$(CXX) -ggdb -o $@ $@.cpp $(OBJ_FILES) $(CFLAGS) $(CFLAGS_BLOB) $(LFLAGS) -lm

clean:
	/bin/rm -f $(OBJ_FILES)
