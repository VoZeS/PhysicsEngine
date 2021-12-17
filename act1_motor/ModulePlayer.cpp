#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "PhysBody.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

update_status ModulePlayer::Update()
{
	// Set Canon POSITION
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (playerTurn == 0)
		{
			if (App->physics->canon.canonBody.x > 0)
				App->physics->canon.canonBody.x -= 4;
			else
				App->physics->canon.canonBody.x = 0;
		}
		if (playerTurn == 1)
		{
			if (App->physics->canon2.canonBody.x > SCREEN_WIDTH - SCREEN_WIDTH / 5)
				App->physics->canon2.canonBody.x -= 4;
			else
				App->physics->canon2.canonBody.x = SCREEN_WIDTH - SCREEN_WIDTH / 5;
				
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (playerTurn == 0)
		{
			if (App->physics->canon.canonBody.x < (SCREEN_WIDTH / 5) - App->physics->canon.canonBody.w)
				App->physics->canon.canonBody.x += 4;
			else
				App->physics->canon.canonBody.x = (SCREEN_WIDTH / 5) - App->physics->canon.canonBody.w;
		}
			

		if (playerTurn == 1)
		{
			if (App->physics->canon2.canonBody.x < SCREEN_WIDTH - App->physics->canon2.canonBody.w)
				App->physics->canon2.canonBody.x += 4;
			else
				App->physics->canon2.canonBody.x = SCREEN_WIDTH - App->physics->canon2.canonBody.w;
		}
	}

	// SET GOD MODE
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		if (!godMode)
			godMode = true;
		else
			godMode = false;
	}
	return UPDATE_CONTINUE;
}



