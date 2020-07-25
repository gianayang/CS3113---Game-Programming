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
#include "Entity.h"
#include <vector>

#define ENEMY_COUNT 3
#define PLATFORM_COUNT 10


struct GameState {
    Entity* player;
    Entity* enemies;
    Entity* platform;
};
   
GameState state;
SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
GLuint playerTextureID;
GLuint fontTextureID;




GLuint LoadTexture(const char* filePath, const char* texture) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    if (texture == "nearest") {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }


    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state.player = new Entity();
    state.player->position = glm::vec3(0, 1.0f, 0);
    state.player->entityType = EntityType::PLAYER;
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -2.0f, 0);
    state.player->speed = 1.5f;
    state.player->jumpPower = 2.0f;
    state.player->textureID = LoadTexture("george.png","nearest");

    state.player->height = 0.85f;
    state.player->width = 0.55f;

    state.platform = new Entity[PLATFORM_COUNT];

    GLuint platformTextureID = LoadTexture("platformPack_tile001.png","nearest");
    state.platform[0].textureID = platformTextureID;
    state.platform[0].position = glm::vec3(4.50f, -3.25f, 0);
    state.platform[0].entityType = EntityType::PLATFORM;

    state.platform[1].textureID = platformTextureID;
    state.platform[1].position = glm::vec3(3.50f, -3.25f, 0);
    state.platform[1].entityType = EntityType::PLATFORM;

    state.platform[2].textureID = platformTextureID;
    state.platform[2].position = glm::vec3(2.50f, -3.25f, 0);
    state.platform[2].entityType = EntityType::PLATFORM;

    state.platform[3].textureID = platformTextureID;
    state.platform[3].position = glm::vec3(1.50f, -3.25f, 0);
    state.platform[3].entityType = EntityType::PLATFORM;

    state.platform[4].textureID = platformTextureID;
    state.platform[4].position = glm::vec3(0.50f, -3.25f, 0);
    state.platform[4].entityType = EntityType::PLATFORM;

    state.platform[9].textureID = platformTextureID;
    state.platform[9].position = glm::vec3(-0.50f, -3.25f, 0);
    state.platform[9].entityType = EntityType::PLATFORM;

    state.platform[5].textureID = platformTextureID;
    state.platform[5].position = glm::vec3(-1.50f, -3.25f, 0);
    state.platform[5].entityType = EntityType::PLATFORM;

    state.platform[6].textureID = platformTextureID;
    state.platform[6].position = glm::vec3(-2.50f, -3.25f, 0);
    state.platform[6].entityType = EntityType::PLATFORM;

    state.platform[7].textureID = platformTextureID;
    state.platform[7].position = glm::vec3(-3.50f, -3.25f, 0);
    state.platform[7].entityType = EntityType::PLATFORM;

    state.platform[8].textureID = platformTextureID;
    state.platform[8].position = glm::vec3(-4.50f, -3.25f, 0);
    state.platform[8].entityType = EntityType::PLATFORM;

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Update(0, state.player,NULL, 0,NULL,0);
    }

    state.player->animRight = new int[4]{ 8, 9,10,11 };
    state.player->animLeft = new int[4]{ 4, 5, 6, 7 };
    state.player->animUp = new int[4]{ 0, 1, 2, 3 };
    state.player->animDown = new int[4]{ 12, 13,14,15 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;

    state.enemies = new Entity[ENEMY_COUNT];

    GLuint enemyTextureID = LoadTexture("cat.png","nearest");
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(-3.0f, -2.25f, 0.0f);
    state.enemies[0].movement = glm::vec3(0, 0, 0);
    state.enemies[0].entityType = EntityType::ENEMY;
    state.enemies[0].aiType = AIType::WALKER;
    state.enemies[0].aiState = AISTATE::WALKING;
    state.enemies[0].speed = 1;
    state.enemies[0].width = 0.6;
    state.enemies[0].height = 0.8;

    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(2.0f, -2.25f, 0.0f);
    state.enemies[1].movement = glm::vec3(0, 0, 0);
    state.enemies[1].entityType = EntityType::ENEMY;
    state.enemies[1].aiType = AIType::WAITANDGO;
    state.enemies[1].aiState = AISTATE::IDLE;
    state.enemies[1].speed = 1;
    state.enemies[1].width = 0.6;
    state.enemies[1].height = 0.8;

    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(4.0f, 1.0f, 0.0f);
    state.enemies[2].movement = glm::vec3(1, 1, 0);
    state.enemies[2].entityType = EntityType::ENEMY;
    state.enemies[2].aiType = AIType::WALKER;
    state.enemies[2].aiState = AISTATE::ATTACKING;
    state.enemies[2].speed = 1;
    state.enemies[2].acceleration.y = -1;
    state.enemies[2].width = 0.6;
    state.enemies[2].height = 0.8;

    fontTextureID = LoadTexture("font.png","nearest");
}



void ProcessInput() {
    state.player->movement = glm::vec3(0);


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
                //if (state.player->collidedBottom) {
                    state.player->jump = true;
                //}
                break;
            }
            break; // SDL_KEYDOWN
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
} 

float lastTicks = 0;
#define FIXED_TIMESTEP 0.01666666666F
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
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.player, state.platform, PLATFORM_COUNT,state.enemies,ENEMY_COUNT);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player,state.platform, PLATFORM_COUNT, NULL, 0);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator -= deltaTime;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {

        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
             offset + (-0.5f * size), 0.5f * size,
             offset + (-0.5f * size), -0.5f * size,
             offset + (0.5f * size), 0.5f * size,
             offset + (0.5f * size), -0.5f * size,
             offset + (0.5f * size), 0.5f * size,
             offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
                u, v,
                u, v + height,
                u + width, v,
                u + width, v + height,
                u + width, v,
                u, v + height,
            });

    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Render(&program);
    }

    state.player->Render(&program);

    if (state.player->gameEnd) {
        if (state.player->win) {
            DrawText(&program, fontTextureID, "You Win", 0.5f, 0.01f, glm::vec3(-4.5f, 1.0f, 0));
        }
        else {
            DrawText(&program, fontTextureID, "You Lost", 0.5f, 0.01f, glm::vec3(-4.5f, 1.0f, 0));
        }
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
        if (!state.player->gameEnd) {
            Update();
        }
        Render();
    }

    Shutdown();
    return 0;
}