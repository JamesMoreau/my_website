CXX        = emcc
OUTPUT     = imgui.js
LIB_DIR   := lib
IMGUI_DIR := lib/imgui
STB_DIR   := lib/stb
BIN       := bin

SOURCES =  main.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/imgui_tables.cpp

LIBS = -lGL
WEBGL_VER = -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1
#WEBGL_VER = USE_GLFW=2
USE_WASM = -s WASM=1

debug: $(SOURCES)
	emcc $(SOURCES) -std=c++11 -o $(OUTPUT) $(LIBS) $(WEBGL_VER) $(USE_WASM) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(STB_DIR) -O0 -g --memoryprofiler --preload-file data

release: $(SOURCES)
	emcc $(SOURCES) -std=c++11 -o $(OUTPUT) $(LIBS) $(WEBGL_VER) $(USE_WASM) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(STB_DIR) -O3 -Oz -flto --preload-file data

run: 
	# open -a "Google Chrome" http://localhost:8000/imgui.html
	python -m SimpleHTTPServer 8000
	
clean:
	rm -f $(OUTPUT)
	rm -f imgui.wasm
	rm -f imgui.data

# all: $(SOURCES) $(OUTPUT)

# $(OUTPUT): $(SOURCES) 
# 	$(CXX) $(SOURCES) -std=c++11 -o $(OUTPUT) $(LIBS) $(WEBGL_VER) --profiling -O1 --preload-file data $(USE_WASM) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(STB_DIR)

# debug:
# 	cc `pkg-config --cflags glfw3` $(SOURCES) -std=c++11 -o me $(LIBS) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(STB_DIR) `pkg-config --static --libs glfw3`
