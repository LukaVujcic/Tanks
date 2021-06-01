#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "core/graphics/text.h"
#include "tools/diagnostics.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/common/particles.h"
#include "tanks_main.h"
#include "tank.h"
#include "tank_movement.h"
#include "tilemap.h"
#include "tank_bullet.h"
#include "tanks_score.h"
#include <string> 
#define BLOCK_SIZE 42

using namespace dagger;
using namespace tanks;


void TanksGame::CoreSystemsSetup(Engine& engine_)
{
    engine_.AddSystem<WindowSystem>();
    engine_.AddSystem<InputSystem>();
    engine_.AddSystem<ShaderSystem>();
    engine_.AddSystem<TextureSystem>();
    engine_.AddSystem<SpriteRenderSystem>();
    engine_.AddPausableSystem<TransformSystem>();
    engine_.AddPausableSystem<AnimationSystem>();
#if !defined(NDEBUG)
    engine_.AddSystem<DiagnosticSystem>();
    engine_.AddSystem<GUISystem>();
    engine_.AddSystem<ToolMenuSystem>();
   
#endif //!defined(NDEBUG)
}

void TanksGame::GameplaySystemsSetup(Engine& engine_)
{

    engine_.AddPausableSystem<SimpleCollisionsSystem>();
    engine_.AddSystem<common_res::ParticleSystem>();
    engine_.AddSystem<TilemapSystem>();
    engine_.AddPausableSystem<TankMovement>();
    engine_.AddPausableSystem<TankBulletSystem>();
    engine_.AddSystem<ScoreSystem>();
    
#if defined(DAGGER_DEBUG)
    
#endif //defined(DAGGER_DEBUG)
}

void TanksGame::WorldSetup(Engine& engine_)
{
   
    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 900, 900 };
    camera->zoom = 1;
    camera->position = { 420, 410, 0 };
    camera->Update();

    SetupWorld(engine_);
}

Entity CreateFloor(Registry& reg_, UInt32 x_, UInt32 y_)
{
    Entity entity = reg_.create();
    auto& sprite = reg_.emplace<Sprite>(entity);
    sprite.position = { x_ * BLOCK_SIZE, y_ * BLOCK_SIZE, 90 };
   
    AssignSprite(sprite, "floor");
    sprite.size = { BLOCK_SIZE, BLOCK_SIZE };
    return entity;
}

Entity CreateOuterWall(Registry& reg_, UInt32 x_, UInt32 y_)
{
    Entity entity = reg_.create();
    auto& sprite = reg_.emplace<Sprite>(entity);
    
    auto& transform = reg_.emplace<Transform>(entity);
    transform.position = { x_ * BLOCK_SIZE, y_ * BLOCK_SIZE, 90 };
    
    AssignSprite(sprite, "outerWall");
    sprite.size = { BLOCK_SIZE, BLOCK_SIZE };
    
    auto& col = reg_.emplace<SimpleCollision>(entity);
    col.size = sprite.size;
    
    return entity;

}
Entity CreateInnerWall(Registry& reg_, UInt32 x_, UInt32 y_)
{
    Entity entity = reg_.create();
    auto& sprite = reg_.emplace<Sprite>(entity);
    
    auto& transform = reg_.emplace<Transform>(entity);
    transform.position = { x_ * BLOCK_SIZE, y_ * BLOCK_SIZE, 90 };

    AssignSprite(sprite, "innerWall");
    sprite.size = { BLOCK_SIZE, BLOCK_SIZE };
    
    auto& col = reg_.emplace<SimpleCollision>(entity);
    col.size = sprite.size;
    
    return entity;

}
Entity CreateStartPlatform(Registry& reg_, UInt32 x_, UInt32 y_)
{
    Entity entity = reg_.create();
    auto& sprite = reg_.emplace<Sprite>(entity);
    
    auto& transform = reg_.emplace<Transform>(entity);
    transform.position = { x_ * BLOCK_SIZE, y_ * BLOCK_SIZE, 90 };

    AssignSprite(sprite, "startPlatform");
    sprite.size = { BLOCK_SIZE, BLOCK_SIZE };
    
    return entity;

}
    
void tanks::SetupWorld(Engine& engine_)
{
    TilemapLegend legend;
    legend['#'] = &CreateOuterWall;
    legend['.'] = &CreateFloor;
    legend['*'] = &CreateStartPlatform;
    legend['$'] = &CreateInnerWall;
    Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ "my-file.map.txt", &legend });

	auto& reg = Engine::Registry();
	
    // player 1
    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "tank1");
        sprite.size = { BLOCK_SIZE, BLOCK_SIZE};
        
        auto& transform = reg.emplace<Transform>(entity);
        transform.position = { 2 * BLOCK_SIZE, 3 * BLOCK_SIZE, 0 };
        
        auto& tank = reg.emplace<Tank>(entity);
        tank.angle = 90.0f;
        tank.description = "tank1";

        auto& controller = reg.emplace<ControllerMapping>(entity);
        TankMovement::SetupPlayerOneMovement(controller);
        
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size = sprite.size;
    }
    
    // player 2
    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "tank2");
        sprite.size = { BLOCK_SIZE, BLOCK_SIZE};
        
        auto& transform = reg.emplace<Transform>(entity);
        transform.position = { 18 * BLOCK_SIZE, 16 * BLOCK_SIZE, 0 };
        
        auto& tank = reg.emplace<Tank>(entity);
        tank.angle = -90.0f;
        tank.description = "tank2";
        auto& controller = reg.emplace<ControllerMapping>(entity);
        TankMovement::SetupPlayerTwoMovement(controller);
        
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size = sprite.size;
    }
}
