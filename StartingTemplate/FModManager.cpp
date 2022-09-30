#include "FModManager.h"

bool FModManager::is_okay(const bool show_error) const
{
	if (last_result_ != FMOD_OK)
	{
		printf("fmod error: #%d-%s\n", last_result_, FMOD_ErrorString(last_result_));
	}

	return last_result_ == FMOD_OK;
}

FModManager::FModManager() : last_result_(FMOD_OK), system_(nullptr)
{}

bool FModManager::initialize(const int number_of_channels, const int system_flags)
{
	last_result_ = System_Create(&system_);
	if (!is_okay())
	{
		return false;
	}

	last_result_ = system_->init(number_of_channels, system_flags, nullptr);
	if (!is_okay())
	{
		return false;
	}

	return true;
}

void FModManager::shutdown()
{
	for (auto iterator = dsps_.begin(); iterator != dsps_.end(); ++iterator)
	{
		iterator->second->release();
	}

	for (auto iterator = sounds_.begin(); iterator != sounds_.end(); ++iterator)
	{
		iterator->second->release();
	}
	sounds_.clear();

	for (auto iterator = channel_groups_.begin(); iterator != channel_groups_.end(); ++iterator)
	{
		iterator->second->release();
	}
	channel_groups_.clear();

	if (system_)
	{
		system_->release();
		system_ = nullptr;
	}
}

bool FModManager::create_channel_group(const std::string& name)
{
	FMOD::ChannelGroup* channel_group;
	last_result_ = system_->createChannelGroup(name.c_str(), &channel_group);
	if (!is_okay())
	{
		return false;
	}

	channel_groups_.try_emplace(name, channel_group);

	return true;
}

bool FModManager::find_channel_group(const std::string& name, FMOD::ChannelGroup** channel_group)
{
	const auto iterator = channel_groups_.find(name);
	if (iterator == channel_groups_.end())
	{
		return false;
	}

	*channel_group = iterator->second;

	return true;
}

void FModManager::remove_channel_group(const std::string& name)
{
	const auto iterator = channel_groups_.find(name);
	if (iterator == channel_groups_.end())
	{
		return;
	}

	iterator->second->release();
	channel_groups_.erase(iterator);
}

bool FModManager::set_channel_group_parent(const std::string& child_name, const std::string& parent_name)
{
	const auto child_group = channel_groups_.find(child_name);
	const auto parent_group = channel_groups_.find(parent_name);

	if (child_group == channel_groups_.end() || parent_group == channel_groups_.end())
	{
		return false;
	}

	last_result_ = parent_group->second->addGroup(child_group->second);

	return true;
}

bool FModManager::get_channel_group_volume(const std::string& name, float* volume)
{
	const auto iterator = channel_groups_.find(name);
	if (iterator == channel_groups_.end())
	{
		return false;
	}

	return is_okay(iterator->second->getVolume(volume));
}

bool FModManager::set_channel_group_volume(const std::string& name, float volume)
{
	const auto iterator = channel_groups_.find(name);
	if (iterator == channel_groups_.end())
	{
		return false;
	}

	return is_okay(iterator->second->setVolume(volume));
}

bool FModManager::get_channel_group_enabled(const std::string& name, bool* enabled)
{
	const auto iterator = channel_groups_.find(name);
	if (iterator == channel_groups_.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->getMute(enabled)))
	{
		return false;
	}

	*enabled = !(*enabled);

	return true;
}

bool FModManager::set_channel_group_enabled(const std::string& name, bool enabled)
{
	const auto iterator = channel_groups_.find(name);
	if (iterator == channel_groups_.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->setMute(!enabled)))
	{
		return false;
	}

	return true;
}

bool FModManager::add_dsp_effect(const std::string& channel_group_name, const std::string& effect_name)
{
	const auto channel_group_iterator = channel_groups_.find(channel_group_name);
	const auto dsp_effect_iterator = dsps_.find(effect_name);
	if (channel_group_iterator == channel_groups_.end() || dsp_effect_iterator == dsps_.end())
	{
		return false;
	}

	int num_dsp;
	if(!is_okay(channel_group_iterator->second->getNumDSPs(&num_dsp)))
	{
		return false;
	}

	if(!is_okay(channel_group_iterator->second->addDSP(num_dsp, dsp_effect_iterator->second)))
	{
		return false;
	}

	return true;
}

bool FModManager::remove_dsp_effect(const std::string& channel_group_name, const std::string& effect_name)
{
	const auto channel_group_iterator = channel_groups_.find(channel_group_name);
	const auto dsp_effect_iterator = dsps_.find(effect_name);
	if (channel_group_iterator == channel_groups_.end() || dsp_effect_iterator == dsps_.end())
	{
		return false;
	}

	if(!is_okay(channel_group_iterator->second->removeDSP(dsp_effect_iterator->second)))
	{
		return false;
	}

	return true;
}


bool FModManager::create_sound(const std::string& name, const std::string& path, const int mode)
{
	FMOD::Sound* sound;
	last_result_ = system_->createSound(path.c_str(), mode, nullptr, &sound);
	if (!is_okay())
	{
		return false;
	}

	sounds_.try_emplace(name, sound);

	return true;
}

bool FModManager::play_sound(const std::string& sound_name, const std::string& channel_group_name)
{
	const auto sound_iterator = sounds_.find(sound_name);
	const auto channel_group_iterator = channel_groups_.find(channel_group_name);

	if (sound_iterator == sounds_.end() || channel_group_iterator == channel_groups_.end())
	{
		return false;
	}

	FMOD::Channel* channel;
	last_result_ = system_->playSound(sound_iterator->second, channel_group_iterator->second, true, &channel);
	if (!is_okay())
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

bool FModManager::create_dsp(const std::string& name, FMOD_DSP_TYPE dsp_type, const float value)
{
	FMOD::DSP* dsp;

	//figure out the kind we are creating
	switch (dsp_type)
	{
	case FMOD_DSP_TYPE_ECHO:
		if (!is_okay(system_->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp))) {
			return false;
		}
		break;
	default:
		return false;
	}

	dsps_.try_emplace(name, dsp);
	return true;
}
