#pragma once

#include <cstdio>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>

#include <map>
#include <string>

class FModManager
{
protected:
	FMOD_RESULT last_result_;
	FMOD::System* system_;

	std::map<std::string, FMOD::ChannelGroup*> channel_groups_;
	std::map<std::string, FMOD::Sound*> sounds_;
	std::map<std::string, FMOD::DSP*> dsps_;

	bool is_okay(const bool show_error = true) const;

public:
	//lifecycle
	FModManager();
	bool initialize(const int number_of_channels, const int system_flags);
	void shutdown();

	//channel groups
	bool create_channel_group(const std::string& name);
	bool find_channel_group(const std::string& name, FMOD::ChannelGroup** channel_group);
	void remove_channel_group(const std::string& name);
	bool set_channel_group_parent(const std::string& child_name, const std::string& parent_name);
	bool get_channel_group_volume(const std::string& name, float* volume);
	bool set_channel_group_volume(const std::string& name, float volume);
	bool get_channel_group_enabled(const std::string& name, bool* enabled);
	bool set_channel_group_enabled(const std::string& name, bool enabled);
	bool add_dsp_effect(const std::string& channel_group_name, const std::string& effect_name);
	bool remove_dsp_effect(const std::string& channel_group_name, const std::string& effect_name);

	//sounds
	bool create_sound(const std::string& name, const std::string& path, const int mode);
	bool play_sound(const std::string& sound_name, const std::string& channel_group_name);

	//dsp
	bool create_dsp(const std::string& name, FMOD_DSP_TYPE dsp_type, const float value);
};
