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
	debug = false;
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
	lake.waterBody.x = SCREEN_WIDTH / 5;
	lake.waterBody.y = SCREEN_HEIGHT - 240;
	lake.waterBody.w = SCREEN_WIDTH - (SCREEN_WIDTH / 5) - 204;
	lake.waterBody.h = 250;

	// ----------------------------------------------------------------------------- GROUND LIMIT
	CreateTerrain(0, SCREEN_HEIGHT, SCREEN_WIDTH, 50, 100, 0, 0.5, 0.5,
		0, 0, 255, 255);

	// ----------------------------------------------------------------------------- GROUND 1
	CreateTerrain(0, SCREEN_HEIGHT - 50 - 240, SCREEN_WIDTH / 5, 50 + 240, 1000, 0, 0.4, 0.5,
		255, 127, 50, 255);

	// ----------------------------------------------------------------------------- GROUND 2
	CreateTerrain(SCREEN_WIDTH - SCREEN_WIDTH / 5, SCREEN_HEIGHT - 50 - 240, SCREEN_WIDTH / 5, 50 + 240, 1000, 0, 0.4, 0.5,
		255, 127, 50, 255);

	// ----------------------------------------------------------------------------- WALL
	CreateTerrain((SCREEN_WIDTH / 2) - 60, SCREEN_HEIGHT - 200 - 240, 120, 180, 100, 0, 0.5, 1.5,
		255, 127, 50, 255);

	initialVelocity1 = MIN_VEL;
	initialVelocity2 = MIN_VEL;

	canon.angle = -MAX_ANGLE;
	canon2.angle = -M_PI;

	canon.alive = true;
	canon2.alive = true;

	canon.win = false;
	canon2.win = false;

	gravityEnabled = true;
	hydrodynamicDragEnabled = true;
	bouyancyEnabled = true;
	aerodynamicDragEnabled = true;
	debug = false;

	sixtyFpsEnabled = true;

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
		Gravity();
			
			// Compute AERODYNAMIC forces
		Aerodynamics(current_ball, lake.waterBody.y);

			// Compute HYDRODYNAMIC forces
		Hydrodynamics(current_ball, lake.waterBody.y);

		// Add gravity force to the total accumulated force of the ball and player
		current_ball->data->fX += (Fgx + FdHx + FdAx);
		current_ball->data->fY += (Fgy + Fb + FdHy + FdAy);

		current_ball = current_ball->next;
	}
	

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	// R TO RESTART
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		// ------------------------------- WE RESET ALL THE VALUES AND POSITIONS
		canon.angle = -MAX_ANGLE;
		canon2.angle = -M_PI;

		initialVelocity1 = 10;
		initialVelocity2 = 10;

		canon.alive = true;
		canon2.alive = true;

		canon.win = false;
		canon2.win = false;

		canon.canonBody.x = 50;
		canon.canonBody.y = SCREEN_HEIGHT - 70 - 240;

		canon2.canonBody.x = SCREEN_WIDTH - 50;
		canon2.canonBody.y = SCREEN_HEIGHT - 70 - 240;

		canon.weapon1 = true;
		canon.weapon2 = false;
		canon.weapon3 = false;

		canon2.weapon1 = true;
		canon2.weapon2 = false;
		canon2.weapon3 = false;

		App->player->playerTurn = 0;

		// WE CLEAR ALL BALLS
		ball_list->clear();

	}

	// Set Integrator
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && debug)
	{
		inte = EULER_BACK;
		printf("Integrator: EULER_BACK\n");
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && debug)
	{
		inte = EULER_FOR;
		printf("Integrator: EULER_FOR\n");
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN && debug)
	{
		inte = VELOCITY_VERLET;
		printf("Integrator: VELOCITY_VERLET\n");
	}

	// Set Canon Angle
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0 && canon.alive)
		{
			if (canon.angle > MIN_ANGLE)
			{
				canon.angle -= 0.1;

			}
			else
			{
				canon.angle = MIN_ANGLE;

			}
			printf("Angle player 1: %.1f\n", canon.angle);
		}
		if (App->player->playerTurn == 1 && canon2.alive)
		{
			if (canon2.angle < MIN_ANGLE - 0.4)
			{
				canon2.angle += 0.1;

			}
			else
			{
				canon2.angle = MIN_ANGLE - 0.4;

			}
			printf("Angle player 2: %.1f\n", canon2.angle);
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0 && canon.alive)
		{
			if (canon.angle < MAX_ANGLE)
			{
				canon.angle += 0.1;

			}
			else
			{
				canon.angle = MAX_ANGLE;

			}
			printf("Angle player 1: %.1f\n", canon.angle);
		}
		if (App->player->playerTurn == 1 && canon2.alive)
		{
			if (canon2.angle > -M_PI)
			{
				canon2.angle -= 0.1;

			}
			else
			{
				canon2.angle = -M_PI;

			}
			printf("Angle player 2: %.1f\n", canon2.angle);
		}
	}

	// Set Velocity Ball
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0 && canon.alive)
		{
			if (initialVelocity1 < MAX_VEL)
				initialVelocity1 += 1;
			else
				initialVelocity1 = MAX_VEL;
			printf("Velocity player 1: %d\n", initialVelocity1);
		}
		if (App->player->playerTurn == 1 && canon2.alive)
		{
			if (initialVelocity2 < MAX_VEL)
				initialVelocity2 += 1;
			else
				initialVelocity2 = MAX_VEL;
			printf("Velocity player 2: %d\n", initialVelocity2);
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (App->player->playerTurn == 0 && canon.alive)
		{
			if (initialVelocity1 > MIN_VEL)
				initialVelocity1 -= 1;
			else
				initialVelocity1 = MIN_VEL;
			printf("Velocity player 1: %d\n", initialVelocity1);
		}
		if (App->player->playerTurn == 1 && canon2.alive)
		{
			if (initialVelocity2 > MIN_VEL)
				initialVelocity2 -= 1;
			else
				initialVelocity2 = MIN_VEL;
			printf("Velocity player 2: %d\n", initialVelocity2);
		}

	}

	// ------------------------------------------------------------------------------------------------------------------ CHANGE WEAPONS
	// WEAPON 1 -> SIMPLE BALL
	// WEAPON 2 -> STICKY MINE
	// WEAPON 3 -> SHOT GUN

	if (App->player->playerTurn == 0 && canon.alive)
	{
		if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
		{
			printf("Weapon P1 - SIMPLE BALL\n");

			canon.weapon1 = true;
			canon.weapon2 = false;
			canon.weapon3 = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
		{
			printf("Weapon P1 - STICKY MINE\n");

			canon.weapon1 = false;
			canon.weapon2 = true;
			canon.weapon3 = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
		{
			printf("Weapon P1 - SHOT GUN\n");

			canon.weapon1 = false;
			canon.weapon2 = false;
			canon.weapon3 = true;
		}
	}
	else if (App->player->playerTurn == 1 && canon2.alive)
	{
		if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
		{
			printf("Weapon P2 - SIMPLE BALL\n");

			canon2.weapon1 = true;
			canon2.weapon2 = false;
			canon2.weapon3 = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
		{
			printf("Weapon P2 - STICKY MINE\n");

			canon2.weapon1 = false;
			canon2.weapon2 = true;
			canon2.weapon3 = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
		{
			printf("Weapon P1 - SHOT GUN\n");

			canon2.weapon1 = false;
			canon2.weapon2 = false;
			canon2.weapon3 = true;
		}
	}

	// ------------------------------------------------------------------------------------------------------------------ SHOOT
	// --------------------------------------------------------------------- WEAPON 1 - SIMPLE BALL
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (App->player->playerTurn == 0 && canon.alive && canon.weapon1)
		{
			printf("Shot player 1 - SIMPLE BULLET\n");
			CreateBall(canon.canonBody.x + canon.canonBody.w, canon.canonBody.y, 5, 10, initialVelocity1, 1, 0, 0, 0, 255);
			App->player->playerTurn = 1;
			printf("---------------------PLAYER'S 2 TURN---------------------\n");
		}
		else if (App->player->playerTurn == 1 && canon2.alive && canon2.weapon1)
		{
			printf("Shot player 2 - SIMPLE BULLET\n");
			CreateBall(canon2.canonBody.x, canon2.canonBody.y, 5, 10, initialVelocity2, 1, 0, 0, 0, 255);
			App->player->playerTurn = 0;
			printf("---------------------PLAYER'S 1 TURN---------------------\n");
		}

		// --------------------------------------------------------------------- WEAPON 2 - STICKY MINE
		else if (App->player->playerTurn == 0 && canon.alive && canon.weapon2)
		{
			printf("Shot player 1 - STICKY MINE\n");
			CreateBall(canon.canonBody.x + canon.canonBody.w, canon.canonBody.y, 3, 5, initialVelocity1, 2, 255, 0, 0, 255);
			App->player->playerTurn = 1;
			printf("---------------------PLAYER'S 2 TURN---------------------\n");
		}
		else if (App->player->playerTurn == 1 && canon2.alive && canon2.weapon2)
		{
			printf("Shot player 2 - STICKY MINE\n");
			CreateBall(canon2.canonBody.x, canon2.canonBody.y, 3, 5, initialVelocity2, 2, 255, 0, 0, 255);
			App->player->playerTurn = 0;
			printf("---------------------PLAYER'S 1 TURN---------------------\n");
		}

		// --------------------------------------------------------------------- WEAPON 3 - SHOT GUN
		else if (App->player->playerTurn == 0 && canon.alive && canon.weapon3)
		{
			printf("Shot player 1 - SHOT GUN\n");
			CreateBall(canon.canonBody.x + canon.canonBody.w - 10, canon.canonBody.y - 20, 2, 2, initialVelocity1, 3, 0, 0, 0, 255);
			CreateBall(canon.canonBody.x + canon.canonBody.w + 10, canon.canonBody.y, 2, 2, initialVelocity1, 3, 0, 0, 0, 255);
			App->player->playerTurn = 1;
			printf("---------------------PLAYER'S 2 TURN---------------------\n");
		}
		else if (App->player->playerTurn == 1 && canon2.alive && canon2.weapon3)
		{
			printf("Shot player 2 - SHOT GUN\n");
			CreateBall(canon2.canonBody.x + 10, canon2.canonBody.y - 20, 2, 2, initialVelocity2, 3, 0, 0, 0, 255);
			CreateBall(canon2.canonBody.x - 10, canon2.canonBody.y, 2, 2, initialVelocity2, 3, 0, 0, 0, 255); 
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
	// Step #4: solve collisions
	App->renderer->DrawQuad(lake.waterBody, 0, 0, 255, 100);
	current_ball = ball_list->getFirst();

	while (current_ball != NULL)
	{
		CollBallTerrain();
		CollBallPlayer();

		current_ball = current_ball->next;
	}
	
	angle1INT = canon.angle * 180 / M_PI;
	angle2INT = canon2.angle * 180 / M_PI + 180;

	// ------------------------------------------------------------------------------------- DEBUG KEYS
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	// ENABLE/DISABLE GRAVITY
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && debug)
	{
		if (!gravityEnabled)
			gravityEnabled = true;
		else
			gravityEnabled = false;
	}

	// ENABLE/DISABLE HYDRODYNAMIC DRAG
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && debug)
	{
		if (!hydrodynamicDragEnabled)
			hydrodynamicDragEnabled = true;
		else
			hydrodynamicDragEnabled = false;
	}

	// ENABLE/DISABLE BOUYANCY
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && debug)
	{
		if (!bouyancyEnabled)
			bouyancyEnabled = true;
		else
			bouyancyEnabled = false;
	}

	// ENABLE/DISABLE AERODYNAMIC DRAG
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && debug)
	{
		if (!aerodynamicDragEnabled)
			aerodynamicDragEnabled = true;
		else
			aerodynamicDragEnabled = false;
	}

	// SET GOD MODE
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && debug)
	{
		if (!App->player->godMode)
			App->player->godMode = true;
		else
			App->player->godMode = false;
	}

	// CHANGE FROM 60fps TO 30fps AND VICE VERSA
	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN && debug)
	{
		if (!sixtyFpsEnabled)
			sixtyFpsEnabled = true;
		else
			sixtyFpsEnabled = false;
	}

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

