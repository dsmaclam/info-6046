#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

#include "FModManager.h"

constexpr int max_channels = 255;

int main(int argc, char* argv[])
{
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

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	FModManager fmod_manager;

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

	//play our bg sound
	if(!fmod_manager.play_sound("bg", "music"))
	{
		return -5;
	}

	//game loop
	while(!glfwWindowShouldClose(window))
	{
		//poll for user events
		glfwPollEvents();

		//clear the back buffer
		glClear(GL_COLOR_BUFFER_BIT);

		//present to the user
		glfwSwapBuffers(window);
		
	}

	fmod_manager.shutdown();
	glfwTerminate();
	return 0;
}
