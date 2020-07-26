#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

Mix_Music* music;
GLuint fontTextureID;
void Level1::Initialize() {

    state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
	// Move over all of the player and enemy code from initialization.

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
   
    fontTextureID = Util::LoadTexture("font.png");

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

    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("cat.png");
    /*
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4.0f, 0.0f, 0.0f);
    state.enemies[0].movement = glm::vec3(0, 0, 0);
    state.enemies[0].entityType = EntityType::ENEMY;
    state.enemies[0].aiType = AIType::WALKER;
    state.enemies[0].aiState = AISTATE::WALKING;
    state.enemies[0].speed = 2.0f;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].width = 0.6;
    state.enemies[0].height = 0.8;
*/
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(2.0f, 5.0f, 0.0f);
    state.enemies[0].movement = glm::vec3(0, 0, 0);
    state.enemies[0].entityType = EntityType::ENEMY;
    state.enemies[0].aiType = AIType::WAITANDGO;
    state.enemies[0].aiState = AISTATE::IDLE;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].speed = 2.0;
    state.enemies[0].width = 0.6;
    state.enemies[0].height = 0.8;
    /*
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
    */
}
void Level1::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    if (state.player->position.x >= 12) {
        state.nextScene = 2;
    }
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime , state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }
}
void Level1::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}