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

// EM_JS(void, print_to_console, (std::string s), {
//   console.log(UTF8ToString(s));
// });

EM_JS(void, copy_string_to_clipboard, (const char* str), {
  try {
    let s = UTF8ToString(str);
    navigator.clipboard.writeText(s);
    console.log(`Copied '${s}' to clipboard!`);
  } catch (err) {
    console.error(`Failed to copy '${s}' to clipboard!`);
  }
}); 