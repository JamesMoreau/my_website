#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

// custom js 
EM_JS(int, canvas_get_width, (), {
  return Module.canvas.width;
});

EM_JS(int, canvas_get_height, (), {
  return Module.canvas.height;
});

EM_JS(void, resizeCanvas, (), {
  js_resizeCanvas();
});

EM_JS(void, open_url, (const char* link), {
  window.open(UTF8ToString(link));
})

EM_JS(void, print_to_console, (const char* str), {
  console.log(UTF8ToString(str));
});