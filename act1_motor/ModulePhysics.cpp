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
	
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	current_ball = ball_list->getFirst();

	while (current_ball != NULL)
	{

		// Step #0: Reset total acceleration and total accumulated force of the ball (clear old values)
		current_ball->data->fX = current_ball->data->fY = 0.0;
		current_ball->data->accX = current_ball->data->accY = current_ball->data->acc = 0.0;

		// Step #1: Compute forces

			// Compute Gravity force
		double fgX = current_ball->data->mass * 0.0;
		double fgY = current_ball->data->mass * 1.0; // Let's assume gravity is constant and downwards

		// Add gravity force to the total accumulated force of the ball
		current_ball->data->fX += fgX;
		current_ball->data->fY += fgY;

		// Compute Aerodynamic Lift & Drag forces
		/*double speed = current_ball->data->speed(current_ball->data->vx - atmosphere.windx, current_ball->data->vy - atmosphere.windy);
		double fdrag = 0.5 * atmosphere.density * speed * speed * current_ball->data->surface * current_ball->data->cd;
		double flift = 0.5 * atmosphere.density * speed * speed * current_ball->data->surface * current_ball->data->cl;
		double fdx = -fdrag; // Let's assume Drag is aligned with x-axis (in your game, generalize this)
		double fdy = flift; // Let's assume Lift is perpendicular with x-axis (in your game, generalize this)*/

		// Add gravity force to the total accumulated force of the ball
		/*current_ball->data->fX += fdX;
		current_ball->data->fX += fdY;*/

		// Other forces
		// ...

		current_ball = current_ball->next;
	}
	

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	// Set Integrator
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

	// Set Canon Angle
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && canon.angle > MIN_ANGLE)
	{
		canon.angle -= 0.1;
		printf("Angle: %.1f\n", canon.angle);

	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && canon.angle < MAX_ANGLE)
	{
		canon.angle += 0.1;
		printf("Angle: %.1f\n", canon.angle);

	}

	// Set Velocity Ball
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && initialVelocity <= MAX_VEL)
	{
		initialVelocity += 0.5;
		printf("Velocity: %.1f\n", initialVelocity);

	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && initialVelocity >= MIN_VEL)
	{
		initialVelocity -= 0.5;
		printf("Velocity: %.1f\n", initialVelocity);

	}

	// SHOOT
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT)
	{
		printf("Shot\n");
		CreateBall(canon.canonBody.x, canon.canonBody.y, 10, 10, 10, initialVelocity);
	}

	current_ball = ball_list->getFirst();

	while (current_ball != NULL)
	{
		// Step #2: 2nd Newton's Law: SUM_Forces = mass * accel --> accel = SUM_Forces / mass
		current_ball->data->accX = current_ball->data->fX / current_ball->data->mass;
		current_ball->data->accY = current_ball->data->fY / current_ball->data->mass;

		// Step #3: Integrate --> from accel to new velocity & new position. 
		switch (inte)
		{
		case EULER_BACK:
			EulerBWDIntegrator(current_ball, dt);
			break;
		case EULER_FOR:
			EulerFWDIntegrator(current_ball, dt);
			break;
		case VELOCITY_VERLET:
			VelocityVerletIntegrator(current_ball, dt);
			break;
		default:
			printf("Integrator: default\n");
			break;
		}

		current_ball = current_ball->next;
	}
	
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	current_ball = ball_list->getFirst();

	while (current_ball != NULL)
	{

		// Step #4: solve collisions
		// GROUND COLL
		if (current_ball->data->ballRect.y >= App->physics->ground.body.y)
		{
			current_ball->data->ballRect.y = App->physics->ground.body.y;

			current_ball->data->velX = ((current_ball->data->velX * (ground.mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + ground.mass)) * ground.restitutionX;
			current_ball->data->velY = -((current_ball->data->velY * (ground.mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + ground.mass)) * ground.restitutionY;

			if (current_ball->data->velY < 5 && current_ball->data->velY > -5) current_ball->data->velY = 0;
			if (current_ball->data->velX < 0.01 && current_ball->data->velX > -0.01) current_ball->data->velX = 0;

			current_ball->data->physics_enabled = false;
		}

		// WALL COLL
		if (current_ball->data->ballRect.y >= App->physics->wall1.body.y &&
			current_ball->data->ballRect.y <= (App->physics->wall1.body.y + App->physics->wall1.body.h) &&
			current_ball->data->ballRect.x >= App->physics->wall1.body.x &&
			current_ball->data->ballRect.x <= (App->physics->wall1.body.x + App->physics->wall1.body.w))
		{
			current_ball->data->ballRect.x = App->physics->wall1.body.x;

			current_ball->data->velX = -((current_ball->data->velX * (wall1.mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + wall1.mass)) * wall1.restitutionX;
			current_ball->data->velY = ((current_ball->data->velY * (wall1.mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + wall1.mass)) * wall1.restitutionY;

			current_ball->data->physics_enabled = false;
		}

		current_ball = current_ball->next;
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