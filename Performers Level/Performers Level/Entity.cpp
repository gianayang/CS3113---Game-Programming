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


void Entity::AI(Entity* player) {
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
void Entity::AIWaitAndGo(Entity* player) {
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
		if (fabs(position.y) + 0.5f >= 3.0f) {
			acceleration.y = acceleration.y * -1;
		}
		else {
			movement = glm::vec3(0, 1, 0);
		}
		break;
	}
}
void Entity::Update(float deltaTime, Entity* player, Entity *objects, int objectCount, Map* map)
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
		CheckCollisionsY(map);// Fix if needed
		position.x += velocity.x * deltaTime;
		CheckCollisionsX(map);// Fix if needed
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
		lastCollided = EntityType::PLAYER;
		velocity.x = movement.x * speed;
		velocity += acceleration * deltaTime;

		position.y += velocity.y * deltaTime; // Move on Y
		CheckCollisionsY(map);
		if (objects != NULL) {
			CheckCollisionsY(objects, objectCount);
		}
		if (position.y <= -10.0f) {
			gameEnd = true;
		}
		if (lastCollided == EntityType::ENEMY) {
			if (collidedBottom == true) {
				for (int i = 0; i < objectCount; i++) {
					if (!objects[i].isActive) {
						enemyLeft--;
					}
				}
				if (enemyLeft <= 0) {
					win = true;
				}
				else {
					enemyLeft = objectCount;
				}
			}
		}

		position.x += velocity.x * deltaTime; // Move on X
		CheckCollisionsX(map);
		if (objects != NULL) {
			CheckCollisionsX(objects, objectCount);
		}

		if (lastCollided == EntityType::ENEMY ) {
			if (collidedLeft == true || collidedRight == true) {
				gameEnd = true;
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
 	if (isActive == false ) {
		return false;
	}
	if (other->entityType == EntityType::ENEMY && other->isActive == false) {
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
		if (CheckCollision(object))
		{
			lastCollided = object->entityType;
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
		if (CheckCollision(object))
		{
			lastCollided = object->entityType;
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

void Entity::CheckCollisionsY(Map* map)
{
	// Probes for tiles
	glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
	glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
	glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

	glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
	glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
	glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

	float penetration_x = 0;
	float penetration_y = 0;
	if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
		position.y -= penetration_y;
		velocity.y = 0;
		collidedTop = true;
	}
	else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
		position.y -= penetration_y;
		velocity.y = 0;
		collidedTop = true;
	}
	else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
		position.y -= penetration_y;
		velocity.y = 0;
		collidedTop = true;
	}
	if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
		position.y += penetration_y;
		velocity.y = 0;
		collidedBottom = true;
	}
	else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
		position.y += penetration_y;
		velocity.y = 0;
		collidedBottom = true;
	}
	else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
		position.y += penetration_y;
		velocity.y = 0;
		collidedBottom = true;
	}
}

void Entity::CheckCollisionsX(Map* map)
{
	// Probes for tiles
	glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
	glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

	float penetration_x = 0;
	float penetration_y = 0;
	if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
		position.x += penetration_x;
		velocity.x = 0;
		collidedLeft = true;
	}

	if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
		position.x -= penetration_x;
		velocity.x = 0;
		collidedRight = true;
	}
}