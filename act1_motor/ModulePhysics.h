#pragma once
#include "Module.h"
#include "Application.h"
#include "Globals.h"
#include "ModulePlayer.h"
#include "p2List.h"

#define MIN_ANGLE -(3.1/2) + 0.2
#define MAX_ANGLE 0

#define MAX_VEL 50
#define MIN_VEL 15

#define GRAVITY 1.0

enum integrator
{
	EULER_BACK,
	EULER_FOR,
	VELOCITY_VERLET
};

struct Ball
{
	int x, y, rad;

	double mass = 10, 
		acc = 0, accX = 0, accY = 0,
		vel = 0, velX = 0, velY = 0,
		fX = 0, fY = 0;

	//Aerodynamics Stuff
	double surface = 0; //Effective wet surface
	double cl = 0; // Lift coefficient
	double cd = 0; // Drag coefficient

	// Has physics enabled?
	bool physics_enabled = true;

	int weapon = 1;

	// Aesthetics
	uint r, g, b, a;
};

struct Player
{
	SDL_Rect canonBody;
	
	float angle = 0;

	bool alive = true,
		win = false;

	bool weapon1 = true,
		weapon2 = false,
		weapon3 = false;
};

struct Terrain
{
	int x, y, w, h;

	double mass = 100,
		acc = 0, accX = 0, accY = 0,
		vel = 0, velX = 0, velY = 0,
		fX = 0, fY = 0,
		restitutionX = 0, restitutionY = 0;

	// Aesthetics
	uint r, g, b, a;
};

struct Water
{
	SDL_Rect waterBody;

};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	double Fl = 0, Fk = 0;
	double Fgx = 0, Fgy = 0; // GRAVITY FORCES
	double FdHx = 0, FdHy = 0, Fb = 0; // HYDRODYNAMIC FORCES (DRAG AND BOUYANCY)
	double FdAx = 0, FdAy = 0; // AERODYNAMIC FORCES (DRAG)

	void EulerBWDIntegrator(p2List_item<Ball*>* &ball, double dt)
	{
		ball->data->x += ball->data->velX * dt;
		ball->data->y += ball->data->velY * dt;
		ball->data->velX += ball->data->accX * dt;
		ball->data->velY += ball->data->accY * dt;
	}
	void EulerFWDIntegrator(p2List_item<Ball*>* &ball, double dt)
	{
		ball->data->velX += ball->data->accX * dt;
		ball->data->velY += ball->data->accY * dt;
		ball->data->x += ball->data->velX * dt;
		ball->data->y += ball->data->velY * dt;
	}

	void VelocityVerletIntegrator(p2List_item<Ball*>* &ball, double dt)
	{
		ball->data->x += ball->data->velX * dt + 0.5 * ball->data->accX * dt * dt;
		ball->data->y += ball->data->velY * dt + 0.5 * ball->data->accY * dt * dt;
		ball->data->velX += ball->data->accX * dt;
		ball->data->velY += ball->data->accY * dt;
	}

	void Gravity()
	{
		Fgx = current_ball->data->mass * 0.0;
		Fgy = current_ball->data->mass * GRAVITY; // Let's assume gravity is constant and downwards
	}

	void Hydrodynamics(p2List_item<Ball*>*& ball, int waterY)
	{
		float coeficientD =0.1, coeficientB = 0.1, density = 0.2, surface = 0;

		if (ball->data->y + ball->data->rad - waterY <= 0)
		{
			surface = 0;
			coeficientD = 0;
		}
		else
		{
			surface = (ball->data->y + ball->data->rad - waterY) * ball->data->rad;
			

			if (surface >= ball->data->rad * ball->data->rad)
			{
				surface = ball->data->rad * ball->data->rad;
			}
			
		}

		// BUOYANCY
		// Sign "-" to define its way
		Fb =  -coeficientB * Fgy * surface;

		

		// DRAG HYDRODYNAMIC
		// Sign "-" to define its way
		if (ball->data->velX < 0)
		{
			FdHx = -ball->data->velX * coeficientD;
		}
		else
		{
			FdHx = -ball->data->velX * coeficientD;
		}
		if (ball->data->velY < 0)
		{
			FdHy = -ball->data->velY * coeficientD;
		}
		else
		{
			FdHy = -ball->data->velY * coeficientD;
		}

	}

	void Aerodynamics(p2List_item<Ball*>*& ball, int waterY)
	{
		float coeficientD = 0.08, density = 0.01, surface = 0;

		if (ball->data->y + ball->data->rad - waterY < 0)
		{
			surface = ball->data->rad*ball->data->rad;
			
		}
		else
		{
			surface = 0;
		}

		// DRAG AERODYNAMIC
		// Sign "-" to define its way
		if (ball->data->velX < 0)
		{
			FdAx = 0.5 * density * (ball->data->velX * ball->data->velX) * surface * coeficientD;
		}
		else
		{
			FdAx = -0.5 * density * (ball->data->velX * ball->data->velX) * surface * coeficientD;
		}
		if (ball->data->velY < 0)
		{
			FdAy = 0.5 * density * (ball->data->velY * ball->data->velY) * surface * coeficientD;
		}
		else
		{
			FdAy = -0.5 * density * (ball->data->velY * ball->data->velY) * surface * coeficientD;
		}
	}


	int initialVelocity1 = 10;
	int initialVelocity2 = 10;

	int angle1INT = 0;
	int angle2INT = 0;

	Player canon, canon2;
	Water lake;

	// BALL LIST
	p2List<Ball*>* ball_list = new p2List<Ball*>();

	Ball* CreateBall(int x, int y, double rad, double mass, double vel, int weaponType, uint r, uint g, uint b, uint a);

	p2List_item<Ball*>* current_ball;

	// TERRAIN LIST
	p2List<Terrain*>* terrain_list = new p2List<Terrain*>();

	Terrain* CreateTerrain(int x, int y, int w, int h, double mass, double vel, double restitutionX, double restitutionY, uint r, uint g, uint b, uint a);

	p2List_item<Terrain*>* current_terrain;

	void CollBallTerrain();
	void CollBallPlayer();

	integrator inte = EULER_FOR;

	double dt = 1;
private:

	bool debug;
};