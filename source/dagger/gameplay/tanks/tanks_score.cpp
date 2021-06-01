#include "tank_movement.h"
#include "tank_bullet.h"
#include "core/graphics/text.h"
#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "tank.h"
#include "tanks_score.h"
#include <iostream>
using namespace std;

#define BLOCK_SIZE 42

#include <cmath>
using namespace dagger;
using namespace tanks;

void ScoreSystem::SpinUp()
{
    auto& reg = Engine::Registry();
    score1 = reg.create();
    text1 = reg.emplace<Text>(score1);
   
    text1.spacing = 0.6f;
    score2 = reg.create();
    text2 = reg.emplace<Text>(score2);
    text2.spacing = 0.6f;

    winner = reg.create();
    textWinner = reg.emplace<Text>(winner);
    textWinner.spacing = 1.5f;

}

void ScoreSystem::WindDown()
{
    Engine::Registry().destroy(score1);
    Engine::Registry().destroy(score2);
}
String ScoreSystem::SystemName()
{
    return "ScoreSystem";
}
void ScoreSystem::Run() 
{
    bool tank1Exists = false;
    bool tank2Exists = false;
    {
        auto view = Engine::Registry().view<Tank>();
        for (auto entity : view)
        {
            auto& t = view.get<Tank>(entity);
            if (t.description.compare("tank1") == 0)
            {
                tank1Exists = true;
                String health = std::to_string(t.health);
                text1.Set("pixel-font", "Blue hp: " + health, { (-6) * BLOCK_SIZE, 9 * BLOCK_SIZE + 2,0 });
            }
        }
    }
    {
        auto view = Engine::Registry().view<Tank>();
        for (auto entity : view)
        {
            auto& t = view.get<Tank>(entity);
            if (t.description.compare("tank2") == 0)
            {
                tank2Exists = true;
                String health = std::to_string(t.health);
                text2.Set("pixel-font", "Red hp: " + health, { (7) * BLOCK_SIZE, 9 * BLOCK_SIZE + 2,0 });
            }
        }
    }
    if (!tank1Exists)
    {
        auto& reg = Engine::Registry();
        textWinner.Set("pixel-font", "Red won!");
        text1.Set("pixel-font", "Blue hp: 0", { (-6) * BLOCK_SIZE, 9 * BLOCK_SIZE + 2,0 });
        dagger::Engine::ToggleSystemsPause(true);
        
    }
    if (!tank2Exists)
    {
        auto& reg = Engine::Registry();
        textWinner.Set("pixel-font", "Blue won!");
        text2.Set("pixel-font", "Red hp: 0", { (7) * BLOCK_SIZE, 9 * BLOCK_SIZE + 2,0 });
        dagger::Engine::ToggleSystemsPause(true);
    }
}
