#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <fstream>
#include <streambuf>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// #define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
// #include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "JS_calls.h"

// TODO:
// [x] Make local and web targets in makefile for easier testing.
// [x] Increase memory size to load larger images.
// [~] Add crypto links. Make it a button so it opens a qr code
// [x] Add other links (ex. github).
// [ ] Add floating spinning guy gif.
// [ ] Add Space Cadet Pinball.
 
// here's how to print a string to conosle: print_to_console(std::to_string(window_width).c_str());


GLFWwindow* g_window;
// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// ImVec4 clear_color = ImVec4(0.75f, 0.58f, 0.80f, 1.00f);
// ImVec4 clear_color = ImVec4(0.675f, 0.918f, 0.651, 1.00f);
auto clear_color = ImVec4(0.98f, 1.00f, 0.75f, 1.00f);

int window_width;
int window_height;
int g_width;
int g_height;

// website view state
bool show_demo_window         = false;
bool show_another_window      = false;
bool show_comfy_image         = true;
bool show_contact_info_window = true;
bool show_crypto_window       = false;
bool show_website_description = true;
bool show_coop_window         = false;
bool show_test_window         = false;
bool show_gif_window          = true;

//common stuff
ImGuiWindowFlags my_simple_window_flags = 0 | ImGuiWindowFlags_NoScrollbar | !ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

// my comfy image
struct custom_image {
  int width; 
  int height; 
  int draw_width;
  int draw_height;
  GLuint texture;
  bool ret;
};
custom_image comfy_image = {0, 0, 1080/4, 1920/4, 0, false};
custom_image github = {0, 0, 20, 20, 0, false};
custom_image email = {0, 0, 20, 20, 0, false};
custom_image home = {0, 0, 20, 20, 0, false};
custom_image linkedin = {0, 0, 20, 20, 0, false};
custom_image journal = {0, 0, 20, 20, 0, false};
custom_image crypto = {0, 0, 20, 20, 0, false};
custom_image bitcoin_qr = {0, 0, 450/2, 450/2, 0, false};
custom_image copy =  {0, 0, 20, 20, 0, false};

//coop term reports
char* summer_2021_report;
char* fall_2020_report;

const char* school_email_address = "jmorea03@uoguelph.ca";
const char* bitcoin_address = "bc1q5lqcyw8dwf0y9k22syjd03p0cmq6d37c5m3h7c";
const char* monero_address  = "45cnUodgCRzcoePUZbuhfRatCe491b8sc9VMuknJY38ueqAqBwhSqw9VCUAzm2ep6n8VsdLW3WXP59SKX2vR478DENtydJb";

// Simple helper to draw text centered in a window.
void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text.c_str());
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

void on_size_changed() {
  glfwSetWindowSize(g_window, g_width, g_height);

  ImGui::SetCurrentContext(ImGui::GetCurrentContext());
}

