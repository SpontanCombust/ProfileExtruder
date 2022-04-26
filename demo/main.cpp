#include "utils/shader_program.hpp"
#include "utils/camera.hpp"
#include "utils/mesh.hpp"
#include "utils/light.hpp"
#include "utils/material.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/type_ptr.hpp>
#include <bezier_curve.hpp>
#include <curve_mesh.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <vector>



#define WIN_WIDTH 1280
#define WIN_HEIGHT 720


GLint unifLocTranslation;
GLint unifLocScale;
GLint unifLocView;
GLint unifLocProjection;

GLint unifLocCameraPosition;
GLint unifLocMaterialDiffuse;
GLint unifLocMaterialSpecular;
GLint unifLocMaterialShininess;
GLint unifLocLightPosition;
GLint unifLocLightAmbient;
GLint unifLocLightDiffuse;
GLint unifLocLightSpecular;


Light light {
    glm::vec3(0.f, 10.f, 5.f),

    glm::vec3(0.1f, 0.1f, 0.1f),
    glm::vec3(0.8f, 0.8f, 0.8f),
    glm::vec3(0.8f, 0.8f, 0.8f)
};

Mesh *curveMesh;
Material curveMaterial {
    {0.1f, 0.5, 1.0f},
    {0.1f, 0.5, 1.0f},
    0.01f
};

Mesh *sphereMesh;


void handleInput(SDL_Event &event, bool &running) {
    if (event.type == SDL_QUIT) 
    {
        running = false;
    } 
    else if(event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                running = false;
                break;
            default:
                break;
        }
    }
}

