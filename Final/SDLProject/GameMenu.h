#include "Scene.h"

class GameMenu : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
    void Fire() override;
};
