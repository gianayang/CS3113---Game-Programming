#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#define LEVEL_LENGTH 4

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include <vector>

#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "StartPage.h"

#include "Effects.h"

Mix_Chunk* bounce;

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program;
ShaderProgram programUI;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 viewMatrix1, modelMatrix1, projectionMatrix1;
GLuint playerTextureID;

GLuint fontTextureID_main;

Scene* currentScene;
Scene* sceneList[4];

Effects* effects;

bool gameStart = false;
void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_lit.glsl", "shaders/fragment_lit.glsl");
    programUI.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    viewMatrix1 = glm::mat4(1.0f);
    modelMatrix1= glm::mat4(1.0f);
    projectionMatrix1 = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    programUI.SetProjectionMatrix(projectionMatrix1);
    programUI.SetViewMatrix(viewMatrix1);
    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fontTextureID_main = Util::LoadTexture("font.png");

    // Clear stuff out of initialize and add this to the bottom:
    sceneList[0] = new StartPage();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);

    effects = new Effects(projectionMatrix, viewMatrix);
   // effects->Start(EffectType::FADEIN,0.5f);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);
    bounce = Mix_LoadWAV("bounce.wav");
}



void ProcessInput() {
    if (gameStart == true) {
        currentScene->state.player->movement = glm::vec3(0);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    // Move the player left
                    break;

                case SDLK_RIGHT:
                    // Move the player right
                    break;

                case SDLK_SPACE:
                    // Some sort of action
                    if (currentScene->state.player->collidedBottom) {
                        currentScene->state.player->jump = true;
                        Mix_PlayChannel(-1, bounce, 0);
                    }
                    break;
                }
                break; // SDL_KEYDOWN
            }
        }
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }

        if (glm::length(currentScene->state.player->movement) > 1.0f) {
            currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
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

float lastTicks = 0;
#define FIXED_TIMESTEP 0.01666666666F
float accumulator = 0.0f;

bool lastCollidedBottom = false;

void Update() {
    if (gameStart) {
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;

        deltaTime += accumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            accumulator = deltaTime;
            return;
        }

        while (deltaTime >= FIXED_TIMESTEP) {
            // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
            currentScene->Update(FIXED_TIMESTEP);

            program.SetLightPosition(currentScene->state.player->position);

            if (lastCollidedBottom == false && currentScene->state.player->collidedBottom) {
                if (currentScene->state.player->lastCollided == EntityType::ENEMY) {
                    effects->Start(EffectType::SHAKE, 2.0f);
                }
                lastCollidedBottom = true;
            }
            lastCollidedBottom = currentScene->state.player->collidedBottom;
            effects->Update(FIXED_TIMESTEP);

            deltaTime -= FIXED_TIMESTEP;
        }
        accumulator -= deltaTime;

        viewMatrix = glm::mat4(1.0f);
        if (currentScene->state.player->position.x > 5) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        }
        else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }

        viewMatrix = glm::translate(viewMatrix, effects->viewOffset);
    }
    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);

    glUseProgram(programUI.programID);
    if (gameStart == false) {
        Util::DrawText(&programUI, fontTextureID_main, "Adventure of G", 0.4f, 0.01f, glm::vec3(-2.5f, 2.5f, 0));
        Util::DrawText(&programUI, fontTextureID_main, "Press Enter to Play!", 0.4f, 0.01f, glm::vec3(-3.5f, 1.0f, 0));
    }
    else {
        currentScene->Render(&program);
        Util::DrawText(&programUI, fontTextureID_main, "lives left: " + std::to_string(currentScene->state.player->lives), 0.2f, 0.01f, glm::vec3(2.0f, 3.50f, 0));
        if (currentScene->state.player->lives <= 0) {
           Util::DrawText(&programUI, fontTextureID_main, "You Lost", 0.7f, 0.01f, glm::vec3(-2.5f, 2.0f, 0));
        }
        if (currentScene->state.player->win == true && currentScene->state.player->level == LEVEL_LENGTH - 1) {
            Util::DrawText(&programUI, fontTextureID_main, "You Win", 0.7f, 0.01f, glm::vec3(-2.5f, 2.0f, 0));
        }
        else {
            currentScene->state.player->gameEnd = false;
        }
        effects->Render();
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
                effects->Start(EffectType::FADEOUT,0.2f);
                SwitchToScene(sceneList[currentScene->state.nextScene]);
                currentScene->state.player->lives = tempLives;
                effects->Start(EffectType::FADEIN,0.2f);
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