Ball* ModulePhysics::CreateBall(int x, int y, double rad, double mass, double vel, int weaponType, uint r, uint g, uint b, uint a)
{
	Ball* ball = new Ball();

	ball->x = x;
	ball->y = y;
	ball->rad = rad;
	ball->mass = mass;
	
	ball->weapon = weaponType;

	//Aesthetics
	ball->r = r;
	ball->g = g;
	ball->b = b;
	ball->a = a;

	if (App->player->playerTurn == 0)
	{
		ball->velY = sin(canon.angle) * vel;
		ball->velX = cos(canon.angle) * vel;
	}
	else
	{
		ball->velY = sin(canon2.angle) * vel;
		ball->velX = cos(canon2.angle) * vel;
	}

	ball_list->add(ball);

	return ball;
}

Terrain* ModulePhysics::CreateTerrain(int x, int y, int w, int h, double mass, double vel, double restitutionX, double restitutionY, uint r, uint g, uint b, uint a)
{
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


void ModulePhysics::CollBallTerrain()
{
	current_terrain = terrain_list->getFirst();

	while (current_terrain != NULL)
	{
		// SOLVE COLLISIONS BETWEEN BALL AND TERRAIN

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
					if (current_ball->data->weapon == 1)
					{
						current_ball->data->x = (current_terrain->data->x + current_terrain->data->w) + current_ball->data->rad / 2;

						current_ball->data->velX = -((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = ((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						current_ball->data->physics_enabled = false;
					}
					else if (current_ball->data->weapon == 2)
					{
						current_ball->data->x = (current_terrain->data->x + current_terrain->data->w) + current_ball->data->rad / 2;

						current_ball->data->velX = 0;
						current_ball->data->velY = 0;

						current_ball->data->physics_enabled = false;

					}
					else if (current_ball->data->weapon == 3)
					{
						current_ball->data->x = (current_terrain->data->x + current_terrain->data->w) + current_ball->data->rad / 2;

						current_ball->data->velX = -((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = ((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						current_ball->data->physics_enabled = false;

					}
					
				}
				// Is the X distance between ball's center and terrain's center --deltaX-- positive? 
				// If NOT, is colliding from the LEFT!
				else
				{
					if (current_ball->data->weapon == 1)
					{
						current_ball->data->x = current_terrain->data->x - current_ball->data->rad / 2;

						current_ball->data->velX = -((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = ((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						current_ball->data->physics_enabled = false;
					}
					else if (current_ball->data->weapon == 2)
					{
						current_ball->data->x = current_terrain->data->x - current_ball->data->rad / 2;

						current_ball->data->velX = 0;
						current_ball->data->velY = 0;

						current_ball->data->physics_enabled = false;

					}
					else if (current_ball->data->weapon == 3)
					{
						current_ball->data->x = current_terrain->data->x - current_ball->data->rad / 2;
						
						current_ball->data->velX = -((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = ((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						current_ball->data->physics_enabled = false;

					}
				}
			}
			// Is the X intersection higher? If NOT, they are colliding UP or DOWN!
			else
			{
				// Is the Y distance between ball's center and terrain's center --deltaY-- positive?
				// If so, is colliding from DOWN!
				if (deltaY > 0.0f)
				{
					if (current_ball->data->weapon == 1)
					{
						current_ball->data->y = (current_terrain->data->y + current_terrain->data->h) + current_ball->data->rad / 2;

						current_ball->data->velX = ((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = -((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						if (current_ball->data->velY > -5 && current_ball->data->velY < 5) current_ball->data->velY = 0;
						if (current_ball->data->velX < 0.001 && current_ball->data->velX > -0.001) current_ball->data->velX = 0;

						current_ball->data->physics_enabled = false;
					}
					else if (current_ball->data->weapon == 2)
					{
						current_ball->data->y = (current_terrain->data->y + current_terrain->data->h) + current_ball->data->rad / 2;

						current_ball->data->velX = 0;
						current_ball->data->velY = 0;

						current_ball->data->physics_enabled = false;

					}
					else if (current_ball->data->weapon == 3)
					{
						current_ball->data->y = (current_terrain->data->y + current_terrain->data->h) + current_ball->data->rad / 2;

						current_ball->data->velX = ((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = -((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						if (current_ball->data->velY > -5 && current_ball->data->velY < 5) current_ball->data->velY = 0;
						if (current_ball->data->velX < 0.001 && current_ball->data->velX > -0.001) current_ball->data->velX = 0;

						current_ball->data->physics_enabled = false;

					}

				}
				// Is the Y distance between ball's center and terrain's center --deltaY-- positive?
				// If NOT, is colliding from UP!
				else
				{
					if (current_ball->data->weapon == 1)
					{
						current_ball->data->y = current_terrain->data->y - current_ball->data->rad / 2;

						current_ball->data->velX = ((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = -((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						if (current_ball->data->velY > -5 && current_ball->data->velY < 5) current_ball->data->velY = 0;
						if (current_ball->data->velX < 0.001 && current_ball->data->velX > -0.001) current_ball->data->velX = 0;

						current_ball->data->physics_enabled = false;
					}
					else if (current_ball->data->weapon == 2)
					{
						current_ball->data->y = current_terrain->data->y - current_ball->data->rad / 2;

						current_ball->data->velX = 0;
						current_ball->data->velY = 0;

						current_ball->data->physics_enabled = false;

					}
					else if (current_ball->data->weapon == 3)
					{
						current_ball->data->y = current_terrain->data->y - current_ball->data->rad / 2;

						current_ball->data->velX = ((current_ball->data->velX * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velX * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));
						current_ball->data->velY = -((current_ball->data->velY * (current_terrain->data->mass - current_ball->data->mass) + 2 * current_ball->data->velY * current_ball->data->mass) / (current_ball->data->mass + current_terrain->data->mass));

						if (current_ball->data->velY > -5 && current_ball->data->velY < 5) current_ball->data->velY = 0;
						if (current_ball->data->velX < 0.001 && current_ball->data->velX > -0.001) current_ball->data->velX = 0;

						current_ball->data->physics_enabled = false;

					}
				}
			}
		}
		current_terrain = current_terrain->next;
	}
}

void ModulePhysics::CollBallPlayer()
{

		// SOLVE COLLISIONS BETWEEN BALL AND PLAYER

		// Calculations:
		// - deltaX = X distance between ball's center and player's center
		// - deltaY = Y distance between ball's center and player's center
		// - intersectX = Sees if the X distance between ball's center and player's center --deltaX-- 
		// is lower than the addition of the half of the two bodies. If so, they are colliding (one inside the other).
		// This is checked later in an if
		// - intersectY = Sees if the Y distance between ball's center and player's center --deltaY-- 
		// is lower than the addition of the half of the two bodies. If so, they are colliding (one inside the other).
		// This is checked later in an if

		float P1_deltaX = (current_ball->data->x + current_ball->data->rad / 2) - (canon.canonBody.x + canon.canonBody.w / 2);
		float P1_deltaY = (current_ball->data->y + current_ball->data->rad / 2) - (canon.canonBody.y + canon.canonBody.h / 2);
		float P1_intersectX = abs(P1_deltaX) - ((current_ball->data->rad / 2) + (canon.canonBody.w / 2));
		float P1_intersectY = abs(P1_deltaY) - ((current_ball->data->rad / 2) + (canon.canonBody.h / 2));

		// Is one body inside of the other?
		if (P1_intersectX < 0.0f && P1_intersectY < 0.0f && !App->player->godMode)
		{
			canon.alive = false;
			canon2.win = true;
		}

		float P2_deltaX = (current_ball->data->x + current_ball->data->rad / 2) - (canon2.canonBody.x + canon2.canonBody.w / 2);
		float P2_deltaY = (current_ball->data->y + current_ball->data->rad / 2) - (canon2.canonBody.y + canon2.canonBody.h / 2);
		float P2_intersectX = abs(P2_deltaX) - ((current_ball->data->rad / 2) + (canon2.canonBody.w / 2));
		float P2_intersectY = abs(P2_deltaY) - ((current_ball->data->rad / 2) + (canon2.canonBody.h / 2));

		// Is one body inside of the other?
		if (P2_intersectX < 0.0f && P2_intersectY < 0.0f && !App->player->godMode)
		{
			canon2.alive = false;
			canon.win = true;

		}
}
