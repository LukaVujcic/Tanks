#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/game.h"
#include "core/engine.h"
#include "core/graphics/text.h"
using namespace dagger;

class ScoreSystem
    : public System
{
    void SpinUp() override;
    void WindDown() override;
    String SystemName() override;
    void Run() override;
private:
    Entity score1;
    Entity score2;
    Text text1;
    Text text2;
    Entity winner;
    Text textWinner;
};