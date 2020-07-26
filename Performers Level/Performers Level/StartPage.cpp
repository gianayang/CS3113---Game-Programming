#include "StartPage.h"
#define StartPage_WIDTH 14
#define StartPage_HEIGHT 8
#define StartPage_ENEMY_COUNT 0


void StartPage::Initialize() {

    state.nextScene = -1;

    state.player = new Entity();
    state.player->position = glm::vec3(0, 0, 0);
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
void StartPage::Update(float deltaTime) {
   
}
void StartPage::Render(ShaderProgram* program) {
}