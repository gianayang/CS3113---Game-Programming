#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix2, modelMatrix3, projectionMatrix;

glm::vec3 player_movement = glm::vec3(0, 0, 0);
glm::vec3 player_position = glm::vec3(-4.75, 0, 0);

glm::vec3 player_movement1 = glm::vec3(0, 0, 0);
glm::vec3 player_position1 = glm::vec3(4.75, 0, 0);

glm::vec3 ball_pos = glm::vec3(0, 0, 0);
//glm::vec3 ball_movement = glm::vec3(0, 0, 0);
//glm::vec3 ball_position = glm::vec3(0, 0, 0);
float ball_x = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f + 1.0f)));
float ball_y = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f + 1.0f)));

float p_1_y = 1.0f;
float p_2_y = 1.0f;

float player_speed = 1.0f;
float player_speed1 = 1.0f;
float ball_speed_x = 3.0f;
float ball_speed_y = 3.0f;
 
bool start = false;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    modelMatrix2 = glm::mat4(1.0f);
    modelMatrix3 = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(1.0f, 1.0f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void ProcessInput() {
    player_movement = glm::vec3(0);
    player_movement1 = glm::vec3(0);


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
                break;
            }
            break; // SDL_KEYDOWN
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_SPACE]) {
        start = true;
    }
    if (keys[SDL_SCANCODE_UP] && start) {
        player_movement1.y = 1.0f;
        p_2_y += 1.0f;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        player_movement1.y = -1.0f;
        p_2_y -= 1.0f;
    }
    if (keys[SDL_SCANCODE_W]) {
        player_movement.y = 1.0f;
        p_1_y += 1.0f;
    }
    if (keys[SDL_SCANCODE_S]) {
        player_movement.y = -1.0f;
        p_1_y -= 1.0f;
    }

}

float lastTicks = 0;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    modelMatrix = glm::mat4(1.0f);
    if (p_1_y < 210.0f && p_1_y > -210.0f) {
        player_position += player_movement * player_speed * deltaTime;

    }
    modelMatrix = glm::translate(modelMatrix, player_position);

    modelMatrix2 = glm::mat4(1.0f);
    if (p_2_y < 210.0f && p_2_y > -210.0f) {
        player_position1 += player_movement1 * player_speed1 * deltaTime;
        
    }
    modelMatrix2 = glm::translate(modelMatrix2, player_position1);
    
    modelMatrix3 = glm::mat4(1.0f);
    if (start) {
        if (ball_pos.x < -4.75f || ball_pos.x > 4.75f) {
            gameIsRunning = false;
        }
        float x_diff = fabs(-4.75f - ball_pos.x) - 0.5f;
        float y_diff = fabs(player_position.y - ball_pos.y) - ((2.0f + 0.5f) / 2.0f);
        float y_diff1 = fabs(player_position1.y - ball_pos.y) - ((2.0f + 0.5f) / 2.0f);
        float x_diff1 = fabs( 4.75f- ball_pos.x) - 0.5f;
        if ((x_diff < 0.0f && y_diff < 0.0f)|| (x_diff1 < 0.0f && y_diff1 < 0.0f) ) {
            ball_speed_x = ball_speed_x * -1;
        }
        else if (ball_pos.y > 3.50f || ball_pos.y < -3.50f) {
            ball_speed_y = ball_speed_y * -1;
        }
        ball_pos.x += ball_x * ball_speed_x * deltaTime;
        ball_pos.y += ball_y * ball_speed_y * deltaTime;
        
       
    }
    modelMatrix3 = glm::translate(modelMatrix3, ball_pos);
    
}

void DrawBar(glm::mat4 matrix) {
    program.SetModelMatrix(matrix);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render() {
    float vertices[] = { -0.25, -1.0, 0.25, -1.0, 0.25, 1.0, -0.25, -1.0, 0.25, 1.0, -0.25, 1.0 };
    float vertices3[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);


    DrawBar(modelMatrix);
    DrawBar(modelMatrix2);

    program.SetModelMatrix(modelMatrix3);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}