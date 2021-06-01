#include "tank_movement.h"
#include "tank_bullet.h"

#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/common/particles.h"
#include "tank.h"

#include <iostream>
using namespace std;

#include <cmath>

using namespace dagger;
using namespace tanks;

void TankMovement::SpinUp()
{
    Engine::Dispatcher().sink<KeyboardEvent>().connect<&TankMovement::OnKeyboardEvent>(this);
}

void TankMovement::WindDown()
{
    Engine::Dispatcher().sink<KeyboardEvent>().disconnect<&TankMovement::OnKeyboardEvent>(this);
}

void CreateTankBullet(float tileSize_, bool spec, String desc, int dmg, Vector3 speed_, Vector3 pos_)
{
    auto& reg = Engine::Registry();
    auto entity = reg.create();
    auto& sprite = reg.emplace<Sprite>(entity);
    if (spec)
    	AssignSprite(sprite, "special_bullet");
    else
    	AssignSprite(sprite, "bullet");
    sprite.size = Vector2(1, 1) * tileSize_;

    auto& transform = reg.emplace<Transform>(entity);
    transform.position = pos_;
    transform.position.z = pos_.z;
    auto& ball = reg.emplace<TankBullet>(entity);
    ball.speed = speed_ * tileSize_;
    ball.tank = desc;
    ball.damage = dmg;
    ball.special_bullet = spec;

    auto& col = reg.emplace<SimpleCollision>(entity);
    col.size.x = tileSize_;
    col.size.y = tileSize_;
    
    common_res::ParticleSpawnerSettings settings;
    settings.Setup(0.02f, {1.5f, 1.5f}, {-0.3f, -0.3f}, {0.3f, 0.3f});
    common_res::ParticleSystem::SetupParticleSystem(entity, settings);
}

void TankMovement::OnKeyboardEvent(KeyboardEvent kEvent_)
{
    Engine::Registry().view<ControllerMapping>().each([&](ControllerMapping& ctrl_)
        {

            if (kEvent_.key == ctrl_.left_key && (kEvent_.action == EDaggerInputState::Pressed || kEvent_.action == EDaggerInputState::Held))
            {
                ctrl_.rotation = 1;
            }
            else if (kEvent_.key == ctrl_.left_key && kEvent_.action == EDaggerInputState::Released && ctrl_.rotation > 0)
            {
                ctrl_.rotation = 0;
            }
            else if (kEvent_.key == ctrl_.right_key && (kEvent_.action == EDaggerInputState::Held || kEvent_.action == EDaggerInputState::Pressed))
            {
                ctrl_.rotation = -1;
            }
            else if (kEvent_.key == ctrl_.right_key && kEvent_.action == EDaggerInputState::Released && ctrl_.rotation < 0)
            {
                ctrl_.rotation = 0;
            }

            if (kEvent_.key == ctrl_.fire_key && (kEvent_.action == EDaggerInputState::Held || kEvent_.action == EDaggerInputState::Pressed)) {
                ctrl_.fire = 1;
            }

            if (kEvent_.key == ctrl_.special_fire_key && (kEvent_.action == EDaggerInputState::Held || kEvent_.action == EDaggerInputState::Pressed)) {
                ctrl_.special_fire = 1;
            }

            if (kEvent_.key == ctrl_.up_key && (kEvent_.action == EDaggerInputState::Pressed || kEvent_.action == EDaggerInputState::Held))
            {
                ctrl_.move = 1;
            }
            else if (kEvent_.key == ctrl_.up_key && kEvent_.action == EDaggerInputState::Released && ctrl_.move > 0)
            {
                ctrl_.move = 0;
            }
            else if (kEvent_.key == ctrl_.down_key && (kEvent_.action == EDaggerInputState::Held || kEvent_.action == EDaggerInputState::Pressed))
            {
                ctrl_.move = -1;
            }
            else if (kEvent_.key == ctrl_.down_key && kEvent_.action == EDaggerInputState::Released && ctrl_.move < 0)
            {
                ctrl_.move = 0;
            }
        });
}

