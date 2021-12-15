#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


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

	// ----------------------------------------------------------------------------------------------- CREATE TERRAIN
		// ----------------------------------------------------------------------------- WATER
	CreateTerrain(SCREEN_WIDTH / 5, SCREEN_HEIGHT - 40, SCREEN_WIDTH - (SCREEN_WIDTH / 5) - 170, 50, 100, 0, 0.8, 0.6,
		100, 100, 255, 100);

	// ----------------------------------------------------------------------------- GROUND 1
	CreateTerrain(0, SCREEN_HEIGHT - 50 , SCREEN_WIDTH / 5, 50, 1000, 0, 0.4, 0.5,
		0, 255, 255, 255);

	// ----------------------------------------------------------------------------- GROUND 2
	CreateTerrain(SCREEN_WIDTH - SCREEN_WIDTH / 5, SCREEN_HEIGHT - 50, SCREEN_WIDTH / 5, 50, 1000, 0, 0.4, 0.5,
		0, 255, 255, 255);

	// ----------------------------------------------------------------------------- WALL
	CreateTerrain((SCREEN_WIDTH / 2) - 60, SCREEN_HEIGHT - 200, 120, 200, 100, 0, 0.5, 1.5,
		255, 0, 255, 255);
	
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
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0)
		{
			if (canon.angle < MAX_ANGLE)
				canon.angle += 0.1;
			else
				canon.angle = MAX_ANGLE;
			printf("Angle player 1: %.1f\n", canon.angle);
		}
		if (App->player->playerTurn == 1)
		{
			if (canon2.angle < MAX_ANGLE)
				canon2.angle += 0.1;
			else
				canon2.angle = MAX_ANGLE;
			printf("Angle player 2: %.1f\n", canon2.angle);
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0)
		{
			if (canon.angle > MIN_ANGLE)
				canon.angle -= 0.1;
			else
				canon.angle = MIN_ANGLE;
			printf("Angle player 1: %.1f\n", canon.angle);
		}
		if (App->player->playerTurn == 1)
		{
			if (canon2.angle > MIN_ANGLE)
				canon2.angle -= 0.1;
			else
				canon2.angle = MIN_ANGLE;
			printf("Angle player 2: %.1f\n", canon2.angle);
		}


		

	}

	// Set Velocity Ball
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0)
		{
			if (initialVelocity1 < MAX_VEL)
				initialVelocity1 += 0.1;
			else
				initialVelocity1 = MAX_VEL;
			printf("Velocity player 1: %.1f\n", initialVelocity1);
		}
		if (App->player->playerTurn == 1)
		{
			if (initialVelocity2 < MAX_VEL)
				initialVelocity2 += 0.1;
			else
				initialVelocity2 = MAX_VEL;
			printf("Velocity player 2: %.1f\n", initialVelocity2);
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0)
		{
			if (initialVelocity1 > MIN_VEL)
				initialVelocity1 -= 0.1;
			else
				initialVelocity1 = MIN_VEL;
			printf("Velocity player 1: %.1f\n", initialVelocity1);
		}
		if (App->player->playerTurn == 1)
		{
			if (initialVelocity2 > MIN_VEL)
				initialVelocity2 -= 0.1;
			else
				initialVelocity2 = MIN_VEL;
			printf("Velocity player 2: %.1f\n", initialVelocity2);
		}

	}

	// SHOOT
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT)
	{
		
		if (App->player->playerTurn == 0)
		{
			printf("Shot player 1\n");
			CreateBall(canon.canonBody.x, canon.canonBody.y, 10, 10, initialVelocity1);
			App->player->playerTurn = 1;
			printf("---------------------PLAYER'S 2 TURN---------------------\n");
		}
		else
		{
			printf("Shot player 2\n");
			CreateBall(canon2.canonBody.x, canon2.canonBody.y, 10, 10, -initialVelocity2);
			App->player->playerTurn = 0;
			printf("---------------------PLAYER'S 1 TURN---------------------\n");
		}


		
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
		current_terrain = terrain_list->getFirst();

		while (current_terrain != NULL)
		{
			// Step #4: solve collisions
			// Calculations:
			// - deltaX = X distance between ball's center and terrain's center
			// - deltaY = Y distance between ball's center and terrain's center
			// - intersectX = Sees if the X distance between ball's center and terrain's center --deltaX-- 
			// is lower than the addition of the half of the two bodies. If so, they are colliding (one inside the other).
			// This is checked later in an if
			// - intersectY = Sees if the Y distance between ball's center and terrain's center --deltaY-- 
			// is lower than the addition of the half of the two bodies. If so, they are colliding (one inside the other).
			// This is checked later in an if

			float deltaX = (current_ball->data->x + current_ball->data->rad / 2) - (current_terrain->data->x + current_terrain->data->w / 2);
			float deltaY = (current_ball->data->y + current_ball->data->rad / 2) - (current_terrain->data->y + current_terrain->data->h / 2);
			float intersectX = abs(deltaX) - ((current_ball->data->rad / 2) + (current_terrain->data->w / 2));
			float intersectY = abs(deltaY) - ((current_ball->data->rad / 2) + (current_terrain->data->h / 2));

			// Is one body inside of the other?
			if (intersectX < 0.0f && intersectY < 0.0f)
			{
				// Is the X intersection higher? If so, they are colliding from the sides!
				if (intersectX > intersectY)
				{
					// Is the X distance between ball's center and terrain's center --deltaX-- positive?
					// If so, is colliding from the RIGHT!
					if (deltaX > 0.0f)
					{
						current_ball->data->x = (current_terrain->data->x + current_terrain->data->w) + current_ball->data->rad;

						current_ball->data->velX = -((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass)) * current_terrain->data->restitutionX;
						current_ball->data->velY = ((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass)) * current_terrain->data->restitutionY;

						current_ball->data->physics_enabled = false;
					}
					// Is the X distance between ball's center and terrain's center --deltaX-- positive? 
					// If NOT, is colliding from the LEFT!
					else
					{
						current_ball->data->x = current_terrain->data->x - current_ball->data->rad;

						current_ball->data->velX = -((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass)) * current_terrain->data->restitutionX;
						current_ball->data->velY = ((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						current_ball->data->physics_enabled = false;
					}
				}
				// Is the X intersection higher? If NOT, they are colliding UP or DOWN!
				else
				{
					// Is the Y distance between ball's center and terrain's center --deltaY-- positive?
					// If so, is colliding from DOWN!
					if (deltaY > 0.0f)
					{
						current_ball->data->y = (current_terrain->data->y + current_terrain->data->h) + current_ball->data->rad;

						current_ball->data->y = current_terrain->data->y;

						current_ball->data->velX = ((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass)) * current_terrain->data->restitutionX;
						current_ball->data->velY = -((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass)) * current_terrain->data->restitutionY;

						current_ball->data->physics_enabled = false;

					}
					// Is the Y distance between ball's center and terrain's center --deltaY-- positive?
					// If NOT, is colliding from UP!
					else
					{
						current_ball->data->y = current_terrain->data->y - current_ball->data->rad;

						current_ball->data->y = current_terrain->data->y;

						current_ball->data->velX = ((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass)) * current_terrain->data->restitutionX;
						current_ball->data->velY = -((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass)) * current_terrain->data->restitutionY;

						if (current_ball->data->velY > -5 && current_ball->data->velY < 5) current_ball->data->velY = 0;
						if (current_ball->data->velX < 0.001 && current_ball->data->velX > -0.001) current_ball->data->velX = 0;

						current_ball->data->physics_enabled = false;

					}
				}
			}
			current_terrain = current_terrain->next;
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
