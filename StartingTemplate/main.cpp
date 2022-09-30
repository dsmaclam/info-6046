#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SoundUI.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

GLFWwindow* window;

#include "FModManager.h"

constexpr int max_channels = 255;

//initialize our sound manager
FModManager fmod_manager;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		fmod_manager.play_sound("arrow", "fx");
	}

	if(key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		fmod_manager.add_dsp_effect("master", "echo");
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		fmod_manager.remove_dsp_effect("master", "echo");
	}
}

int main(int argc, char* argv[])
{
	//initialize glfw/glad
	glfwInit();
	window = glfwCreateWindow(800, 600, "INFO-6064", nullptr, nullptr);

	if(!window)
	{
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		return 2;
	}

	glfwSetKeyCallback(window, key_callback);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//initialize imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	//platform / render bindings
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 460"))
	{
		return 3;
	}

	//imgui style (dark mode for the win)
	ImGui::StyleColorsDark();

	//initialize fmod with max channels
	if (!fmod_manager.initialize(max_channels, FMOD_INIT_NORMAL))
	{
		return -1;
	}

	//create channel groups
	if (
		!fmod_manager.create_channel_group("master") ||
		!fmod_manager.create_channel_group("music") ||
		!fmod_manager.create_channel_group("fx")
		)
	{
		return -2;
	}

	//set parents for channel groups
	if(!fmod_manager.set_channel_group_parent("music", "master") || ! fmod_manager.set_channel_group_parent("fx", "master"))
	{
		return -3;
	}

	//create a sound object
	if(!fmod_manager.create_sound("bg", "./sounds/music/bg.wav", FMOD_LOOP_NORMAL))
	{
		return -4;
	}

	if (!fmod_manager.create_sound("arrow", "./sounds/fx/air_raid.wav", FMOD_DEFAULT))
	{
		return -4;
	}

	//play our bg sound
	if(!fmod_manager.play_sound("bg", "music"))
	{
		return -5;
	}

	//dsp effects
	if(!fmod_manager.create_dsp("echo", FMOD_DSP_TYPE_ECHO, 500.0f))
	{
		return -6;
	}

	//create sound ui
	SoundUI sound_ui(&fmod_manager);

	//game loop
	while(!glfwWindowShouldClose(window))
	{
		//poll for user events
		glfwPollEvents();

		//clear the back buffer
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		sound_ui.render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//present to the user
		glfwSwapBuffers(window);
	}

	fmod_manager.shutdown();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}
