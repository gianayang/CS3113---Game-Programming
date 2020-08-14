#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define LEVEL_LENGTH 2
#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>

#include "Util.h"
#include "Entity.h"

#include "StartPage.h"
#include "Level1.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

glm::mat4 uiViewMatrix, uiProjectionMatrix;
GLuint fontTextureID;
GLuint heartTextureID;


GameState state;
Scene* currentScene;
Scene* sceneList[2];

GLuint bulletTextureID;
Mesh* bulletMesh;

bool gameStart = false;
void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("3D!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 720);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f);


    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    fontTextureID = Util::LoadTexture("font.png");
    heartTextureID = Util::LoadTexture("platformPack_item017.png");


    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    sceneList[0] = new StartPage();
    sceneList[1] = new Level1();
    SwitchToScene(sceneList[0]);

    bulletTextureID = Util::LoadTexture("textures.bmp");

    bulletMesh = new Mesh();
    bulletMesh->LoadOBJ("pingu.obj", 1);
}



void ProcessInput() {
    if (gameStart == true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    Entity* bullet = new Entity();
                    bullet->textureID = bulletTextureID;
                    bullet->mesh = bulletMesh;
                    bullet->position = currentScene->state.player->position;

                    bullet->rotation = glm::vec3(0, -90, -90);
                    bullet->velocity.x = currentScene->state.player->velocity.x;
                    bullet->scale = glm::vec3(0.01f, 0.01f, 0.01f);
                    bullet->entityType = EntityType::BULLET;
                    currentScene->state.bullets.push_back(bullet);
                    break;
                }
                break;
            }
            
        }
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_A]) {
            currentScene->state.player->rotation.y += 1.0f;
        }
        else if (keys[SDL_SCANCODE_D]) {
            currentScene->state.player->rotation.y -= 1.0f;
        }
        currentScene->state.player->velocity.x = 0;
        currentScene->state.player->velocity.z = 0;

        if (keys[SDL_SCANCODE_W]) {
            currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
            currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
        }
        else if (keys[SDL_SCANCODE_S]) {
            currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
            currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
        }
    }
    else {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_RETURN:
                    gameStart = true;
                    currentScene->state.nextScene = 1;
                    break; // SDL_KEYDOWN
                }
            }
        }
    }
    
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(currentScene->state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -currentScene->state.player->position);

    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);
    //state.player->Render(&program);
    if (gameStart == false) {
        Util::DrawText(&program, fontTextureID, "Adventure of G", 0.4f, 0.01f, glm::vec3(-2.5f, 2.5f, 0));
        Util::DrawText(&program, fontTextureID, "Press Enter to Play!", 0.4f, 0.01f, glm::vec3(-3.5f, 1.0f, 0));
    }
    else {
        program.SetProjectionMatrix(projectionMatrix);
        currentScene->Render(&program);
        //Util::DrawText(&program, fontTextureID, "lives left: " + std::to_string(currentScene->state.player->lives), 0.2f, 0.01f, glm::vec3(2.0f, 3.50f, 0));
        if (currentScene->state.player->lives <= 0) {
            Util::DrawText(&program, fontTextureID, "You Lost", 0.7f, 0.01f, glm::vec3(-2.5f, 2.0f, 0));
        }
        /*if (currentScene->state.player->win == true && currentScene->state.player->level == LEVEL_LENGTH - 1) {
            Util::DrawText(&programUI, fontTextureID, "You Win", 0.7f, 0.01f, glm::vec3(-2.5f, 2.0f, 0));
        }
        else {
            currentScene->state.player->gameEnd = false;
        }
        */
    }
    

    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);

    Util::DrawText(&program, fontTextureID, "Lives: 3", 0.5, -0.3f, glm::vec3(-6, 3.2, 0));

    for (int i = 0; i < 3; i++)
    {
        // These icons are small, so just move 0.5 to the right for each one.
        Util::DrawIcon(&program, heartTextureID, glm::vec3(5 + (i * 0.5f), 3.2, 0));
    }

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        if (!currentScene->state.player->gameEnd) {
            Update();
            int tempLives = currentScene->state.player->lives;
            if (currentScene->state.nextScene >= 0) {
                SwitchToScene(sceneList[currentScene->state.nextScene]);
                currentScene->state.player->lives = tempLives;
            }
            if (currentScene->state.player->gameEnd) {
                sceneList[1] = new Level1();
                SwitchToScene(sceneList[1]);
                tempLives--;
                currentScene->state.player->lives = tempLives;
            }
        }
        Render();
    }

    Shutdown();
    return 0;
}
