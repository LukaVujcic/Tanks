#include "simple_collisions.h"

#include "core/engine.h"
#include "core/game/transforms.h"
#include <cmath>
#include <limits>
#include <iostream>

using namespace dagger;

void SimpleCollisionsSystem::Run()
{
    auto view = Engine::Registry().view<SimpleCollision, Transform>();

    auto it = view.begin();
    while(it != view.end())
    {
        auto &collision = view.get<SimpleCollision>(*it);
        auto &transform = view.get<Transform>(*it);

        auto it2 = it;
        it2++;
        while(it2 != view.end())
        {
            auto &col = view.get<SimpleCollision>(*it2);
            auto &tr = view.get<Transform>(*it2);

            // processing one collision per frame for each colider
            if (collision.IsCollided(transform.position, col, tr.position))
            {
                collision.colided = true;
                collision.colidedWith = *it2;

                col.colided = true;
                col.colidedWith = *it;
            }
            it2++;
        }
        it++;
    }
}

// Collision for rotated rectangles

bool SimpleCollision::IsCollided(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_)
{
	// if one rectangle is rotated calculate collision for rotated rectangle
	if (angle != 0.0 || other_.angle != 0.0){
	
		Vector2 p(pivot.x * size.x, pivot.y * size.y);
		Vector2 p2(other_.pivot.x * other_.size.x, other_.pivot.y * other_.size.y);
		
		Float32 sin1 = sin(angle), cos1 = cos(angle);
		Float32 sin2 = sin(other_.angle), cos2 = cos(other_.angle);
		
		// claculate rectangle point coordinates
		
		Float32 a[4][2] = {{p.x * cos1 - p.y * sin1 + pos_.x, 
							p.x * sin1 + p.y * cos1 + pos_.y},
						  {(p.x+size.x) * cos1 - p.y * sin1 + pos_.x, 
						   (p.x+size.x) * sin1 + p.y * cos1 + pos_.y},
						  {(p.x+size.x) * cos1 - (p.y+size.y) * sin1 + pos_.x, 
						   (p.x+size.x) * sin1 + (p.y+size.y) * cos1 + pos_.y},
						  {p.x * cos1 - (p.y+size.y) * sin1 + pos_.x, 
						   p.x * sin1 + (p.y+size.y) * cos1 + pos_.y}};
						   
		Float32 b[4][2] = {{p2.x * cos2 - p2.y * sin2 + posOther_.x, 
							p2.x * sin2 + p2.y * cos2 + posOther_.y},
						  {(p2.x+other_.size.x) * cos2 - p2.y * sin2 + posOther_.x, 
						   (p2.x+other_.size.x) * sin2 + p2.y * cos2 + posOther_.y},
						  {(p2.x+other_.size.x) * cos2 - (p2.y+other_.size.y) * sin2 + posOther_.x, 
						   (p2.x+other_.size.x) * sin2 + (p2.y+other_.size.y) * cos2 + posOther_.y},
						  {p2.x * cos2 - (p2.y+other_.size.y) * sin2 + posOther_.x, 
						   p2.x * sin2 + (p2.y+other_.size.y) * cos2 + posOther_.y}};
		
		// check if rectangles are collided by projecting rectangles on normal vector for each edge of both rectangles
		// and the checking interesection for projected rectangles
		
		for (auto rect : {a, b}){
			for (int i1=0; i1<4; i1++){
				
				int i2 = (i1+1)%4;
				Vector2 normal(rect[i2][1]-rect[i1][1], rect[i1][0]-rect[i2][0]);
				
				Float32 minA = std::numeric_limits<float>::max();
				Float32 maxA = std::numeric_limits<float>::min();
				for (int j=0; j<4; j++){
					Float32 projected = normal.x*a[j][0] + normal.y*a[j][1];
					if (projected < minA)
						minA = projected;
					if (projected > maxA)
						maxA = projected;
				}
				
				Float32 minB = std::numeric_limits<float>::max();
				Float32 maxB = std::numeric_limits<float>::min();
				for (int j=0; j<4; j++){
					Float32 projected = normal.x*b[j][0] + normal.y*b[j][1];
					if (projected < minB)
						minB = projected;
					if (projected > maxB)
						maxB = projected;
				}
				if (maxA < minB || maxB < minA)
		            return false;
			}
		}
		return true;
    }
    // if both rectangles are not rotated calculate simple collision
    else {
    	
    	Vector2 p(pos_.x + pivot.x * size.x, pos_.y + pivot.y * size.y);
		Vector2 p2(posOther_.x + other_.pivot.x * other_.size.x, posOther_.y + other_.pivot.y * other_.size.y);

		if (p.x < p2.x + other_.size.x &&
		    p.x + size.x > p2.x &&
		    p.y < p2.y + other_.size.y &&
		    p.y + size.y > p2.y)
		{
		    return true;
		}

		return false;
    }
}

Vector2 SimpleCollision::GetCollisionSides(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_)
{
    Vector2 res(0, 0);

    Vector2 p(pos_.x + pivot.x * size.x, pos_.y + pivot.y * size.y);
    Vector2 p2(posOther_.x + other_.pivot.x * other_.size.x, posOther_.y + other_.pivot.y * other_.size.y);

    Float32 xDif = std::min(std::abs(p.x + size.x - p2.x), std::abs(p2.x + other_.size.x - p.x));
    Float32 yDif = std::min(std::abs(p.y + size.y - p2.y), std::abs(p2.y + other_.size.y - p.y));

    if (xDif < yDif)
    {
        res.x = std::abs(p.x + size.x - p2.x) < std::abs(p2.x + other_.size.x - p.x) ? 1 : -1;
    }
    else
    {
        res.y = std::abs(p.y + size.y - p2.y) < std::abs(p2.y + other_.size.y - p.y) ? 1 : -1;
    }

    return res;
}

Vector3 SimpleCollision::GetCollisionCenter(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_)
{
    Vector3 res;

    Vector2 p(pos_.x + pivot.x * size.x, pos_.y + pivot.y * size.y);
    Vector2 p2(posOther_.x + other_.pivot.x * other_.size.x, posOther_.y + other_.pivot.y * other_.size.y);

    res.x = (p.x + size.x / 2.f + p2.x + other_.size.x) / 2.f;
    res.y = (p.y + size.y / 2.f + p2.y + other_.size.y) / 2.f;
    res.z = 1.f;

    return res;
}
