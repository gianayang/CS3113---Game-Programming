#include "Entity.h"
Entity::Entity()
{
	position = glm::vec3(0);
	speed = 0;
	movement = glm::vec3(0);
	acceleration = glm::vec3(0);
	velocity = glm::vec3(0);
	modelMatrix = glm::mat4(1.0f);
}


void Entity::AI(Entity *player) {
	switch (aiType) {
		case AIType::WALKER:
			AIWalker();
			break;
		case AIType::WAITANDGO:
			AIWaitAndGo(player);
			break;
	}
}
void Entity::AIWalker() {
	if (fabs(position.x) + 0.5f >= 5.0f || fabs(position.y) + 0.5f >= 3.75f) {
		speed = speed * -1;
	}
	movement = glm::vec3(-2, 0, 0);
}
void Entity::AIWaitAndGo(Entity *player){
	switch (aiState) {
		case AISTATE::IDLE:
			if (glm::distance(position, player->position) < 3.0f) {
				aiState = AISTATE::WALKING;
			}
			break;
		case AISTATE::WALKING:
			if (player->position.x < position.x) {
				movement = glm::vec3(-1, 0, 0);
			}
			else {
				movement = glm::vec3(1, 0, 0);
			}
			break;
		case AISTATE::ATTACKING:
			acceleration.y = 0.0f;
			if (fabs(position.y) +0.5f >= 3.0f) {
				acceleration.y = acceleration.y * -1;
			}
			else {
				movement = glm::vec3(0, 1, 0);
			}
			break;
	}
}
void Entity::Update(float deltaTime,Entity* player, Entity* platforms, int platformCount, Entity* enemies, int enemyCount)
{
	if (isActive == false) {
		return;
	}

	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	if (Entity::entityType == EntityType::ENEMY) {
		AI(player);
		if (glm::length(movement) > 1.0f) {
			movement = glm::normalize(movement);
		}
		velocity.x = movement.x * speed;
		velocity += acceleration * deltaTime;
		position.y += velocity.y * deltaTime; // Move on Y
		CheckCollisionsY(platforms, platformCount);// Fix if needed
		position.x += velocity.x * deltaTime; 
	}

	
	if (animIndices != NULL) {
		if (glm::length(movement) != 0) {
			animTime += deltaTime;
			if (animTime >= 0.25f)
			{
				animTime = 0.0f;
				animIndex++;
				if (animIndex >= animFrames)
				{
					animIndex = 0;
				}
			}
		}
		else {
			animIndex = 0;
		}
	}
	

	if (entityType == EntityType::PLAYER) {
		velocity.x = movement.x * speed;
		velocity += acceleration * deltaTime;

		position.y += velocity.y * deltaTime; // Move on Y
		CheckCollisionsY(platforms, platformCount);// Fix if needed
		CheckCollisionsY(enemies, enemyCount);

		if (lastCollided == EntityType::ENEMY) {
			if (collidedBottom == true) {
				for (int i = 0; i < enemyCount; i++) {
					if (!enemies[i].isActive) {
						enemyLeft--;
					}
				}
				if (enemyLeft <= 0) {
					gameEnd = true;
					win = true;
				}
				else {
					enemyLeft = enemyCount;
				}
			}
		}

		position.x += velocity.x * deltaTime; // Move on X
		CheckCollisionsX(enemies, enemyCount);
		if (lastCollided == EntityType::ENEMY) {
			if (collidedLeft == true || collidedRight == true) {
				gameEnd = true;
				win = false;
			}
		}
		if (jump == true) {
			jump = false;
			velocity.y += jumpPower;
		}
	}
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
}

bool Entity::CheckCollision(Entity* other) {
	if (isActive == false || other->isActive == false) {
		return false;
	}
	float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
	float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
	if (xdist < 0 && ydist < 0) return true;

	return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
	for (int i = 0; i < objectCount; i++)
	{
		Entity* object = &objects[i];
		lastCollided = object->entityType;
		if (CheckCollision(object))
		{
			float ydist = fabs(position.y - object->position.y);
			float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
			if (velocity.y > 0) {
				if (object->entityType == EntityType::ENEMY) {
					collidedTop = true;
				}
				else {
					position.y += penetrationY;
					velocity.y = 0;
				}
			}
			else if (velocity.y < 0) {
				if (object->entityType == EntityType::ENEMY) {
					object->isActive = false;
					collidedBottom = true;
				}
				else {
					position.y += penetrationY;
					velocity.y = 0;
				}
			}
		}
	}
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
	for (int i = 0; i < objectCount; i++)
	{
		Entity* object = &objects[i];
		lastCollided = object->entityType;
		if (CheckCollision(object))
		{
			float xdist = fabs(position.x - object->position.x);
			float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
			if (velocity.x > 0) {
				position.x += penetrationX;
				velocity.x = 0;
				collidedRight = true;
			}
			else if (velocity.x < 0) {
				position.x += penetrationX;
				velocity.x = 0;
				collidedLeft = true;
			}
		}
	}
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
	float u = (float)(index % animCols) / (float)animCols;
	float v = (float)(index / animCols) / (float)animRows;

	float width = 1.0f / (float)animCols;
	float height = 1.0f / (float)animRows;

	float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
	u, v + height, u + width, v, u, v };

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
	if (isActive == false) {
		return;
	}
	program->SetModelMatrix(modelMatrix);

	if (animIndices != NULL) {
		DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
		return;
	}

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}