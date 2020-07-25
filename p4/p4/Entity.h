#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum class EntityType { PLAYER, PLATFORM, ENEMY};
enum class AIType {WALKER,WAITANDGO};
enum class AISTATE{IDLE, WALKING,ATTACKING};

class Entity {	
public:
	EntityType lastCollided = EntityType::PLAYER;
	EntityType entityType;
	AIType aiType;
	AISTATE aiState;

	glm::vec3 position;
	glm::vec3 movement;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float speed;

	float width = 1;
	float height = 1;

	bool jump = false;
	float jumpPower = 0;

	GLuint textureID;

	glm::mat4 modelMatrix;

	int* animRight = NULL;
	int* animLeft = NULL;
	int* animUp = NULL;
	int* animDown = NULL;

	int* animIndices = NULL;
	int animFrames = 4;
	int animIndex = 0;
	float animTime = 0;

	int animCols = 4;
	int animRows = 4;

	bool gameEnd = false;
	bool win = false;
	int enemyLeft = 3;

	bool isActive = true;

	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;

	Entity();
	bool CheckCollision(Entity* other);
	void CheckCollisionsY(Entity* objects, int objectCount);
	void CheckCollisionsX(Entity* objects, int objectCount);
	void Update(float deltaTime, Entity* platforms, Entity* player,int platformCount, Entity* enemies, int enemyCount);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index); \
	void AI(Entity *player);
	void AIWalker();
	void AIWaitAndGo(Entity* player);
};