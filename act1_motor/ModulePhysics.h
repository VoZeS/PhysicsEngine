#pragma once
#include "Module.h"
#include "Application.h"
#include "Globals.h"
#include "p2List.h"

#define MIN_ANGLE -(M_PI/2) + 0.2
#define MAX_ANGLE -0.2

#define MAX_VEL 34
#define MIN_VEL 15

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
};

struct Player
{
	SDL_Rect canonBody;
	
	float angle = 0;

	bool alive = true,
		win = false;
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

	float Ft = 0, Fg = 0, Fd = 0, Fl = 0, Fb = 0, Fk = 0;
	float g = 0;
	double initialVelocity1 = 10;
	double initialVelocity2 = 10;

	Player canon, canon2;
	Water lake;

	// BALL LIST
	p2List<Ball*>* ball_list = new p2List<Ball*>();

	Ball* CreateBall(int x, int y, double rad, double mass, double vel);

	p2List_item<Ball*>* current_ball;

	// TERRAIN LIST
	p2List<Terrain*>* terrain_list = new p2List<Terrain*>();

	Terrain* CreateTerrain(int x, int y, int w, int h, double mass, double vel, double restitutionX, double restitutionY, uint r, uint g, uint b, uint a) {
		Terrain* t = new Terrain();

		t->x = x;
		t->y = y;
		t->w = w;
		t->h = h;
		t->mass = mass;
		t->restitutionX = restitutionX;
		t->restitutionY = restitutionY;

		//Aesthetics
		t->r = r;
		t->g = g;
		t->b = b;
		t->a = a;

		t->velY = sin(canon.angle) * vel;
		t->velX = cos(canon.angle) * vel;

		terrain_list->add(t);

		return t;
	}

	p2List_item<Terrain*>* current_terrain;

	void CollBallTerrain();
	void CollBallPlayer();

	integrator inte = EULER_BACK;

	double dt = 1;
private:

	bool debug;
};