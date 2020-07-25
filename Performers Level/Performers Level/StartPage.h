#include "Scene.h"
#include <SDL_mixer.h>
class StartPage : public Scene {

public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};