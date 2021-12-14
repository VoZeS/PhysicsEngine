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

	//Set ground1 body
	App->physics->ground1.body.x = 0;
	App->physics->ground1.body.h = 50;
	App->physics->ground1.body.y = SCREEN_HEIGHT - App->physics->ground1.body.h;
	App->physics->ground1.body.w = SCREEN_WIDTH/5;

	//Set ground2 body
	App->physics->ground2.body.x = SCREEN_WIDTH- SCREEN_WIDTH / 6;
	App->physics->ground2.body.h = 50;
	App->physics->ground2.body.y = SCREEN_HEIGHT - App->physics->ground1.body.h;
	App->physics->ground2.body.w = SCREEN_WIDTH / 6;

	//Set wall1 body
	/*App->physics->wall1.body.x = SCREEN_WIDTH - 50;
	App->physics->wall1.body.y = 0;
	App->physics->wall1.body.h = SCREEN_HEIGHT;
	App->physics->wall1.body.w = 50;*/

	//Set PLAYER body
	App->physics->canon.canonBody.x = 50;
	App->physics->canon.canonBody.h = 20;
	App->physics->canon.canonBody.y = SCREEN_HEIGHT - App->physics->ground1.body.h - 20;
	App->physics->canon.canonBody.w = 20;

	//Set PLAYER2 body
	App->physics->canon2.canonBody.x = SCREEN_WIDTH - 50;
	App->physics->canon2.canonBody.h = 20;
	App->physics->canon2.canonBody.y = SCREEN_HEIGHT - App->physics->ground1.body.h - 20;
	App->physics->canon2.canonBody.w = 20;

	//Set water body
	App->physics->lake.waterBody.x = SCREEN_WIDTH/5;
	App->physics->lake.waterBody.h = 50;
	App->physics->lake.waterBody.y = SCREEN_HEIGHT - App->physics->ground1.body.h+5;
	App->physics->lake.waterBody.w = SCREEN_WIDTH - (SCREEN_WIDTH/5)-170;

	//Set iceberg body
	App->physics->iceberg.body.x = (SCREEN_WIDTH/2)-60;
	App->physics->iceberg.body.h = 200;
	App->physics->iceberg.body.y = SCREEN_HEIGHT - 200;
	App->physics->iceberg.body.w = 120;

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
	int alpha = 100;
	App->renderer->Blit(backgroundTex, 0, 0, &screen);
	App->renderer->DrawQuad(App->physics->ground1.body, 0, 255, 255, alpha);
	App->renderer->DrawQuad(App->physics->ground2.body, 0, 255, 255, alpha);
	//App->renderer->DrawQuad(App->physics->wall1.body, 0, 255, 255, alpha);
	App->renderer->DrawQuad(App->physics->lake.waterBody, 100, 100, 255, alpha);

	App->renderer->DrawQuad(App->physics->canon.canonBody, 255, 0, 255, 255);
	App->renderer->DrawQuad(App->physics->canon2.canonBody, 255, 0, 255, 255);
	App->renderer->DrawQuad(App->physics->iceberg.body, 255, 0, 255, 255);
	
	
	
	App->physics->current_ball = App->physics->ball_list->getFirst();
	while (App->physics->current_ball != NULL)
	{

		App->renderer->DrawCircle(App->physics->current_ball->data->ballRect.x, App->physics->current_ball->data->ballRect.y, 5, 255, 255, 0, 255);

		App->physics->current_ball = App->physics->current_ball->next;
	}

	return UPDATE_CONTINUE;
}
