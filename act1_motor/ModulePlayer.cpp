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
				App->physics->canon.canonBody.x--;
			else
				App->physics->canon.canonBody.x == 0;
		}
		if (playerTurn == 1)
		{
			App->physics->canon2.canonBody.x--;
				
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (playerTurn == 0)
			App->physics->canon.canonBody.x++;

		if (playerTurn == 1)
		{
			if (App->physics->canon2.canonBody.x < SCREEN_WIDTH)
				App->physics->canon2.canonBody.x++;
			else
				App->physics->canon2.canonBody.x == SCREEN_WIDTH;
		}


	}
	return UPDATE_CONTINUE;
}