void TankMovement::Run()
{
    auto view = Engine::Registry().view<Transform, ControllerMapping, Tank, Sprite, SimpleCollision>();
    for (auto entity : view)
    {
        auto &t = view.get<Transform>(entity);
        auto &ctrl = view.get<ControllerMapping>(entity);
        auto &tank = view.get<Tank>(entity);
        auto &s = view.get<Sprite>(entity);
        auto &col = view.get<SimpleCollision>(entity);
        Float32 rad;
        
        if (ctrl.move){
        	if (ctrl.move == 1.0 && tank.speed > 0.0 || ctrl.move == -1.0 && tank.speed < 0.0){
        		tank.rotationSpeed = 45.0;
        		tank.driftAngle = tank.angle;
        		tank.direction = ctrl.move;
        		tank.speed = tank.speed + ctrl.move * tank.acceleration * Engine::DeltaTime();
        	}
        	else{
        		tank.rotationSpeed = 60.0;
        		tank.speed = tank.speed + 1.5 * ctrl.move * tank.acceleration * Engine::DeltaTime();
        	}
        	if (tank.speed > tank.maxSpeed)
        		tank.speed = tank.maxSpeed;
        	if (tank.speed < tank.minSpeed)
        		tank.speed = tank.minSpeed;
        }
        else {
        	tank.rotationSpeed = 60.0;
        	tank.driftAngle = tank.angle;
        	tank.speed = tank.speed - 1.2 * tank.direction * tank.acceleration * Engine::DeltaTime();
        	if (tank.direction == 1.0 && tank.speed < 0.0 || tank.direction == -1.0 && tank.speed > 0.0){
        		tank.direction = 0.0;
        		tank.speed = 0.0;
        	}
        }

        if (col.colided)
        {
        	auto viewBullet = Engine::Registry().view<TankBullet, Transform, SimpleCollision>();

            bool tankDestroyed = false;

            for (auto entityBullet : viewBullet){
            
            	auto& ball = viewBullet.get<TankBullet>(entityBullet);
            	
                if (col.colidedWith == entityBullet){
                    tank.health -= ball.damage;   
                }
                if(tank.health <= 0 && Engine::Registry().valid(entity)){
                    Engine::Registry().destroy(entity);
                    tankDestroyed = true; 
                }
            }
            
            if(!tankDestroyed){
            	
            	tank.angle = tank.last_angle;
            	s.rotation = {-90.0f + tank.angle};
        		rad = tank.angle * PI / 180.0f;
        		col.angle = rad;
        		
        		tank.speed = 0.0;
        		tank.direction = 0.0;
                t.position = tank.pos;
                col.colided = false;
            }
        }
        else 
        {
        	tank.last_angle = tank.angle;
        	tank.angle += ctrl.rotation * Engine::DeltaTime() * tank.rotationSpeed;
        	s.rotation = {-90.0f + tank.angle};
        	rad = tank.angle * PI / 180.0f;
        	col.angle = rad;
        	
        	tank.pos = t.position;
        	if (ctrl.move == 0 || ctrl.move == tank.direction){
        		t.position.x += cos(rad) * tank.speed;
        		t.position.y += sin(rad) * tank.speed;
        	}
        	else{
        		t.position.x += cos(tank.driftAngle * PI / 180.0f) * tank.speed;
        		t.position.y += sin(tank.driftAngle * PI / 180.0f) * tank.speed;
        	}
        }
        
        if (ctrl.fire || ctrl.special_fire) {
        
        	bool spec = true;
        	int dmg = 15;
        	if (ctrl.fire){
            	ctrl.fire = 0;
            	spec = false;
            	dmg = 25;
            }
            else
            	ctrl.special_fire = 0;
            
            if(tank.description == "tank1" && tank1_num_bullets < 1){
                tank1_num_bullets += 1;
                CreateTankBullet(20, spec, tank.description, dmg,
		            {sin(-s.rotation * PI / 180.0f) * 30, cos(-s.rotation * PI / 180.0f) * 30, 0},
					{tank.pos.x + 42 * cos(rad), tank.pos.y + 42 * sin(rad), 0 });
			}
            if(tank.description == "tank2" && tank2_num_bullets < 1){
                tank2_num_bullets += 1;
                CreateTankBullet(20, spec, tank.description, dmg,
		            {sin(-s.rotation * PI / 180.0f) * 30, cos(-s.rotation * PI / 180.0f) * 30, 0},
					{tank.pos.x + 42 * cos(rad), tank.pos.y + 42 * sin(rad), 0 });
			}
        }
    }
}
