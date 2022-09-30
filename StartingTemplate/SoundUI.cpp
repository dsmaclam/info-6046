#include "SoundUI.h"
#include "imgui/imgui.h"

void SoundUI::render()
{
	//setup ui structure
	ImGui::Begin("Audio Settings");

	{
		FMOD::ChannelGroup* channel_group;
		if (!fmod_manager_->find_channel_group("master", &channel_group))
		{
			//cannot find channel group
			//do something...
		}

		float current_volume;
		if (!fmod_manager_->get_channel_group_volume("master", &current_volume)) {
			//failed
		}
		current_volume *= 100;
		ImGui::SliderFloat("master volume", &current_volume, 0.0f, 100.0f, "%.0f");
		current_volume /= 100;

		// WARNING: volume range (0.0-1.0) //dont go to high above 1 (you can hurt your hardware)
		if (!fmod_manager_->set_channel_group_volume("master", current_volume)) {
			//failed
		}

		if (!fmod_manager_->get_channel_group_enabled("master", &master_volume_enabled_)) {
			//failed
		}

		ImGui::SameLine();
		ImGui::Checkbox("##master_volume", &master_volume_enabled_);

		if (!fmod_manager_->set_channel_group_enabled("master", master_volume_enabled_)) {
			//failed
		}
	}
	
	//BREAK UNTIL 2:10pm

	ImGui::End();
	
}
