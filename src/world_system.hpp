#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"

enum class LevelType {
    FOREST_LEVEL,
    DESERT_LEVEL,
	ICE_LEVEL
};

#define LEVEL_TRANSITION_TIME_IN_MS 3000.0f

struct Level {

    LevelType type;

    float next_eagle_spawn;
    bool eagle_spawn_enable;
	
    float next_scorpion_spawn;
	bool scorpion_spawn_enable;

    float next_snake_spawn;
    bool snake_spawn_enable;

    float next_bug_spawn;
    bool bug_spawn_enable;

	float next_ice_monster_1_spawn;
	bool ice_monster_1_spawn_enable;

	float next_ice_monster_2_spawn;
	bool ice_monster_2_spawn_enable;

	float next_boss_spawn = 100.0f;
    bool boss_spawn_enable;
	
    float next_forest_enemy_spawn;
    bool forest_enemy_enable;

    float next_forest_boss_spawn = 100.0f;
    bool forest_boss_enable;

	float next_ice_boss_spawn = 100.0f;
	bool ice_boss_enable;

	float next_obstacle_spawn;

	Entity player_protagonist;

	Entity background_entity;
	Entity background_entity2;
	
    Entity foreground_entity;
	Entity foreground_entity2;
	
	Entity help_text_entity_0;
	Entity help_text_entity_1;
	Entity help_text_entity_2;

    vec3 rocks_color;

 };


// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	bool display_fps = true;
	float fps;

	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

    void createExplosionAnimation();

    void createDragonAnimation();

    void createTornadoAnimation();
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

	// restart level
	void restart_game(LevelType level_type, bool reset_stats);

    // Update the inventory
    void updateInventory();

	// OpenGL window handle
	GLFWwindow* window;

	// Number of bug eaten by the chicken, displayed in the window title
	unsigned int points;

	unsigned int scorpionsKilled;
    unsigned int snakesKilled;
    unsigned int spiderKilled;
	unsigned int iceMonster1Killed;
	unsigned int iceMonster2Killed;
    unsigned int forestBossesKilled;
    unsigned int desertBossesKilled;
	unsigned int iceBossesKilled;

	// Game state
	RenderSystem* renderer;

	float current_speed;
	Level currentLevel;
    float on_level_transition_timer;
    
	// music references
	Mix_Music* background_music;
	Mix_Chunk* game_over_sound;
	Mix_Chunk* player_jump_sound;
	Mix_Chunk* player_attack_sound;
    Mix_Chunk* clapping_sound;
    Mix_Chunk* explosion_sound;
    Mix_Chunk* player_win_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
