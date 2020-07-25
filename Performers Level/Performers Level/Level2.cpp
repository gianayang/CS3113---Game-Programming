#include "Level2.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 3, 1,
 3, 1, 1, 1, 1, 0, 1, 0, 3, 3, 3, 2, 2, 2,
 3, 2, 2, 2, 2, 0, 2, 0, 3, 3, 3, 2, 2, 2
};

Mix_Music* music_level2;
Mix_Chunk* bounce_level2;

void Level2::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music_level2 = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music_level2, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1,MIX_MAX_VOLUME / 4);

    bounce_level2 = Mix_LoadWAV("bounce.wav");

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

}
void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    if (state.player->collidedBottom) {
        Mix_PlayChannel(-1, bounce_level2, 0);
    }
}
void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}