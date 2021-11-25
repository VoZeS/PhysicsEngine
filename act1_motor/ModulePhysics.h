#pragma once
#include "Module.h"
#include "Globals.h"

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

	double mass, 
		acc, accX, accY,
		vel, velX, velY,
		fX, fY;

	//Aerodynamics Stuff
	double surface; //Effective wet surface
	double cl; // Lift coefficient
	double cd; // Drag coefficient

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

	double mass,
		acc, accX, accY,
		vel, velX, velY,
		fX, fY,
		restitutionX, restitutionY;
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

	void EulerBWDIntegrator(Ball &ball, double dt)
	{
		ball.ballRect.x += ball.velX * dt;
		ball.ballRect.y += ball.velY * dt;
		ball.velX += ball.accX * dt;
		ball.velY += ball.accY * dt;
	}
	void EulerFWDIntegrator(Ball& ball, double dt)
	{
		ball.velX += ball.accX * dt;
		ball.velY += ball.accY * dt;
		ball.ballRect.x += ball.velX * dt;
		ball.ballRect.y += ball.velY * dt;
	}

	void VelocityVerletIntegrator(Ball &ball, double dt)
	{
		ball.ballRect.x += ball.velX * dt + 0.5 * ball.accX * dt * dt;
		ball.ballRect.y += ball.velY * dt + 0.5 * ball.accY * dt * dt;
		ball.velX += ball.accX * dt;
		ball.velY += ball.accY * dt;
	}

	float Ft = 0, Fg = 0, Fd = 0, Fl = 0, Fb = 0, Fk = 0;
	float g;

	Ball ball;
	Canon canon;
	Wall ground;
	Wall wall1;

	p2List<Ball*>* ball_list;
	Ball* CreateBall(int x, int y, int w, int h) {
		Ball* b = new Ball();
		//Omplir els valors de la bola
		b->ballRect.x = x;
		b->ballRect.y = y;
		b->ballRect.w = w;
		b->ballRect.h = h;

		ball_list->add(b);
		return b;
	}

	p2List_item<Ball*>* current_ball = ball_list->getFirst();
	
	//current_ball = current_ball->next;

	integrator inte = EULER_BACK;
	double dt = 1;
private:

	bool prep = true;

	bool debug;
};