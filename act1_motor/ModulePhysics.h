#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2List.h"

#define MIN_ANGLE -(M_PI/2)+0.2
#define MAX_ANGLE -0.2

#define MAX_VEL 50
#define MIN_VEL 10

enum integrator
{
	EULER_BACK,
	EULER_FOR,
	VELOCITY_VERLET
};

struct Ball
{
	SDL_Rect ballRect;

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

struct Canon
{
	SDL_Rect canonBody;
	

	float angle = MAX_ANGLE;
};

struct Wall
{
	SDL_Rect body;

	double mass = 100,
		acc = 0, accX = 0, accY = 0,
		vel = 0, velX = 0, velY = 0,
		fX = 0, fY = 0,
		restitutionX = 0, restitutionY = 0;
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
		ball->data->ballRect.x += ball->data->velX * dt;
		ball->data->ballRect.y += ball->data->velY * dt;
		ball->data->velX += ball->data->accX * dt;
		ball->data->velY += ball->data->accY * dt;
	}
	void EulerFWDIntegrator(p2List_item<Ball*>* &ball, double dt)
	{
		ball->data->velX += ball->data->accX * dt;
		ball->data->velY += ball->data->accY * dt;
		ball->data->ballRect.x += ball->data->velX * dt;
		ball->data->ballRect.y += ball->data->velY * dt;
	}

	void VelocityVerletIntegrator(p2List_item<Ball*>* &ball, double dt)
	{
		ball->data->ballRect.x += ball->data->velX * dt + 0.5 * ball->data->accX * dt * dt;
		ball->data->ballRect.y += ball->data->velY * dt + 0.5 * ball->data->accY * dt * dt;
		ball->data->velX += ball->data->accX * dt;
		ball->data->velY += ball->data->accY * dt;
	}

	float Ft = 0, Fg = 0, Fd = 0, Fl = 0, Fb = 0, Fk = 0;
	float g = 0;
	double initialVelocity = 10;

	Canon canon;
	Wall ground;
	Wall wall1;

	p2List<Ball*>* ball_list = new p2List<Ball*>();

	Ball* CreateBall(int x, int y, int w, int h, int mass, int vel) {
		Ball* b = new Ball();

		//Omplim els valors de la bola
		b->ballRect.x = x;
		b->ballRect.y = y;
		b->ballRect.w = w;
		b->ballRect.h = h;
		b->mass = mass;


		b->velY = sin(canon.angle) * vel;
		b->velX = cos(canon.angle) * vel;

		ball_list->add(b);
		
		return b;
	}

	p2List_item<Ball*>* current_ball;

	integrator inte = EULER_BACK;
	double dt = 1;
private:

	bool debug;
};