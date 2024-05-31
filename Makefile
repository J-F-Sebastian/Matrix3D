.PHONY: all clean

OBJDIR := build
CXXFLAGS += -I E:\sviluppo\SDL2-2.0.22\i686-w64-mingw32\include\SDL2 -Wall -Wextra -O2 -g3 -march=core2 -msse3 -std=c++11
CFLAGS += -Wall -Wextra -O2 -g -march=core2
CC = gcc

MATRIX3D = m3d_display.o m3d_world.o m3d_vertex.o m3d_renderer.o m3d_object.o m3d_math.o m3d_light_source.o m3d_color.o m3d_camera.o main.o

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(addprefix $(OBJDIR)/, $(MATRIX3D))
	$(CXX) -L E:\sviluppo\SDL2-2.0.22\i686-w64-mingw32\lib -o matrix3d.exe $^ -lmingw32 -lSDL2main -lSDL2
	$(CXX) -L E:\sviluppo\SDL2-2.0.22\i686-w64-mingw32\lib -s -o matrix3d_release.exe $^ -lmingw32 -lSDL2main -lSDL2

clean:
	del matrix3d*.exe
	del gensphere.exe
	del gensphere.o
	del /F /S /Q $(OBJDIR)

gensphere: gensphere.o
	gcc -s -o gensphere.exe gensphere.o
