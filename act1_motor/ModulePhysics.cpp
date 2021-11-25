#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>


ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	//Set physics propierties of the ball
	ball.mass = 10; // kg
	ball.surface = 2; // m^2
	ball.cd = 0.4;
	ball.cl = 1.2;

	ball.vel = 10;

	// Ground Propierties
	ground.mass = 1000; //Kg
	ground.vel = 0;
	ground.velX = 0;
	ground.velY = 0;
	ground.restitutionX = 0.4;
	ground.restitutionY = 0.6;

	// Wall Propierties
	wall1.mass = 100; //Kg
	wall1.vel = 0;
	wall1.velX = 0;
	wall1.velY = 0;
	wall1.restitutionX = 0.8;
	wall1.restitutionY = 0.6;

	prep = true;
	
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	// Step #0: Reset total acceleration and total accumulated force of the ball (clear old values)
	ball.fX = ball.fY = 0.0;
	ball.accX = ball.accY = ball.acc = 0.0;

	// Step #1: Compute forces

		// Compute Gravity force
	double fgX = ball.mass * 0.0;
	double fgY = ball.mass * 1.0; // Let's assume gravity is constant and downwards

	// Add gravity force to the total accumulated force of the ball
	ball.fX += fgX;
	ball.fY += fgY;

	// Compute Aerodynamic Lift & Drag forces
	/*double speed = ball.speed(ball.vx - atmosphere.windx, ball.vy - atmosphere.windy);
	double fdrag = 0.5 * atmosphere.density * speed * speed * ball.surface * ball.cd;
	double flift = 0.5 * atmosphere.density * speed * speed * ball.surface * ball.cl;
	double fdx = -fdrag; // Let's assume Drag is aligned with x-axis (in your game, generalize this)
	double fdy = flift; // Let's assume Lift is perpendicular with x-axis (in your game, generalize this)*/

	// Add gravity force to the total accumulated force of the ball
	/*ball.fX += fdX;
	ball.fX += fdY;*/

	// Other forces
	// ...

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	// Step #2: 2nd Newton's Law: SUM_Forces = mass * accel --> accel = SUM_Forces / mass
	ball.accX = ball.fX / ball.mass;
	ball.accY = ball.fY / ball.mass;

	// Set Canon Angle
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && canon.angle > MIN_ANGLE && prep)
	{
		canon.angle -= 0.1;
		printf("Angle: %.1f\n", canon.angle);

	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && canon.angle < MAX_ANGLE && prep)
	{
		canon.angle += 0.1;
		printf("Angle: %.1f\n", canon.angle);

	}

	// Set Velocity Ball
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && ball.vel <= MAX_VEL && prep)
	{
		ball.vel += 0.5;
		printf("Velocity: %.1f\n", ball.vel);

	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && ball.vel >= MIN_VEL && prep)
	{
		ball.vel -= 0.5;
		printf("Velocity: %.1f\n", ball.vel);

	}

	// Calculate velX & velY
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && prep)
	{
		printf("Shot");
		ball.ballRect.x = canon.canonBody.x;
		ball.ballRect.y = canon.canonBody.y;
		ball.velY = sin(canon.angle) * ball.vel;
		ball.velX = cos(canon.angle) * ball.vel;
		prep = false;


	}

	// Step #3: Integrate --> from accel to new velocity & new position. 
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		inte = EULER_BACK;
		printf("Integrator: EULER_BACK\n");
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		inte = EULER_FOR;
		printf("Integrator: EULER_FOR\n");
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		inte = VELOCITY_VERLET;
		printf("Integrator: VELOCITY_VERLET\n");
	}

	switch (inte)
	{
	case EULER_BACK:
		EulerBWDIntegrator(ball, dt);
		break;
	case EULER_FOR:
		EulerFWDIntegrator(ball, dt);
		break;
	case VELOCITY_VERLET:
		VelocityVerletIntegrator(ball, dt);
		break;
	default:
		printf("Integrator: default\n");
		break;
	}



	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{

	// Step #4: solve collisions
	// GROUND COLL
	if (ball.ballRect.y >= App->physics->ground.body.y)
	{
		ball.ballRect.y = App->physics->ground.body.y;

		ball.velX = ((ball.velX * (ground.mass - ball.mass) + 2 * ball.velX * ball.mass) / (ball.mass + ground.mass)) * ground.restitutionX;
		ball.velY = -((ball.velY * (ground.mass - ball.mass) + 2 * ball.velY * ball.mass) / (ball.mass + ground.mass)) * ground.restitutionY;

		if (ball.velX < 1 && ball.velX > -1) ball.velX = 0;
		if (ball.velY < 4 && ball.velY > -4) ball.velY = 0;

		ball.physics_enabled = false;
		prep = true;
	}

	// WALL COLL
	if (ball.ballRect.y >= App->physics->wall1.body.y && 
		ball.ballRect.y <= (App->physics->wall1.body.y + App->physics->wall1.body.h) &&
		ball.ballRect.x >= App->physics->wall1.body.x &&
		ball.ballRect.x <= (App->physics->wall1.body.x + App->physics->wall1.body.w))
	{
		ball.ballRect.x = App->physics->wall1.body.x;

		ball.velX = -((ball.velX * (wall1.mass - ball.mass) + 2 * ball.velX * ball.mass) / (ball.mass + wall1.mass)) * wall1.restitutionX;
		ball.velY = ((ball.velY * (wall1.mass - ball.mass) + 2 * ball.velY * ball.mass) / (ball.mass + wall1.mass)) * wall1.restitutionY;

		if (ball.velX < 1 && ball.velX > -1) ball.velX = 0;
		if (ball.velY < 4 && ball.velY > -4) ball.velY = 0;

		ball.physics_enabled = false;
		prep = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return UPDATE_CONTINUE;

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	return true;
}
