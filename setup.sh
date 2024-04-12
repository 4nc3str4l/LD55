
# Check if emcc is installed
if ! command -v emcc &> /dev/null
then
    echo "emcc could not be found"
    echo "Please install emscripten and add it to your PATH"
    exit
fi

# Check that gcc is installed
if ! command -v gcc &> /dev/null
then
    echo "gcc could not be found"
    echo "Please install gcc"
    exit
fi

# Check that make is installed
if ! command -v make &> /dev/null
then
    echo "make could not be found"
    echo "Please install make"
    exit
fi

# Check that cmake is installed
if ! command -v cmake &> /dev/null
then
    echo "cmake could not be found"
    echo "Please install cmake"
    exit
fi

#go to dependencies/raylib and create a build directory
cd dependencies/raylib
mkdir build

#go to the build directory and run cmake
cd build
cmake -DBUILD_SHARED_LIBS=ON ..

#run make
make

# Also build for web
cd ../src
emcc -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c utils.c -Os -Wall -DPLATFORM_WEB
emcc -c raudio.c -Os -Wall -DPLATFORM_WEB

emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o

