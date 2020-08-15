#include "Level2.h"
#define OBJECT_COUNT 3
#define ENEMY_COUNT 10

Mix_Music* music_level2;

void Level2::Initialize() {

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music_level2 = Mix_LoadMUS("horror.mp3");
    Mix_PlayMusic(music_level2, -1);

    state.player = new Entity();
    state.player->entityType = EntityType::PLAYER;
    state.player->position = glm::vec3(-50.0, 0.25f, -10.0);
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

    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("alien2.png");
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].entityType = EntityType::ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].position = glm::vec3(rand() % 20 - 10, 0.5f, rand() % 20 - 10);
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
    }

    state.player->level = 2;
    state.player->enemyLeft = ENEMY_COUNT;
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);

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

void Level2::Render(ShaderProgram* program) {
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