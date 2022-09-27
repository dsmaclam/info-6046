#include "FModManager.h"

bool FModManager::is_okay(const bool show_error) const
{
	if(last_result_ != FMOD_OK)
	{
		printf("fmod error: #%d-%s\n", last_result_, FMOD_ErrorString(last_result_));
	}
	

	return last_result_ == FMOD_OK;
}

FModManager::FModManager(): last_result_(FMOD_OK), system_(nullptr)
{}

bool FModManager::initialize(const int number_of_channels, const int system_flags)
{
	last_result_ = System_Create(&system_);
	if(!is_okay())
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
	if(system_)
	{
		system_->release();
		system_ = nullptr;
	}
}

bool FModManager::create_channel_group(const std::string& name)
{
	FMOD::ChannelGroup* channel_group;
	last_result_ = system_->createChannelGroup(name.c_str(), &channel_group);
	if(!is_okay())
	{
		return false;
	}

	channel_groups_.try_emplace(name, channel_group);

	return true;
}

void FModManager::remove_channel_group(const std::string& name)
{
	const auto iterator = channel_groups_.find(name);
	if(iterator == channel_groups_.end())
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
		
	if(child_group == channel_groups_.end() || parent_group == channel_groups_.end())
	{
		return false;
	}

	last_result_ = parent_group->second->addGroup(child_group->second);
		
	return true;
}

bool FModManager::create_sound(const std::string& name, const std::string& path, const int mode)
{
	FMOD::Sound* sound;
	last_result_ = system_->createSound(path.c_str(), mode, nullptr, &sound);
	if(!is_okay())
	{
		return false;
	}

	sounds_.try_emplace(name, sound);

	return true;
}
