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
	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Set wall1 body
	/*App->physics->wall1.body.x = SCREEN_WIDTH - 50;
	App->physics->wall1.body.y = 0;
	App->physics->wall1.body.h = SCREEN_HEIGHT;
	App->physics->wall1.body.w = 50;*/

	//Set PLAYER body
	App->physics->canon.canonBody.x = 50;
	App->physics->canon.canonBody.h = 20;
	App->physics->canon.canonBody.y = SCREEN_HEIGHT - 70;
	App->physics->canon.canonBody.w = 20;

	//Set PLAYER2 body
	App->physics->canon2.canonBody.x = SCREEN_WIDTH - 50;
	App->physics->canon2.canonBody.h = 20;
	App->physics->canon2.canonBody.y = SCREEN_HEIGHT - 70;
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
	App->renderer->Blit(backgroundTex, 0, 0, &screen);
	// ----------------------------------------------------------------------------------------- DRAW TERRAIN
	App->physics->current_terrain = App->physics->terrain_list->getFirst();
	while (App->physics->current_terrain != NULL)
	{

		App->renderer->DrawQuad({ App->physics->current_terrain->data->x, App->physics->current_terrain->data->y, App->physics->current_terrain->data->w, App->physics->current_terrain->data->h },
			App->physics->current_terrain->data->r, App->physics->current_terrain->data->g, App->physics->current_terrain->data->b, App->physics->current_terrain->data->a);

		App->physics->current_terrain = App->physics->current_terrain->next;
	}

	// ----------------------------------------------------------------------------------------- DRAW PLAYERS
	App->renderer->DrawQuad(App->physics->canon.canonBody, 255, 0, 255, 255);
	App->renderer->DrawQuad(App->physics->canon2.canonBody, 255, 0, 255, 255);
	
	
	// ----------------------------------------------------------------------------------------- DRAW BALLS
	App->physics->current_ball = App->physics->ball_list->getFirst();
	while (App->physics->current_ball != NULL)
	{

		App->renderer->DrawCircle(App->physics->current_ball->data->x, App->physics->current_ball->data->y, 5, 255, 255, 0, 255);

		App->physics->current_ball = App->physics->current_ball->next;
	}

	return UPDATE_CONTINUE;
}
