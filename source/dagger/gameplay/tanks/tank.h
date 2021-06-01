#pragma once

#ifndef TANK_H
#define TANK_H

inline int tank1_num_bullets = 0;
inline int tank2_num_bullets = 0;

struct Tank
{
	Float32 speed{0.0};
	Float32 acceleration{1.0};
	Float32 maxSpeed{60.0};
	Float32 minSpeed{-60.0};
	Float32 direction{0};
	Float32 rotationSpeed{60.0};
	Float32 angle;
	Float32 driftAngle{0};
	Vector3 pos{0,0,0}; // last good position before collision
	Float32 last_angle{0};
    int health{100};
	String description{""};
};

#endif