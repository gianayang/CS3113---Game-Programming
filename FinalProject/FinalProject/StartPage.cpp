#include "StartPage.h"
#define StartPage_WIDTH 14
#define StartPage_HEIGHT 8
#define StartPage_ENEMY_COUNT 0


void StartPage::Initialize() {
    state.nextScene = -1;

    state.player = new Entity();
    state.player->position = glm::vec3(0, 0, 0);
    state.player->entityType = EntityType::PLAYER;
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
    state.player->velocity = glm::vec3(0, 0, 0);
    state.player->rotation = glm::vec3(0, 0, 0);


    state.player->height = 0.85f;
    state.player->width = 0.55f;
}
void StartPage::Update(float deltaTime) {

}
void StartPage::Render(ShaderProgram* program) {
}