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
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->physics->canon.canonBody.x >= 0)
	{
		App->physics->canon.canonBody.x--;

	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->physics->canon.canonBody.x <= SCREEN_WIDTH / 4)
	{
		App->physics->canon.canonBody.x++;

	}
	return UPDATE_CONTINUE;
}



