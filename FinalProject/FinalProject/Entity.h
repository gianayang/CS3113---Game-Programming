#pragma once
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
#include "Mesh.h"
#include <SDL_mixer.h>

enum class EntityType { PLAYER, PLATFORM, ENEMY_Billboard, ENEMY, CRATE,FLOOR, BULLET,PLANET};

class Entity {
public:
    EntityType entityType;

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 rotation;
    glm::vec3 scale;

    float speed;
    bool billboard;
    float width;
    float height;
    float depth;


    bool gameEnd = false;
    bool win = false;
    bool valid = true;
    int lives = 3;
    int enemyLeft;
    int level;

    GLuint textureID;
    Mesh* mesh;
    Mix_Chunk* attack;
    Mix_Chunk* dead;

    glm::mat4 modelMatrix;

    Entity();

    bool CheckCollision(Entity* other);
    void DrawBillboard(ShaderProgram* program);
    void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* enemies, int enemyCount);
    void Render(ShaderProgram* program);
};