void scenePropertiesWindow()
{
    ImGui::Begin("Debug menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if(ImGui::BeginTabBar("Scene properties"))
    {
        if(ImGui::BeginTabItem("Mesh material"))
        {
            ImGui::ColorEdit3("Diffuse##mesh", (float *)glm::value_ptr(curveMaterial.diffuse));
            ImGui::ColorEdit3("Specular##mesh", (float *)glm::value_ptr(curveMaterial.specular));
            ImGui::SliderFloat("Shininess##mesh", &curveMaterial.shininess, 0.0f, 128.0f);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Light"))
        {
            ImGui::SliderFloat3("Position##light", (float *)glm::value_ptr(light.position), -20.0f, 20.0f);
            ImGui::ColorEdit3("Ambient##light", (float *)glm::value_ptr(light.ambient));
            ImGui::ColorEdit3("Diffuse##light", (float *)glm::value_ptr(light.diffuse));
            ImGui::ColorEdit3("Specular##light", (float *)glm::value_ptr(light.specular));
            
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Curve"))
        {

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void setLightProperties()
{
    glUniform3fv(unifLocLightPosition, 1, glm::value_ptr(light.position));
    glUniform3fv(unifLocLightAmbient, 1, glm::value_ptr(light.ambient));
    glUniform3fv(unifLocLightDiffuse, 1, glm::value_ptr(light.diffuse));
    glUniform3fv(unifLocLightSpecular, 1, glm::value_ptr(light.specular));
}

void renderMesh(const Mesh* mesh, const Material& material, glm::vec3 translation = glm::vec3(0.f), float scale = 1.f)
{
    glUniform3fv(unifLocTranslation, 1, glm::value_ptr(translation));
    glUniform1f(unifLocScale, scale);

    glUniform3fv(unifLocMaterialDiffuse, 1, glm::value_ptr(material.diffuse));
    glUniform3fv(unifLocMaterialSpecular, 1, glm::value_ptr(material.specular));
    glUniform1f(unifLocMaterialShininess, material.shininess);

    mesh->draw();
}

void renderLightSphere()
{
    glUniform3fv(unifLocTranslation, 1, glm::value_ptr(light.position));
    glUniform1f(unifLocScale, 0.05f);

    // a little hack to make sphere have the same plain color on the whole mesh corresponding to light's attributes
    glUniform3f(unifLocMaterialDiffuse, 1.f, 1.f, 1.f);
    glUniform3f(unifLocMaterialSpecular, 1.f, 1.f, 1.f);
    glUniform1f(unifLocMaterialShininess, 1.f);

    glUniform3fv(unifLocLightAmbient, 1, glm::value_ptr(light.diffuse));
    glUniform3f(unifLocLightDiffuse, 0.f, 0.f, 0.f);
    glUniform3f(unifLocLightSpecular, 0.f, 0.f, 0.f);

    sphereMesh->draw();
}

int main(int argc, char const *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "Curves!", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_OPENGL
    );

    SDL_GLContext context = SDL_GL_CreateContext(window);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 330");



    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        printf("glewInit Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    SDL_GL_SetSwapInterval(1);


    GLuint shader = loadShaderProgramFromFiles("data/shader.vs.glsl", "data/shader.fs.glsl");
    glUseProgram(shader);

    unifLocTranslation = glGetUniformLocation(shader, "uTranslation");
    unifLocScale = glGetUniformLocation(shader, "uScale");
    unifLocView = glGetUniformLocation(shader, "uView");
    unifLocProjection = glGetUniformLocation(shader, "uProjection");

    unifLocCameraPosition = glGetUniformLocation(shader, "uCameraPosition");
    unifLocMaterialDiffuse = glGetUniformLocation(shader, "uMaterial.diffuse");
    unifLocMaterialSpecular = glGetUniformLocation(shader, "uMaterial.specular");
    unifLocMaterialShininess = glGetUniformLocation(shader, "uMaterial.shininess");
    unifLocLightPosition = glGetUniformLocation(shader, "uLight.position");
    unifLocLightAmbient = glGetUniformLocation(shader, "uLight.ambient");
    unifLocLightDiffuse = glGetUniformLocation(shader, "uLight.diffuse");
    unifLocLightSpecular = glGetUniformLocation(shader, "uLight.specular");

    
    Camera camera;
    curveMesh = new Mesh();
    sphereMesh = new Mesh();
    sphereMesh->load("data/sphere.obj");


    std::vector<glm::vec2> profile {
        glm::vec2(0.f, 0.4f),
        glm::vec2(0.2f, 0.f),
        glm::vec2(0.f, -0.4f),
        glm::vec2(-0.2f, 0.f),
    };

    std::vector<BezierCurvePoint> curvePoints {
        {{-5.f, 0.f, 0.f}, 0.3f},
        {{-2.f, 7.f, -1.f}, 1.f},
        {{5.f, 0.f, -2.f}, 1.f},
        {{2.f, 7.f, -3.f}, 0.1f},
    };

    unsigned int segmentCount = 100;


    camera.setPosition(glm::vec3(0.f, 3.5f, 10.f));

    // this will later reload every time change is made to the curve
    CurveMeshData curveMeshData = extrudeProfileWithCurve(profile, curvePoints, segmentCount);
    curveMesh->load(curveMeshData.vertices, curveMeshData.normals, curveMeshData.indices);

    SDL_Event e;
    bool running = true;
    unsigned int prevTick, currTick;
    prevTick = currTick = SDL_GetTicks();
    while(running)
    {
        currTick = SDL_GetTicks();
        float dt = (currTick - prevTick) / 1000.0f;

        while(SDL_PollEvent(&e))
        {
            handleInput(e, running);
            camera.handleEvent(e, dt);
            ImGui_ImplSDL2_ProcessEvent(&e);
        }
        camera.update();
        prevTick = currTick;
        

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        scenePropertiesWindow();
        // ImGui::ShowDemoWindow();
        ImGui::Render();



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUniformMatrix4fv(unifLocView, 1, GL_FALSE, glm::value_ptr(camera.getView()));
        glUniformMatrix4fv(unifLocProjection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
        glUniform3fv(unifLocCameraPosition, 1, glm::value_ptr(camera.getPosition()));

        setLightProperties();
        renderMesh(curveMesh, curveMaterial);
        
        renderLightSphere();


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    delete curveMesh; 
    delete sphereMesh;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
