#include "Level3.h"
#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8
#define LEVEL3_ENEMY_COUNT 3

unsigned int level3_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
 3, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 2, 2, 2,
 3, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2
};

Mix_Music* music_level3;
Mix_Chunk* bounce_level3;

void Level3::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music_level3 = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music_level3, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);

    bounce_level3 = Mix_LoadWAV("bounce.wav");

    state.player = new Entity();
    state.player->position = glm::vec3(5, 0, 0);
    state.player->entityType = EntityType::PLAYER;
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
    state.player->jumpPower = 6.0f;
    state.player->textureID = Util::LoadTexture("george.png");

    state.player->height = 0.85f;
    state.player->width = 0.55f;

    state.player->animRight = new int[4]{ 8, 9,10,11 };
    state.player->animLeft = new int[4]{ 4, 5, 6, 7 };
    state.player->animUp = new int[4]{ 0, 1, 2, 3 };
    state.player->animDown = new int[4]{ 12, 13,14,15 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("cat.png");

    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(11.0f, 5.0f, 0.0f);
    state.enemies[0].movement = glm::vec3(0, 0, 0);
    state.enemies[0].entityType = EntityType::ENEMY;
    state.enemies[0].aiType = AIType::WALKER;
    state.enemies[0].aiState = AISTATE::WALKING;
    state.enemies[0].speed = 2.0f;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].width = 0.6;
    state.enemies[0].height = 0.8;

    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(2.0, 5.0, 0);
    state.enemies[1].movement = glm::vec3(0, 0, 0);
    state.enemies[1].entityType = EntityType::ENEMY;
    state.enemies[1].aiType = AIType::WAITANDGO;
    state.enemies[1].aiState = AISTATE::IDLE;
    state.enemies[1].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[1].speed = 2.0;
    state.enemies[1].width = 0.6;
    state.enemies[1].height = 0.8;
   
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(4.0f, 1.0f, 0.0f);
    state.enemies[2].movement = glm::vec3(1, 1, 0);
    state.enemies[2].entityType = EntityType::ENEMY;
    state.enemies[2].aiType = AIType::WALKER;
    state.enemies[2].aiState = AISTATE::WALKING;
    state.enemies[2].speed = 2.0f;
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].width = 0.6;
    state.enemies[2].height = 0.8;

    state.player->enemyLeft = 3;
    state.player->level = 3;
}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }
}
void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}