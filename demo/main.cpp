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
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <vector>



#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define EDITOR_MODE_POSITION glm::vec3(0.f, 3.5f, 10.f)

namespace imgui = ImGui;


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
    {0.1f, 0.9, 1.0f},
    {0.1f, 1.f, 0.9f},
    32.f
};

Mesh *sphereMesh;

Camera camera;



std::vector<glm::vec2> profile {
    glm::vec2(0.0f, 0.05f),
    glm::vec2(-0.05f, 0.15f),
    glm::vec2(-0.15f, 0.15f),
    glm::vec2(-0.20f, 0.05f),
    glm::vec2(0.00f, -0.15f),
    glm::vec2(0.20f, 0.05f),
    glm::vec2(0.15f, 0.15f),
    glm::vec2(0.05f, 0.15f),
};
std::vector<BezierCurvePoint> curvePoints {
    {{-5.f, 0.f, 0.f}, 0.3f},
    {{-2.f, 7.f, -1.f}, 1.f},
    {{5.f, 0.f, -2.f}, 1.f},
    {{2.f, 7.f, -3.f}, 0.1f},
};
int segmentCount = 50;


bool isInEditorMode = false;
int selectedCurvePoint = 0;
bool isDragging = false;
glm::vec4 debugWindowRect; //x, y, w, h
SDL_Cursor *cursorHand;
const float DRAGGING_SPEED = 0.02f;

CurveMeshData curveMeshData;



void handleInput(SDL_Event &event, bool &running) 
{
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
    else if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        auto x = event.button.x;
        auto y = event.button.y;
        if( !(x >= debugWindowRect.x && x < debugWindowRect.x + debugWindowRect.z && y >= debugWindowRect.y && y < debugWindowRect.y + debugWindowRect.w ) )
        {
            isDragging = true;
        }
    }
    else if(event.type == SDL_MOUSEBUTTONUP)
    {
        isDragging = false;
    }
    else if(event.type == SDL_MOUSEMOTION)
    {
        if(isInEditorMode && isDragging)
        {
            curvePoints[selectedCurvePoint].position += glm::vec3(
                (float)event.motion.xrel * DRAGGING_SPEED,
                (float)-event.motion.yrel * DRAGGING_SPEED,
                0.f
            );
        }
    }
}

