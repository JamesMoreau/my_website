#include <stdio.h>
#include <stdlib.h>

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
// [ ] Add crypto links. Make it a button so it opens a 
// [ ] Add other links (ex. github).
// [ ] Add floating spinning guy.


GLFWwindow* g_window;
// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// ImVec4 clear_color = ImVec4(0.75f, 0.58f, 0.80f, 1.00f);
// ImVec4 clear_color = ImVec4(0.675f, 0.918f, 0.651, 1.00f);
auto clear_color = ImVec4(0.98f, 1.00f, 0.75f, 1.00f);
int g_width;
int g_height;

// website view state
bool show_demo_window         = false;
bool show_another_window      = false;
bool show_comfy_image         = true;
bool show_contact_info_window = true;
bool show_crypto_window       = false;
bool show_website_description = true;
bool show_coop_window         = true;
bool show_test_window         = true;

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

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
  int width = canvas_get_width();
  int height = canvas_get_height();
  if (width != g_width || height != g_height)
  {
    g_width = width;
    g_height = height;
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
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(340, 300));
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
    ImGui::SetNextWindowPos(ImVec2(85, 450), ImGuiCond_FirstUseEver);
    ImGui::Begin("A Pretty Image", NULL, my_simple_window_flags);
    // ImGui::Text("pointer = %p", (void*)comfy_image.texture);
    // ImGui::Text("size = %d x %d", comfy_image.width, comfy_image.height);
    ImGui::Image((void*)(intptr_t)comfy_image.texture, ImVec2(comfy_image.draw_height, comfy_image.draw_width));
    ImGui::End();
  }

  if (show_crypto_window) {
    ImGui::Begin("My crypto links", NULL, my_simple_window_flags);
    ImGui::TextWrapped("fdfads");

    ImGui::End();
  }

  if (show_contact_info_window) {
    ImGui::SetNextWindowPos(ImVec2(1650, 50), ImGuiCond_FirstUseEver);
    ImGui::Begin("My Other Info", NULL, my_simple_window_flags);

    ImGui::Image((void*)(intptr_t)email.texture, ImVec2(email.draw_height, email.draw_width));
    ImGui::SameLine();
    ImGui::Text("jmorea03@uoguelph.ca");
    
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

    ImGui::End();
  }

  if (show_coop_window) {
    ImGui::SetNextWindowPos(ImVec2(1000, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiCond_FirstUseEver);

    ImGui::Begin("Co-op Experience");

    if (ImGui::CollapsingHeader("Fall 2020 Co-op work term report")) {
      ImGui::TextWrapped(
        "This fall I have had the fortune of having been invited to work remotely for the Co-operators as a test automation developer. "
        "This blog will serve as a journal to myself and others about my experience as a Co-op student. Hopefully, this blog will act as "
        "a time capsule to observe my progression in terms of knowledge and skills, as well as the development of my professional career."
        "\n\n"
        "My employer, The Co-operators Group Limited is a leading Canadian multi-line insurance and financial services co-operative with "
        "$47.3 billion in assets under management. I worked at a local Guelph branch in the Quality Assurance team under the Information "
        "Technology department. Since it's founding in 1945, the co-operators now offers home, business, life, farm, and travel insurance "
        "to many Canadians."
        "\n\n"
        "This was my first co-op, which also happened to be completely remote, so my expectations as far as what I was going to be learning -- "
        "that is, technologies, insurance practices, etc. -- were modest. I created a list earlier on my work term outlining the goals/learning "
        "outcomes I had in mind."
        "\n\n"
        "1. I would like to improve my oral communication in team meetings and in one-on-one."
        "\n"
        "2. I want to become more familiar with git. Specifically the advanced branching features."
        "\n"
        "3. I want to maintain a daily agenda to record my ideas, tasks, notes, etc. to have a more organized and efficient workday."
        "\n\n"
        );
      ImGui::TextWrapped( // for some reason there is a character limit on TextWrapped so I am making multiple calls.
        "When I applied for this job, though previous classes had prepared me to the best of their ability, I have had little practice working in "
        "a professional environment on a development team. My hopes were to gain hands-on experience working with a pre-existing codebase composed "
        "of multiple coupled technologies to acquire a holistic experience of software development. I was fortunate to work on the QA team where I "
        "obtained skills that will benefit me in future work experiences."
        "\n\n"
        "Looking back at the goals I outlined, I would say that for the most part, they have been successfully accomplished. For oral communication "
        "I frequently asked questions when confused; I even got to lead a scrum meeting one time. For becoming more familiar with and using advanced "
        "functionality of git source control, I worked with multiple repositories and created product features using branches and merging. "
        "With maintaining a daily agenda, I kept one next to me every day; It is pretty full now. While I can say that I have been taking notes when "
        "I feel the need to, I don't do it every day."
        "\n\n"
        "I worked on the quality assurance team and our job was to ensure that changes made to the backend of Policy Center (this is where insurance "
        "underwriters go to create an insurance policy for a client) by other development teams did not create problems for production users. To accomplish "
        "my daily tasks, I worked with a variety of technologies including Guidewire PolicyCenter, QTP (Micro Focus Unified Functional Testing), Selenium, "
        "and Java programming. Most of the programming practice I had already come from schooling. This served as a good basis, however, I learned a lot of "
        "what I ended up working on while on the job, with the help of co-workers. One example task I would be assigned to is called \"regression testing\". "
        "For this task, which we would do every few weeks, I would run our suite of test cases that we had for the auto insurance product, then report on any "
        "errors and ensure that the policies created matched in different environments. This task called on a number of my skills such as teamwork, defect (bug) "
        "analysis, reporting."
        "\n\n"
        "I really enjoyed my time at The Co-operators. I wish I could spend more time working here but school is calling me back! Hopefully, I will have a "
        "chance in the future to come back and do another work term. Maybe next time I could actually join the team in the office."
        "\n\n"
        "I would like to thank Emil Sathiya and the rest of my team for having patience and helping me learn so much in such little time under these circumstances."
        "\n\n"
      );
    }

    if (ImGui::CollapsingHeader("Summer 2021 Co-op Work Term Report")) {
      ImGui::TextWrapped(
        "This summer I had the opportunity to work at Purolator's Digital Lab, where I worked on quality assurance for a new application under development that "
        "will help partners of Purolator manage their parcel delivery. Founded in 1960, Purolator is a Canadian courier that delivers parcels to Canadians all around "
        "the country. Purolator is at the forefront of technological research and development. Currently they are looking into engineering their fleet to make "
        "deliveries easier and faster for the workers, while also reducing emissions (here's a great video to show what I'm talking about: link). I was very excited "
        "to be a part of such an innovative company."
        "\n\n"
        "It's kind of a funny story, actually. I had applied to and done an interview with another courier by the name of Shipperbee, which was based in Guelph. "
        "This company was acquired by Purolator just after they had hired me. Luckily, my boss negotiated that Co-ops would be transferred over (thanks, Dariusz!)."
        "This was my second fully remote Co-op term so the experience wasn't that alien to me. I was already used to doing scrums over video chat. I have a pretty "
        "good setup here at home. And I got a nice mac laptop sent to me so the work-from-home was comfortable. Although, for a future Co-op term I would still like "
        "to try working in an office environment."
        "\n\n"
        "The work term goals were chosen partly from what I think I missed out on last Co-op term and partly what I think would be a good opportunity at this specific "
        "employment. Anyway, the goals I outlined at the beginning of the term are as follows:"
        "\n\n"
        "1. I want to learn more about microservices and deployment in modern software development."
        "\n"
        "2. I want to maintain a daily agenda to record my ideas, tasks, notes, etc. to have a more organized and efficient workday."
        "\n"
        "3. I should be able to work effectively on my own without requiring too much assistance from or asking questions of co-workers."
        "\n"
      );
      ImGui::TextWrapped( //same thing as above
        "On the topic of my day-to-day work, without getting too specific (I think I signed an NDA), I worked with a great team of developers and QA testers on testing "
        "a new mobile application. My hours were 9-5:30 every day. I mainly worked with 2 others in the QA team. My most common work item was to re-test a filed bug on "
        "multiple devices/platforms to ensure that the issue was fixed. In terms of our testing strategies, we did manual testing, automation testing, and API testing. "
        "Doing testing at multiple \"levels\" is advantageous because you are more likely to catch bugs; like in any field of engineering, the weakest points are the "
        "moving parts, so that's where we look first. Aside from testing, we worked on documentation and task management (Azure). During my work term, I feel like I made "
        "good contributions to tooling and testing utilities used by Digital lab, that can be used and built upon in the future by other developers."
        "\n\n"
        "I would say my academics have helped me to acclimate to a professional tech environment. I think I have strong technical knowledge, which helps me understand when "
        "I am testing an issue what is happening behind the scenes. However, I would also say that during my time at Purolator, I've acquired a lot of technical knowledge "
        "about the architecture of modern mobile applications, web microservices, and the continuous integration and deployment model. The hard and soft skills I gained "
        "here will make me a more well-rounded person who is capable of working well with other teams/members who are focused on their slice of the software stack."
        "\n\n"
        "Overall, I would say that from what I have learned over the course of the last few months has made me more confident in abilities as a software developer. "
        "I would definitely be interested in coming back to Purolator in the future if that is a possibility."
        "\n\n"
        "Acknowledgments:"
        "\n"
        "Thanks to the great QA team, Pooja and Emma."
        "\n"
        "Thanks to my boss, Dariusz, and the rest of the team."
        "\n\n"
      );
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
  io.Fonts->AddFontFromFileTTF("data/fonts/Ruda/static/Ruda-SemiBold.ttf", 15.0f);
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

void load_custom_images() {
  comfy_image.ret = LoadTextureFromFile("data/comfy.jpg", &comfy_image.texture, &comfy_image.width, &comfy_image.height);
  github.ret = LoadTextureFromFile("data/github.png", &github.texture, &github.width, &github.height);
  email.ret = LoadTextureFromFile("data/email.png", &email.texture, &email.width, &email.height);
  home.ret = LoadTextureFromFile("data/home.png", &home.texture, &home.width, &home.height);
  home.ret = LoadTextureFromFile("data/linkedin.png", &linkedin.texture, &linkedin.width, &linkedin.height);
}

int init() {
  init_gl();
  init_imgui();
  load_custom_images();
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