void loop() {
  window_width = canvas_get_width();
  window_height = canvas_get_height();
  if (window_width != g_width || window_height != g_height)
  {
    g_width = window_width;
    g_height = window_height;
    on_size_changed();
  }

  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();


  if (show_test_window) {
      ImGui::SetNextWindowPos(ImVec2(1000, 100), ImGuiCond_FirstUseEver);
      ImGui::Begin("test", &show_test_window);
      static float f = 0.0f;
      static int counter = 0;
      ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

      ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

      if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
  }

  if (show_another_window) {
    ImGui::Begin("Another Window", &show_another_window);
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me"))
        show_another_window = false;
    ImGui::End();
  }

  if (show_demo_window) {
      ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
      ImGui::ShowDemoWindow(&show_demo_window);
  }

  if (show_website_description) {
    ImGui::SetNextWindowPos(ImVec2(window_width * 1/15, window_height * 1/15), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 400));
    ImGui::Begin("About This Site", NULL, my_simple_window_flags);
    ImGui::TextWrapped(
      "Hello, and welcome to my website. This site is all about myself, so if you aren't interested in me, "
      "then feel free to close this window!"
      "\n\n"
      "I use this site to showcase my work, and write about what i'm up to."
      "\n\n"
      "This site was implemented using imgui (an immediate mode graphics library) and is running in WebAssembly (compiled using emscripten), "
      "so it is unlike traditional js/html websites."
    );

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    ImGui::ColorEdit4("clear color", (float*)&clear_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoDragDrop);
    ImGui::SameLine();
    ImGui::Text("Change the background color!");
    
    if (ImGui::Button("Open Imgui Demo"))
      show_demo_window = !show_demo_window;

    ImGui::Text("Try moving around some windows.");
    ImGui::End();
  }

  if (show_comfy_image) {
    ImGui::SetNextWindowPos(ImVec2(window_width * 1/25, window_height * 1/2), ImGuiCond_FirstUseEver);
    ImGui::Begin("A Pretty Image", NULL, my_simple_window_flags);
    ImGui::Image((void*)(intptr_t)comfy_image.texture, ImVec2(comfy_image.draw_height, comfy_image.draw_width));
    ImGui::End();
  }

  if (show_crypto_window) {
    ImGui::SetNextWindowPos(ImVec2(750, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(900, 500));
    ImGui::Begin("My Crypto Links (give me money)", NULL, my_simple_window_flags);
    auto windowWidth = ImGui::GetWindowSize().x;

    // BITCOIN
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Bitcoin: ");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
    ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(bitcoin_address).x) * 0.5f);
    if (ImGui::Button(bitcoin_address)) copy_string_to_clipboard(bitcoin_address);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Copy address to clipboard");
    ImGui::PopStyleColor(3);
    ImGui::SetCursorPosX((windowWidth - bitcoin_qr.draw_width) * 0.5f);
    ImGui::Image((void*)(intptr_t)bitcoin_qr.texture, ImVec2(bitcoin_qr.draw_height, bitcoin_qr.draw_width));
    
    ImGui::Separator();

    //MONERO
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Monero: ");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(monero_address).x) * 0.5f);
    if (ImGui::Button(monero_address)) copy_string_to_clipboard(bitcoin_address);
    ImGui::PopStyleColor(3);
    ImGui::SameLine();

    ImGui::End();
  }

  if (show_contact_info_window) {
    ImGui::SetNextWindowPos(ImVec2(window_width * 8/10, window_height * 1/15), ImGuiCond_FirstUseEver);
    // ImGui::SetNextWindowSize(ImVec2(500, 500));
    ImGui::Begin("My Other Info", NULL, my_simple_window_flags);

    ImGui::Image((void*)(intptr_t)email.texture, ImVec2(email.draw_height, email.draw_width));
    ImGui::SameLine();
    if (ImGui::Button(school_email_address)) copy_string_to_clipboard(school_email_address);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Copy adress to clipboard");
    
    ImGui::Image((void*)(intptr_t)github.texture, ImVec2(github.draw_height, github.draw_width));
    ImGui::SameLine();
    if (ImGui::Button("Github"))
      open_url("https://github.com/JamesMoreau");

    // ImGui::Dummy(ImVec2(20, 20));
    ImGui::Image((void*)(intptr_t)home.texture, ImVec2(home.draw_height, home.draw_width));
    ImGui::SameLine();
    if (ImGui::Button("My Website"))
      open_url("http://localhost:8000/imgui.html");

    ImGui::Image((void*)(intptr_t)linkedin.texture, ImVec2(linkedin.draw_height, linkedin.draw_width));
    ImGui::SameLine();
    if (ImGui::Button("LinkedIn"))
      open_url("https://www.linkedin.com/in/james-moreau/");

    ImGui::Image((void*)(intptr_t)journal.texture, ImVec2(journal.draw_height, journal.draw_width));
    ImGui::SameLine();
    if (ImGui::Button("Co-op work terms")) 
      show_coop_window = !show_coop_window;

    ImGui::Image((void*)(intptr_t)crypto.texture, ImVec2(crypto.draw_height, crypto.draw_width));
    ImGui::SameLine();
    if (ImGui::Button("Crypto"))
      show_crypto_window = !show_crypto_window;

    ImGui::End();
  }

  if (show_coop_window) {
    ImGui::SetNextWindowPos(ImVec2(window_width * 1/3, window_height * 1/15), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(1010, 900), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

    ImGui::Begin("Co-op Experience", &show_coop_window, ImGuiWindowFlags_NoCollapse);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
    
      if (ImGui::BeginTabItem("Fall 2020 Co-op work term report")) {
          ImGui::TextUnformatted(fall_2020_report);
          ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Summer 2021 Co-op Work Term Report")) {
          ImGui::TextUnformatted(summer_2021_report);
          ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }

    ImGui::End();
  }

  ImGui::Render();

  int display_w, display_h;
  glfwMakeContextCurrent(g_window);
  glfwGetFramebufferSize(g_window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwMakeContextCurrent(g_window);
}

int init_gl() {
  if (!glfwInit()) {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      return 1;
  }

  //glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  // Open a window and create its OpenGL context
  int canvasWidth = 800;
  int canvasHeight = 600;
  g_window = glfwCreateWindow(canvasWidth, canvasHeight, "WebGui Demo", NULL, NULL);
  if (!g_window) {
    fprintf( stderr, "Failed to open GLFW window.\n" );
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(g_window); // Initialize GLEW

  return 0;
}

int init_imgui() {
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(g_window, true);
  ImGui_ImplOpenGL3_Init();

  // Setup style
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("data/fonts/Ruda/static/Ruda-SemiBold.ttf", 20.0f);
  ImGui::GetStyle().FrameRounding = 4.0f;
  ImGui::GetStyle().GrabRounding = 4.0f;

  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
  colors[ImGuiCol_TextDisabled]          = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
  colors[ImGuiCol_WindowBg]              = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
  colors[ImGuiCol_ChildBg]               = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
  colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  colors[ImGuiCol_Border]                = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
  colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
  colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
  colors[ImGuiCol_FrameBgActive]         = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
  colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
  colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg]             = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
  colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
  colors[ImGuiCol_CheckMark]             = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
  colors[ImGuiCol_SliderGrab]            = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
  colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
  colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
  colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
  colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
  colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
  colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
  colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
  colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
  colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  colors[ImGuiCol_Tab]                   = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
  colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
  colors[ImGuiCol_TabUnfocused]          = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
  colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
  resizeCanvas();

  return 0;
}

void load_work_term_reports() {
  FILE* f;
  long fsize;

  f = fopen("data/coop_term_reports/fall_2020_report.txt", "r+");
  fseek(f, 0, SEEK_END);
  fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  fall_2020_report = (char*) malloc(fsize + 1);
  fread(fall_2020_report, fsize, 1, f);
  fclose(f);
  fall_2020_report[fsize] = 0;

  f = fopen("data/coop_term_reports/summer_2021_report.txt", "r+");
  fseek(f, 0, SEEK_END);
  fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  summer_2021_report = (char*) malloc(fsize + 1);
  fread(summer_2021_report, fsize, 1, f);
  fclose(f);
  summer_2021_report[fsize] = 0;
}

void load_custom_images() {
  comfy_image.ret = LoadTextureFromFile("data/comfy.jpg",      &comfy_image.texture, &comfy_image.width, &comfy_image.height);
  github.ret      = LoadTextureFromFile("data/github.png",     &github.texture,      &github.width,      &github.height);
  email.ret       = LoadTextureFromFile("data/email.png",      &email.texture,       &email.width,       &email.height);
  home.ret        = LoadTextureFromFile("data/home.png",       &home.texture,        &home.width,        &home.height);
  linkedin.ret    = LoadTextureFromFile("data/linkedin.png",   &linkedin.texture,    &linkedin.width,    &linkedin.height);
  journal.ret     = LoadTextureFromFile("data/journal.png",    &journal.texture,     &journal.width,     &journal.height);
  crypto.ret      = LoadTextureFromFile("data/crypto.png",     &crypto.texture,      &crypto.width,      &crypto.height);
  bitcoin_qr.ret  = LoadTextureFromFile("data/bitcoin_qr.jpg", &bitcoin_qr.texture,  &bitcoin_qr.width,  &bitcoin_qr.height);
  copy.ret        = LoadTextureFromFile("data/copy.png",       &copy.texture,        &copy.width,        &copy.height);
}

int init() {
  init_gl();
  init_imgui();
  load_custom_images();
  load_work_term_reports();
  return 0;
}

void quit() {
  glfwTerminate();
}

extern "C" int main(int argc, char** argv) {
  if (init() != 0) return 1;

  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
  #endif

  quit();

  return 0;
}
