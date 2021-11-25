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

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Set ground body
	App->physics->ground.body.x = 0;
	App->physics->ground.body.h = 50;
	App->physics->ground.body.y = SCREEN_HEIGHT - App->physics->ground.body.h;
	App->physics->ground.body.w = SCREEN_WIDTH;

	//Set wall1 body
	App->physics->wall1.body.x = SCREEN_WIDTH - 50;
	App->physics->wall1.body.y = 0;
	App->physics->wall1.body.h = SCREEN_HEIGHT;
	App->physics->wall1.body.w = 50;

	//Set canon body
	App->physics->canon.canonBody.x = 50;
	App->physics->canon.canonBody.h = 50;
	App->physics->canon.canonBody.y = SCREEN_HEIGHT - App->physics->ground.body.h - 50;
	App->physics->canon.canonBody.w = 50;

	//Set ball body
	App->physics->ball.ballRect.x = App->physics->canon.canonBody.x;
	App->physics->ball.ballRect.y = SCREEN_HEIGHT - App->physics->ground.body.h;
	App->physics->ball.ballRect.h = 5;
	App->physics->ball.ballRect.w = 5;

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
	App->renderer->DrawQuad(App->physics->ground.body, 0, 255, 255, alpha);
	App->renderer->DrawQuad(App->physics->wall1.body, 0, 255, 255, alpha);

	App->renderer->DrawQuad(App->physics->canon.canonBody, 255, 0, 255, 255);
	App->renderer->DrawQuad(App->physics->ball.ballRect, 255, 255, 0, 255);

	return UPDATE_CONTINUE;
}
