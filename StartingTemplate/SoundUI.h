#pragma once
#include "FModManager.h"

class SoundUI
{
protected:
	FModManager* fmod_manager_;

	bool master_volume_enabled_;

public:
	SoundUI(FModManager* f_manager) : fmod_manager_(f_manager), master_volume_enabled_(true) {}
	void render();
};
