#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics.h"



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	backgroundTex = App->textures->Load("Assets/background.png");
	p1winTex = App->textures->Load("Assets/p1win.png");
	p2winTex = App->textures->Load("Assets/p2win.png");
	restartTex = App->textures->Load("Assets/restart.png");

	ballTex = App->textures->Load("Assets/ball.png");
	mineTex = App->textures->Load("Assets/mine.png");
	pumpTex = App->textures->Load("Assets/pump.png");

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Set wall1 body
	/*App->physics->wall1.body.x = SCREEN_WIDTH - 50;
	App->physics->wall1.body.y = 0;
	App->physics->wall1.body.h = SCREEN_HEIGHT;
	App->physics->wall1.body.w = 50;*/

	//Set PLAYER body
	App->physics->canon.canonBody.x = 50;
	App->physics->canon.canonBody.h = 20;
	App->physics->canon.canonBody.y = SCREEN_HEIGHT - 70 - 240;
	App->physics->canon.canonBody.w = 20;

	//Set PLAYER2 body
	App->physics->canon2.canonBody.x = SCREEN_WIDTH - 50;
	App->physics->canon2.canonBody.h = 20;
	App->physics->canon2.canonBody.y = SCREEN_HEIGHT - 70 - 240;
	App->physics->canon2.canonBody.w = 20;

	screen.x = 0;
	screen.y = 0;
	screen.h = SCREEN_HEIGHT;
	screen.w = SCREEN_WIDTH;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// ----------------------------------------------------------------------------------------- DRAW BACKGROUND
	App->renderer->DrawQuad(screen, 200, 200, 200, 255);

	// ----------------------------------------------------------------------------------------- DRAW TERRAIN
	App->physics->current_terrain = App->physics->terrain_list->getFirst();
	while (App->physics->current_terrain != NULL)
	{

		App->renderer->DrawQuad({ App->physics->current_terrain->data->x, App->physics->current_terrain->data->y, App->physics->current_terrain->data->w, App->physics->current_terrain->data->h },
			App->physics->current_terrain->data->r, App->physics->current_terrain->data->g, App->physics->current_terrain->data->b, App->physics->current_terrain->data->a);

		App->physics->current_terrain = App->physics->current_terrain->next;
	}

	// ----------------------------------------------------------------------------------------- DRAW PLAYERS
	if(App->physics->canon.alive)
		App->renderer->DrawQuad(App->physics->canon.canonBody, 0, 0, 255, 255);
	if(App->physics->canon2.alive)
		App->renderer->DrawQuad(App->physics->canon2.canonBody, 255, 0, 0, 255);
	
	
	// ----------------------------------------------------------------------------------------- DRAW BALLS
	App->physics->current_ball = App->physics->ball_list->getFirst();
	while (App->physics->current_ball != NULL)
	{

		App->renderer->DrawCircle(App->physics->current_ball->data->x, App->physics->current_ball->data->y, App->physics->current_ball->data->rad, 
			App->physics->current_ball->data->r, App->physics->current_ball->data->g, App->physics->current_ball->data->b, App->physics->current_ball->data->a);


		App->physics->current_ball = App->physics->current_ball->next;
	}

	// ----------------------------------------------------------------------------------------- DRAW WEAPONS
	if (App->physics->canon.weapon1)
	{
		App->renderer->Blit(ballTex, 50, 50);

	}
	else if (App->physics->canon.weapon2)
	{
		App->renderer->Blit(mineTex, 50, 50);

	}
	else if (App->physics->canon.weapon3)
	{
		App->renderer->Blit(pumpTex, 50, 50);

	}

	if (App->physics->canon2.weapon1)
	{
		App->renderer->Blit(ballTex, SCREEN_WIDTH - 130, 50);

	}
	else if (App->physics->canon2.weapon2)
	{
		App->renderer->Blit(mineTex, SCREEN_WIDTH - 130, 50);

	}
	else if (App->physics->canon2.weapon3)
	{
		App->renderer->Blit(pumpTex, SCREEN_WIDTH - 130, 50);

	}

	// ----------------------------------------------------------------------------------------- DRAW WINS
	if (App->physics->canon.win)
	{
		App->renderer->Blit(p1winTex, 200, 0);
		App->renderer->Blit(restartTex, 100, 400);

	}
	if (App->physics->canon2.win)
	{
		App->renderer->Blit(p2winTex, 200, 0);
		App->renderer->Blit(restartTex, 100, 400);

	}

	// ----------------------------------------------------------------------------------------- DRAW VEL
	// P1
	App->renderer->DrawQuad({50, 500, App->physics->initialVelocity1 * 2, 20}, 255, 255, 255, 255);

	// P2
	App->renderer->DrawQuad({875, 500, App->physics->initialVelocity2 * 2, 20}, 255, 255, 255, 255);

	// ----------------------------------------------------------------------------------------- DRAW ANGLE
	// P1
	App->renderer->DrawQuad({ 50, 700, 20, App->physics->angle1INT}, 255, 255, 255, 255);

	// P2
	App->renderer->DrawQuad({ 875, 700, 20, -App->physics->angle2INT}, 255, 255, 255, 255);

	return UPDATE_CONTINUE;
}
