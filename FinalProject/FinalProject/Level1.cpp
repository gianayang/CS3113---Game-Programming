#include "Level1.h"
#define OBJECT_COUNT 11
#define ENEMY_COUNT 10

Mix_Music* music;

void Level1::Initialize() {

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("horror.mp3");
    Mix_PlayMusic(music, -1);

    state.player = new Entity();
    state.player->entityType = EntityType::PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;
    state.player->velocity = glm::vec3(0, 0, 0);

    state.objects = new Entity[OBJECT_COUNT];
    GLuint floorTextureID = Util::LoadTexture("galaxy.png");
    Mesh* cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("cube.obj", 10);
    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0, -0.25f, 0);
    state.objects[0].rotation = glm::vec3(0, 0, 0);
    state.objects[0].acceleration = glm::vec3(0, 0, 0);
    state.objects[0].scale = glm::vec3(1000, 0.3f, 1000);
    
    state.objects[1].entityType = EntityType::FLOOR;
    state.objects[1].textureID = floorTextureID;
    state.objects[1].mesh = cubeMesh;
    state.objects[1].position = glm::vec3(0, 10.0f, 0);
    state.objects[1].rotation = glm::vec3(0, 0, 0);
    state.objects[1].acceleration = glm::vec3(0, 0, 0);
    state.objects[1].scale = glm::vec3(1000, 0.3f, 1000);
    state.objects[1].entityType = EntityType::FLOOR;

    GLuint marioTextureID = Util::LoadTexture("planet.jpg");

    Mesh* marioMesh = new Mesh();
    marioMesh->LoadOBJ("planet.obj", 1);


    state.objects[2].textureID = marioTextureID;
    state.objects[2].mesh = marioMesh;
    state.objects[2].position = glm::vec3(-2, 3, -5);
    state.objects[2].scale = glm::vec3(0.002f, 0.002f, 0.002f);
    state.objects[2].entityType = EntityType::PLANET;

    GLuint crateTextureID = Util::LoadTexture("box.png");
    Mesh* crateMesh = new Mesh();
    crateMesh->LoadOBJ("cube.obj", 1);

    state.objects[3].textureID = crateTextureID;
    state.objects[3].mesh = crateMesh;
    state.objects[3].position = glm::vec3(0, 0.5f, -3.0f);
    state.objects[3].entityType = EntityType::CRATE;

    state.objects[4].textureID = crateTextureID;
    state.objects[4].mesh = crateMesh;
    state.objects[4].position = glm::vec3(-3, 0.5f, -3.0f);
    state.objects[4].entityType = EntityType::CRATE;

    state.objects[5].textureID = crateTextureID;
    state.objects[5].mesh = crateMesh;
    state.objects[5].position = glm::vec3(-1, 0.5f, -3.0f);
    state.objects[5].entityType = EntityType::CRATE;

    state.objects[6].textureID = crateTextureID;
    state.objects[6].mesh = crateMesh;
    state.objects[6].position = glm::vec3(-2, 0.5f, -3.0f);
    state.objects[6].entityType = EntityType::CRATE;

    state.objects[7].textureID = crateTextureID;
    state.objects[7].mesh = crateMesh;
    state.objects[7].position = glm::vec3(-4, 0.5f, -3.0f);
    state.objects[7].entityType = EntityType::CRATE;

    state.objects[8].textureID = crateTextureID;
    state.objects[8].mesh = crateMesh;
    state.objects[8].position = glm::vec3(-5, 0.5f, -3.0f);
    state.objects[8].entityType = EntityType::CRATE;

    state.objects[9].textureID = crateTextureID;
    state.objects[9].mesh = crateMesh;
    state.objects[9].position = glm::vec3(-6, 0.5f, -3.0f);
    state.objects[9].entityType = EntityType::CRATE;

    state.objects[10].textureID = crateTextureID;
    state.objects[10].mesh = crateMesh;
    state.objects[10].position = glm::vec3(-7, 0.5f, -3.0f);
    state.objects[10].entityType = EntityType::CRATE;


    state.enemies = new Entity[ENEMY_COUNT];
    state.player->enemyLeft = ENEMY_COUNT;
    state.player->level = 1;

    GLuint enemyTextureID = Util::LoadTexture("alien2.png");
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].position = glm::vec3(rand() % 20 - 10, 0.5f, rand() % 20 - 10);
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
        state.enemies[i].entityType = EntityType::ENEMY_Billboard;
    }
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.objects, OBJECT_COUNT,state.enemies,ENEMY_COUNT);

    for (int i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
    }
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].valid) {
            state.enemies[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
        }
    }
    for (int i = 0; i < state.bullets.size(); i++) {
        if (state.bullets[i]->valid) {
            state.bullets[i]->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
        }
    }
}

void Level1::Render(ShaderProgram* program) {
    for (int i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].valid) {
            state.enemies[i].Render(program);
        }
    }
    for (int i = 0; i < state.bullets.size(); i++) {
        if (state.bullets[i]->valid) {
            state.bullets[i]->Render(program);
        }
        
    }
}