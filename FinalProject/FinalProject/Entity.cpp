#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    rotation = glm::vec3(0);
    scale = glm::vec3(1);

    modelMatrix = glm::mat4(1.0f);

    speed = 0.0f;

    billboard = false;
    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Volume(-1, MIX_MAX_VOLUME);
    attack = Mix_LoadWAV("gun.wav");
    dead = Mix_LoadWAV("Monster.wav");
}

bool Entity::CheckCollision(Entity* other)
{
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) return true;

    return false;
}


void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* enemies, int enemyCount)
{
    glm::vec3 previousPosition = position;

    if (billboard) {
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));
    }

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    if (entityType == EntityType::PLAYER) {
        for (int i = 0; i < objectCount; i++)
        {
            // Ignore collisions with the floor
            if (objects[i].entityType == EntityType::FLOOR) continue;

            if (CheckCollision(&objects[i])) {
                position = previousPosition;
                break;
            }
        }
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].valid && CheckCollision(&enemies[i])) {
                Mix_PlayChannel(-1, dead, 0);
                gameEnd = true;
            }
            if (enemies[i].valid == false) {
                enemyLeft--;
            }
        }
        if (enemyLeft <= 0) {
            win = true;
        }
        else {
            enemyLeft = enemyCount;
        }
    }


    if (entityType == EntityType::BULLET && valid) {
        for (int i = 0; i < enemyCount; i++) {
            if (CheckCollision(&enemies[i])) {
                Mix_PlayChannel(-1, attack, 0);
                enemies[i].valid = false;
                valid = false;
            }
        }
    }

    else if (entityType == EntityType::PLANET) {
        rotation.z += 90 * deltaTime;
        rotation.y += 30 * deltaTime;
    }
    else if (entityType == EntityType::ENEMY) {
        velocity.z = cos(glm::radians(rotation.y)) * -1.0f;
        velocity.x = sin(glm::radians(rotation.y)) * -1.0f;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Entity::DrawBillboard(ShaderProgram* program) {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
    program->SetModelMatrix(modelMatrix);

    glBindTexture(GL_TEXTURE_2D, textureID);

    if (billboard) {
        DrawBillboard(program);
    }
    else {
        mesh->Render(program);
    }
}

