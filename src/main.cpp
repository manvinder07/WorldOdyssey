
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"

#include <iostream>
#include <stdio.h>


using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;

	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window);
	world.init(&renderer);

    std::cout << "here0" << std::endl;
	// setup fonts
	std::string font_filename = "";
#if __APPLE__
	font_filename = "..//data//fonts//Kenney_Pixel_Square.ttf";
#elif __WINDOWS__
	font_filename = "..//..//..//data//fonts//Kenney_Pixel_Square.ttf";
#endif
	unsigned int font_default_size = 48;
	renderer.fontInit(font_filename, font_default_size);

	std::cout << "here1" << std::endl;

	// variable timestep loop
	auto t = Clock::now();
	auto last_fps_update = t;
	float counter = 0;
	world.fps = 0;
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		//std::string FPS_display = "FPS: " + std::to_string(1000 / elapsed_ms);
		float since_last_fps = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - last_fps_update)).count() / 1000;
		if (since_last_fps > 1000) {
			float new_fps = counter;
			world.fps = new_fps;
			last_fps_update = now;
			counter = 0;
		}
		counter++;
        gameConfig.fail_screen_pause_time -= elapsed_ms;
        gameConfig.success_screen_pause_time -= elapsed_ms;

        if (!gameConfig.show_help && gameConfig.fail_screen_pause_time <= 0 && gameConfig.success_screen_pause_time <= 0) {
            world.step(elapsed_ms);
            physics.step(elapsed_ms);
            world.handle_collisions();
        }
		renderer.draw();
	}

	return EXIT_SUCCESS;
}