void debugWindow()
{
    imgui::Begin("Debug menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if(isInEditorMode)
    {
        if(imgui::Button("Go to freeroam mode"))
        {
            isInEditorMode = false;
        }
    }
    else
    {
        if(imgui::Button("Go to editor mode"))
        {
            isInEditorMode = true;
            camera.setPosition(EDITOR_MODE_POSITION);
            camera.setRotation(-90.f, 0.f);
        }
    }
    
    if(imgui::BeginTabBar("Scene properties"))
    {
        if(imgui::BeginTabItem("Mesh material"))
        {
            imgui::ColorEdit3("Diffuse##mesh", (float *)glm::value_ptr(curveMaterial.diffuse));
            imgui::ColorEdit3("Specular##mesh", (float *)glm::value_ptr(curveMaterial.specular));
            imgui::SliderFloat("Shininess##mesh", &curveMaterial.shininess, 0.0f, 128.0f);

            imgui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Light"))
        {
            imgui::SliderFloat3("Position##light", (float *)glm::value_ptr(light.position), -20.0f, 20.0f);
            imgui::ColorEdit3("Ambient##light", (float *)glm::value_ptr(light.ambient));
            imgui::ColorEdit3("Diffuse##light", (float *)glm::value_ptr(light.diffuse));
            imgui::ColorEdit3("Specular##light", (float *)glm::value_ptr(light.specular));
            
            imgui::EndTabItem();
        }
        if(imgui::BeginTabItem("Curve"))
        {
            if(!isInEditorMode)
            {
                imgui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                imgui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            imgui::SliderInt("Segment count##curve", &segmentCount, 1, 200);

            imgui::Text("Point 1");
            imgui::SameLine();
            if(imgui::Button("Select##curve_point_select1")) {
                selectedCurvePoint = 0;
            }
            imgui::SameLine();
            imgui::SliderFloat("##curve_point_ratio1", &curvePoints[0].ratio, 0.0f, 2.0f);
            
            imgui::Text("Point 2");
            imgui::SameLine();
            if(imgui::Button("Select##curve_point_select2")) {
                selectedCurvePoint = 1;
            }
            imgui::SameLine();
            imgui::SliderFloat("##curve_point_ratio2", &curvePoints[1].ratio, 0.0f, 2.0f);

            imgui::Text("Point 3");
            imgui::SameLine();
            if(imgui::Button("Select##curve_point_select3")) {
                selectedCurvePoint = 2;
            }
            imgui::SameLine();
            imgui::SliderFloat("##curve_point_ratio3", &curvePoints[2].ratio, 0.0f, 2.0f);

            imgui::Text("Point 4");
            imgui::SameLine();
            if(imgui::Button("Select##curve_point_select4")) {
                selectedCurvePoint = 3;
            }
            imgui::SameLine();
            imgui::SliderFloat("##curve_point_ratio4", &curvePoints[3].ratio, 0.0f, 2.0f);
            
            if(!isInEditorMode)
            {
                imgui::PopItemFlag();
                imgui::PopStyleVar();
            }
            
            imgui::EndTabItem();
        }

        imgui::EndTabBar();
    }

    auto pos = imgui::GetWindowPos();
    auto size = imgui::GetWindowSize();
    debugWindowRect = glm::vec4(pos.x, pos.y, size.x, size.y);

    imgui::End();
}

void enableLighting()
{
    glUniform3fv(unifLocLightPosition, 1, glm::value_ptr(light.position));
    glUniform3fv(unifLocLightAmbient, 1, glm::value_ptr(light.ambient));
    glUniform3fv(unifLocLightDiffuse, 1, glm::value_ptr(light.diffuse));
    glUniform3fv(unifLocLightSpecular, 1, glm::value_ptr(light.specular));
}

void disableLighting()
{
    // this way it will always be independent from light's position
    // will apply the same color everywhere on the mesh
    // and will only use mesh's diffuse color
    glUniform3f(unifLocLightAmbient, 1.f, 1.f, 1.f);
    glUniform3f(unifLocLightDiffuse, 0.f, 0.f, 0.f);
    glUniform3f(unifLocLightSpecular, 0.f, 0.f, 0.f);
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

    glUniform3fv(unifLocMaterialDiffuse, 1, glm::value_ptr(light.diffuse));

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

    cursorHand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    imgui::CreateContext();
    imgui::StyleColorsDark();
    ImGuiIO& io = imgui::GetIO(); (void)io;
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
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    SDL_GL_SetSwapInterval(1);


    GLuint shader = loadShaderProgramFromFiles("data/phong.vs.glsl", "data/phong.fs.glsl");
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


    curveMesh = new Mesh();
    sphereMesh = new Mesh();
    sphereMesh->load("data/sphere.obj");


    camera.setPosition(glm::vec3(0.f, 3.5f, 10.f));

    curveMeshData = extrudeProfileWithCurve(profile, curvePoints, segmentCount);
    curveMesh->load(curveMeshData.vertices, curveMeshData.normals, curveMeshData.indices);


    SDL_Event e;
    bool running = true;
    unsigned int prevTick, currTick;
    prevTick = currTick = SDL_GetTicks();
    while(running)
    {
        ImGui_ImplSDL2_NewFrame();

        currTick = SDL_GetTicks();
        float dt = (currTick - prevTick) / 1000.0f;

        while(SDL_PollEvent(&e))
        {
            handleInput(e, running);

            if(!isInEditorMode)
            {
                camera.handleEvent(e, dt);
            }

            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        if(isDragging && isInEditorMode)
        {
            SDL_SetCursor(cursorHand);
        }

        camera.update();

        prevTick = currTick;
        


        ImGui_ImplOpenGL3_NewFrame();
        imgui::NewFrame();

        debugWindow();
        // ImGui::ShowDemoWindow();
        imgui::Render();



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(unifLocView, 1, GL_FALSE, glm::value_ptr(camera.getView()));
        glUniformMatrix4fv(unifLocProjection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
        glUniform3fv(unifLocCameraPosition, 1, glm::value_ptr(camera.getPosition()));

        enableLighting();

        if(isInEditorMode)
        {
            curveMeshData = extrudeProfileWithCurve(profile, curvePoints, segmentCount);
            curveMesh->load(curveMeshData.vertices, curveMeshData.normals, curveMeshData.indices);
        }

        renderMesh(curveMesh, curveMaterial);

        disableLighting();
        
        renderLightSphere();

        if(isInEditorMode)
        {
            // so that points are visible no matter what
            glClear(GL_DEPTH_BUFFER_BIT);

            Material disabledPointMat {
                {1.f, 1.f, 0.f},
                {1.f, 1.f, 0.f},
                0.f
            };
            Material enabledPointMat {
                {0.f, 1.f, 0.f},
                {0.f, 1.f, 0.f},
                0.f
            };

            for (int i = 0; i < curvePoints.size(); i++)
            {
                if(i == selectedCurvePoint)
                {
                    renderMesh(sphereMesh, enabledPointMat, curvePoints[i].position, 0.05f);
                }
                else
                {
                    renderMesh(sphereMesh, disabledPointMat, curvePoints[i].position, 0.05f);
                }
                
            }
        }


        ImGui_ImplOpenGL3_RenderDrawData(imgui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    delete curveMesh; 
    delete sphereMesh;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    imgui::DestroyContext();

    SDL_FreeCursor(cursorHand);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
