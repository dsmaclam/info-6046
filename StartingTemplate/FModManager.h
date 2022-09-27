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

	bool is_okay(const bool show_error = true) const;

public:
	//lifecycle
	FModManager();
	bool initialize(const int number_of_channels, const int system_flags);
	void shutdown();

	//channel groups
	bool create_channel_group(const std::string& name);
	void remove_channel_group(const std::string& name);
	bool set_channel_group_parent(const std::string& child_name, const std::string& parent_name);

	//sounds
	bool create_sound(const std::string& name, const std::string& path, const int mode);
	bool play_sound(const std::string& sound_name, const std::string& channel_group_name)
	{
		const auto sound_iterator = sounds_.find(sound_name);
		const auto channel_group_iterator = channel_groups_.find(channel_group_name);

		if(sound_iterator == sounds_.end() || channel_group_iterator == channel_groups_.end())
		{
			return false;
		}

		FMOD::Channel* channel;
		last_result_ = system_->playSound(sound_iterator->second, channel_group_iterator->second, true, &channel);
		if(!is_okay())
		{
			return false;
		}

		//more stuff to come here next class

		last_result_ = (*channel).setPaused(false);
		if (!is_okay())
		{
			return false;
		}

		return true;
	}
};
