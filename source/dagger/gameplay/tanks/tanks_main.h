#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/game.h"
#include "core/engine.h"

using namespace dagger;

namespace tanks
{
    void SetupWorld(Engine& engine_);

    class TanksGame : public Game
    {
        inline String GetIniFile() override
        {
            return "tanks.ini";
        };

        void CoreSystemsSetup(Engine& engine_) override;
        void GameplaySystemsSetup(Engine& engine_) override;
        void WorldSetup(Engine& engine_) override;
    };
}
