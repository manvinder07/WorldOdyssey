// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include "physics_system.hpp"

// Game configuration

const size_t MAX_EAGLES = 15;
const size_t MAX_BUG = 5;
const size_t MAX_SCORPIONS = 10; // previus value 10
const size_t MAX_SNAKES = 10; // previus value 5
const size_t MAX_SPIDERS = 10;
const size_t MAX_ICE_1 = 10;
const size_t MAX_ICE_2 = 10;
const size_t EAGLE_DELAY_MS = 2000 * 3;
const size_t BUG_DELAY_MS = 5000 * 3;
const float SWORD_DAMAGE = 25.f;
const size_t SCORPION_DELAY_MS = 3000 * 3;
const size_t SNAKE_DELAY_MS = 2500 * 3;
const size_t ICE1_DELAY_MS = 3000 * 3;
const size_t ICE2_DELAY_MS = 2500 * 3;
const size_t BOSS_DELAY_MS = 2500 * 3;
const size_t OBSTACLE_DELAY_MS = 1000 * 3;
const size_t MAX_FOREST_BOSSES = 1;
const size_t MAX_DESERT_BOSSES = 1;
const size_t MAX_ICE_BOSSES = 1;

float protagonist_center_pos_y = GROUND_POSITION - PROTAGONIST_BB_HEIGHT / 2;
float fps;
int currentForestBosses;
int currentDesertBosses;
int currentIceBosses;
int scorpionsSpawned;
int ice1Spawned;
int ice2Spawned;
int snakesSpawned;
int forestEnemySpawned;
int obstaclesSpawned;
bool chickenFalling;
bool keyPressed;
bool forestBossAlive;
bool forestbossKilled = false;
bool desertBossAlive;
bool desertBossKilled = false;
bool iceBossAlive;
bool iceBossKilled = false;
float timeSinceBulletSpawn = 0;
float timeSinceGrenadeSpawn = 0;
float timeSinceSnowballSpawn = 0;
float timeSinceTornadoSpawn = 0;
float timeSinceExplosionSwitch = 0;
float timeSinceDragonSwitch = 0;
float grenadeSpawnTime = 5000;
float snowballSpawnTime = 5000;
float tornadoSpawnTime = 5000;
int currentPlayerSprite = 0;
int currentExplosionSprite = 0;
int currentDragonSprite = 0;
int prevDragonSprite = 1;
bool playerMoving = false;
float timeSincePlayerWalk = 500;
int numRocks = 0;
vec2 foregroundVelocity;
Entity forestBoss;
Entity desertBoss;
Entity iceBoss;

// List of color for the items
#define MAX_ARMOR_LEVEL 4
vec3 color_list[MAX_ARMOR_LEVEL] = {
	// bronze
	{205 / 255.f, 127 / 255.f, 50 / 255.f},
	// silver
	{192 / 255.f, 192 / 255.f, 192 / 255.f},
	// gold 
	{234 / 255.f, 190 / 255.f, 63 / 255.f},
	// diamond
	{145 / 255.f, 252 / 255.f, 265 / 255.f}
};

// Create the bug world
WorldSystem::WorldSystem()
	: points(0)
	, scorpionsKilled(0)
    , snakesKilled(0)
    , spiderKilled(0)
	, iceMonster1Killed(0)
	, iceMonster2Killed(0)
	, iceBossesKilled(0)
    , forestBossesKilled(0)
    , desertBossesKilled(0) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (game_over_sound != nullptr)
		Mix_FreeChunk(game_over_sound);
	if (player_jump_sound != nullptr)
		Mix_FreeChunk(player_jump_sound);
	if (player_attack_sound != nullptr)
		Mix_FreeChunk(player_attack_sound);
    if (clapping_sound != nullptr)
        Mix_FreeChunk(clapping_sound);
    if (explosion_sound != nullptr)
        Mix_FreeChunk(explosion_sound);
    if (player_win_sound != nullptr)
        Mix_FreeChunk(player_win_sound);

	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char* desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "World Odyssey", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	game_over_sound = Mix_LoadWAV(audio_path("game_over.wav").c_str());
	player_jump_sound = Mix_LoadWAV(audio_path("jump.wav").c_str());
	player_attack_sound = Mix_LoadWAV(audio_path("attack.wav").c_str());
    clapping_sound = Mix_LoadWAV(audio_path("clapping.wav").c_str());
    explosion_sound = Mix_LoadWAV(audio_path("bomb.wav").c_str());
    player_win_sound= Mix_LoadWAV(audio_path("win.wav").c_str());

	if (background_music == nullptr || game_over_sound == nullptr || player_jump_sound == nullptr
        || player_attack_sound == nullptr || clapping_sound == nullptr || explosion_sound == nullptr || player_win_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("game_over.wav").c_str(),
			audio_path("jump.wav").c_str(),
			audio_path("attack.wav").c_str(),
            audio_path("clapping.wav").c_str(),
            audio_path("bomb.wav").c_str(),
            audio_path("win.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::updateInventory() {
	// Update inventory


#define MAX_BOX_IN_INVENTORY 6
#define BOX_DIMENSION 64
#define ITEM_DIMENSION 48
#define LINE_DIM 4

// Create inventory background
	vec3 background_color = vec3(163 / 255.f, 124 / 255.f, 73 / 255.f) * 0.7f;
	vec2 background_dim = vec2(BOX_DIMENSION * MAX_BOX_IN_INVENTORY, BOX_DIMENSION);
	vec2 background_pos = vec2(window_width_px, 460 + window_height_px) * 0.5f - background_dim * 0.5f;
	createInventoryTexture(renderer, TEXTURE_ASSET_ID::WHITE, background_pos, background_dim, background_color);

	vec3 box_color = vec3(240 / 255.f, 216 / 255.f, 180 / 255.f) * 0.7f;
	// Render boxes
	for (unsigned int box_index = 0; box_index < MAX_BOX_IN_INVENTORY; ++box_index) {
		// Top line
		vec2 line_pos = vec2(background_pos.x + box_index * BOX_DIMENSION, background_pos.y);
		vec2 line_dim = vec2(BOX_DIMENSION, LINE_DIM);
		createInventoryTexture(renderer, TEXTURE_ASSET_ID::WHITE, line_pos, line_dim, box_color);
		// Bottom line
		line_pos = vec2(background_pos.x + box_index * BOX_DIMENSION, background_pos.y + BOX_DIMENSION);
		line_dim = vec2(BOX_DIMENSION, LINE_DIM);
		createInventoryTexture(renderer, TEXTURE_ASSET_ID::WHITE, line_pos, line_dim, box_color);
		// Right line
		line_pos = vec2(background_pos.x + box_index * BOX_DIMENSION + BOX_DIMENSION, background_pos.y);
		line_dim = vec2(LINE_DIM, BOX_DIMENSION);
		createInventoryTexture(renderer, TEXTURE_ASSET_ID::WHITE, line_pos, line_dim, box_color);
		// Left line
		line_pos = vec2(background_pos.x + box_index * BOX_DIMENSION, background_pos.y);
		line_dim = vec2(LINE_DIM, BOX_DIMENSION);
		createInventoryTexture(renderer, TEXTURE_ASSET_ID::WHITE, line_pos, line_dim, box_color);

	}

	// Draw items in the inventory

    assert(registry.healths.has(currentLevel.player_protagonist));
    Health health = registry.healths.get(currentLevel.player_protagonist);
    unsigned int num_shields = health.armor_level; //clamp((int)floorf(health.health / (int)PROTAGONIST_HEALTH), 0, (int)MAX_ARMOR_LEVEL);

    Entity gunEnt = registry.guns.entities[0];
    assert(registry.guns.has(gunEnt));
    Gun& gun = registry.guns.get(gunEnt);
    unsigned int num_damage = clamp((int)floorf(gun.damage / BULLET_DAMAGE), 0, (int)MAX_ARMOR_LEVEL);
        
    unsigned int offset = (BOX_DIMENSION/2 - ITEM_DIMENSION/2);
    vec2 item_dim = vec2(ITEM_DIMENSION, ITEM_DIMENSION);
    vec2 item_pos = vec2(background_pos.x + 0 * BOX_DIMENSION + offset, background_pos.y + offset);
    createInventoryTexture(renderer, TEXTURE_ASSET_ID::SWORD, item_pos, item_dim, color_list[num_damage > 0 ? num_damage-1 : 0]);
    item_pos = vec2(background_pos.x + 1 * BOX_DIMENSION + offset, background_pos.y + offset);
    createInventoryTexture(renderer, TEXTURE_ASSET_ID::SHIELD, item_pos, item_dim, color_list[num_shields > 0 ? num_shields-1 : 0]);
        
    // Draw Player lifes

    int num_lives = max((int)ceilf(health.health / PROTAGONIST_HEALTH), 0);
    bool half_heart = (health.health - (num_lives * PROTAGONIST_HEALTH) >= 0.5f);

    int last_index = 0;

    // Draw completes hearts

    for(int live_index = 0; live_index < num_lives; ++live_index) {
        vec2 item_dim = vec2(24, 24);
        vec2 item_pos = vec2(background_pos.x + live_index * 32, background_pos.y - 32);
        createInventoryTexture(renderer, TEXTURE_ASSET_ID::HEART, item_pos, item_dim);
        last_index = live_index + 1;
    }

    // Draw completes half hearts

   /* if (half_heart > 0) {
        vec2 item_dim = vec2(12, 24);
        vec2 item_pos = vec2(background_pos.x + last_index * 32, background_pos.y - 32);
        createInventoryTexture(renderer, TEXTURE_ASSET_ID::HEART_L, item_pos, item_dim);
    
    }
	*/

}

void WorldSystem::createExplosionAnimation() {
    for (Entity explosionE : registry.explosions.entities) {
        Explosion explosion = registry.explosions.get(explosionE);
        RenderRequest& explosionRR = registry.renderRequests.get(explosionE);
        if (timeSinceExplosionSwitch >= explosion.timeSwitch) {
            timeSinceExplosionSwitch = 0;
            if (currentExplosionSprite == registry.explosion_sprites.size()) {
                currentExplosionSprite = 0;
                registry.remove_all_components_of(explosionE);
                return;
            }
            explosionRR.used_texture = registry.explosion_sprites[currentExplosionSprite].used_texture;
            currentExplosionSprite++;
        }
    }
}

void WorldSystem::createDragonAnimation() {
    for (Entity dragonE : registry.desertBosses.entities) {
        DesertBoss dragon = registry.desertBosses.get(dragonE);
        RenderRequest& desertBossRR = registry.renderRequests.get(dragonE);
        if (timeSinceDragonSwitch >= dragon.timeSwitch) {
            timeSinceDragonSwitch = 0;
            int prev = prevDragonSprite;
            prevDragonSprite = currentDragonSprite;
            if (currentDragonSprite == 1) {
                if (prev == 0) {
                    currentDragonSprite++;
                } else if (prev == 2) {
                    currentDragonSprite--;
                }
            } else if (currentDragonSprite == 0) {
                currentDragonSprite++;
            } else if (currentDragonSprite == 2) {
                currentDragonSprite--;
            }
            desertBossRR.used_texture = registry.dragon_sprites[currentDragonSprite].used_texture;
        }
    }
}

void WorldSystem::createTornadoAnimation() {
    for (Entity tornadoE : registry.tornados.entities) {
        if (registry.tornados.has(tornadoE)) {
            Tornado& tornado = registry.tornados.get(tornadoE);
            RenderRequest& tornadoRR = registry.renderRequests.get(tornadoE);
            if (tornado.timeSinceSwitch >= tornado.timeSwitch) {
                tornado.timeSinceSwitch = 0;
                tornado.spriteIndex++;
                tornado.spriteIndex = tornado.spriteIndex % registry.tornado_sprites.size();
                tornadoRR.used_texture = registry.tornado_sprites[tornado.spriteIndex].used_texture;
            }
        }
    }
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game(LevelType::FOREST_LEVEL, true);
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
    gameConfig.did_user_fail = false;
    gameConfig.fail_screen_pause_time = 0.0f;
    gameConfig.did_user_succeed = false;
    gameConfig.success_screen_pause_time = 0.f;

	timeSinceBulletSpawn -= elapsed_ms_since_last_update;
    timeSinceGrenadeSpawn += elapsed_ms_since_last_update;
	timeSinceSnowballSpawn += elapsed_ms_since_last_update;
    timeSinceExplosionSwitch += elapsed_ms_since_last_update;
    timeSinceDragonSwitch += elapsed_ms_since_last_update;
    timeSinceTornadoSpawn += elapsed_ms_since_last_update;

    for (Entity tornadoE : registry.tornados.entities) {
        Tornado& tornado = registry.tornados.get(tornadoE);
        tornado.timeSinceSwitch += elapsed_ms_since_last_update;
    }

    // Remove grenade if it touches the ground
    for (Entity grenadeE : registry.grenades.entities) {
        Grenade grenade = registry.grenades.get(grenadeE);
        Motion grenadeMotion = registry.motions.get(grenadeE);
        if (grenadeMotion.position.y >= (GROUND_POSITION - (GRENADE_BB_HEIGHT / 2))) {
            timeSinceExplosionSwitch = 0;
            Mix_PlayChannel(-1, explosion_sound, 0);
            createExplosion(renderer, grenadeMotion.position);
            registry.remove_all_components_of(grenadeE);
        } 
		/*else if (grenadeMotion.position.y <= (GRENADE_BB_HEIGHT / 2)) {
            registry.remove_all_components_of(grenadeE);
        }
		*/
    }

	for (Entity snowballE : registry.snowballs.entities) {
		Snowball snowball = registry.snowballs.get(snowballE);
		Motion snowballMotion = registry.motions.get(snowballE);
		if (snowballMotion.position.y >= (GROUND_POSITION - (SNOWBALL_BB_HEIGHT / 2))) {
			registry.remove_all_components_of(snowballE);
		} 
		/*else if (snowballMotion.position.y <= (SNOWBALL_BB_HEIGHT / 2)) {
            registry.remove_all_components_of(snowballE);
        }
		*/
	}

    // Remove tornado if it touches the ground or flys out of bounds
    for (Entity tornadoE : registry.tornados.entities) {
        Tornado tornado = registry.tornados.get(tornadoE);
        Motion tornadoMotion = registry.motions.get(tornadoE);
        if (tornadoMotion.position.y >= (GROUND_POSITION - (TORNADO_BB_HEIGHT / 2))) {
            registry.remove_all_components_of(tornadoE);
        } else if (tornadoMotion.position.y <= (TORNADO_BB_HEIGHT / 2)) {
            registry.remove_all_components_of(tornadoE);
        }
    }

    Entity player_protagonist = currentLevel.player_protagonist;

    // Spawn a grenade after every grenadeSpawnTime and if forest boss is still alive
    if (forestBossAlive && (timeSinceGrenadeSpawn >= grenadeSpawnTime)) {
        timeSinceGrenadeSpawn = 0;
        createGrenade(renderer, forestBoss, player_protagonist);
    }

	// Spawn a snowball after every snowball spawn time and if ice boss is still alive
	if (iceBossAlive && (timeSinceSnowballSpawn >= snowballSpawnTime)) {
		timeSinceSnowballSpawn = 0;
		createSnowball(renderer, iceBoss, player_protagonist);
	}

    // Spawn a grenade after every tornadoSpawnTime and if desert boss is still alive
    if (desertBossAlive && (timeSinceTornadoSpawn >= tornadoSpawnTime)) {
        // (registry.tornados.entities.size() == 0)
        timeSinceTornadoSpawn = 0;
        createTornado(renderer, desertBoss, player_protagonist);
    }

    createExplosionAnimation();
    createDragonAnimation();
    createTornadoAnimation();

	// Animate player movement
	RenderRequest& playerRR = registry.renderRequests.get(player_protagonist);
	if (!playerMoving) {
		playerRR.used_texture = registry.player_sprites[3].used_texture;
		timeSincePlayerWalk = 500;
	}
	else {
		timeSincePlayerWalk -= elapsed_ms_since_last_update;
		if (timeSincePlayerWalk <= 0) {
			currentPlayerSprite = (currentPlayerSprite + 1) % 3;
			playerRR.used_texture = registry.player_sprites[currentPlayerSprite].used_texture;
			timeSincePlayerWalk = 500;
		}
	}

	// Animate player damage
	if (registry.damageTimers.has(player_protagonist)) {

		float f = sinf((registry.damageTimers.get(player_protagonist).counter_ms / DAMAGE_TIMER_TIME) * 100.0f);
		f = (f + 1.0f) / 2.0f; // Map f from -1 to +1 range into 0 to 1 range

		vec3 animColor = vec3(1, 0, 0) * f + vec3(1, 1, 1) * (1.0f - f);
		if (!registry.colors.has(player_protagonist)) {
			registry.colors.emplace_with_duplicates(player_protagonist, animColor);
		}
		else {
			vec3& color = registry.colors.get(player_protagonist);
			color = animColor;
		}
	}
	else {
		registry.colors.remove(player_protagonist);
	}

	// Updating window title with points
	std::stringstream title_ss;
	if (display_fps) {
		title_ss << "  FPS: " << fps;
	}
	glfwSetWindowTitle(window, title_ss.str().c_str());
	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

    // Extract needed entities from the level
    Entity background_entity = currentLevel.background_entity;
    Entity background_entity2 = currentLevel.background_entity2;
    Entity foreground_entity = currentLevel.foreground_entity;
    Entity foreground_entity2 = currentLevel.foreground_entity2;

	Motion& backgroundmotion1 = registry.motions.get(background_entity);
	Motion& backgroundmotion2 = registry.motions.get(background_entity2);

	Motion& playerMotion = registry.motions.get(player_protagonist);


	if (backgroundmotion1.position.x + backgroundmotion1.scale.x / 2 < playerMotion.position.x - static_cast<float>(window_width_px) / 2) { // First background is out of view
		backgroundmotion1.position.x = backgroundmotion2.position.x + backgroundmotion2.scale.x;
	}
	else if (backgroundmotion1.position.x - backgroundmotion1.scale.x / 2 > playerMotion.position.x + static_cast<float>(window_width_px) / 2) {
		backgroundmotion1.position.x = backgroundmotion2.position.x - backgroundmotion2.scale.x;

	}
	if (backgroundmotion2.position.x + backgroundmotion2.scale.x / 2 < playerMotion.position.x - static_cast<float>(window_width_px) / 2) {
		backgroundmotion2.position.x = backgroundmotion1.position.x + backgroundmotion1.scale.x;
	}
	else if (backgroundmotion2.position.x - backgroundmotion2.scale.x / 2 > playerMotion.position.x + static_cast<float>(window_width_px) / 2) {
		backgroundmotion2.position.x = backgroundmotion1.position.x - backgroundmotion1.scale.x;
	}

	/*if (backgroundmotion1.position.x + window_width_px / 2 < 0) { // First background is out of view
		backgroundmotion1.position.x = backgroundmotion2.position.x + backgroundmotion2.scale.x;
	} else if (backgroundmotion1.position.x > window_width_px * 3 / 2) {
		backgroundmotion1.position.x = backgroundmotion2.position.x - backgroundmotion2.scale.x;
	}
	if (backgroundmotion2.position.x + window_width_px / 2 < 0) { // First background is out of view
		backgroundmotion2.position.x = backgroundmotion1.position.x + backgroundmotion1.scale.x;
	} else if (backgroundmotion2.position.x > window_width_px * 3 / 2) {
		backgroundmotion2.position.x = backgroundmotion1.position.x - backgroundmotion1.scale.x;
	}
	*/

	Motion& foregroundmotion1 = registry.motions.get(foreground_entity);
	Motion& foregroundmotion2 = registry.motions.get(foreground_entity2);

	if (foregroundmotion1.position.x + foregroundmotion1.scale.x / 2 < playerMotion.position.x - static_cast<float>(window_width_px) / 2) { // First background is out of view
		foregroundmotion1.position.x = foregroundmotion2.position.x + foregroundmotion2.scale.x;
	}
	else if (foregroundmotion1.position.x - foregroundmotion1.scale.x / 2 > playerMotion.position.x + static_cast<float>(window_width_px) / 2) {
		foregroundmotion1.position.x = foregroundmotion2.position.x - foregroundmotion2.scale.x;
	}
	if (foregroundmotion2.position.x + foregroundmotion2.scale.x / 2 < playerMotion.position.x - static_cast<float>(window_width_px) / 2) {
		foregroundmotion2.position.x = foregroundmotion1.position.x + foregroundmotion1.scale.x;
	}
	else if (foregroundmotion2.position.x - foregroundmotion2.scale.x / 2 > playerMotion.position.x + static_cast<float>(window_width_px) / 2) {
		foregroundmotion2.position.x = foregroundmotion1.position.x - foregroundmotion1.scale.x;
	}

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	/*
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
		Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if(!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}
	*/


	// Check if player is standing on an obstacle
	bool isUnder = false;
	for (auto obstacle : registry.obstacles.entities) {
		Motion& playerMotion = registry.motions.get(player_protagonist);
		float playerLeft = playerMotion.position.x - PROTAGONIST_BB_WIDTH / 2;
		float playerRight = playerMotion.position.x + PROTAGONIST_BB_WIDTH / 2;
		float playerTop = playerMotion.position.y - PROTAGONIST_BB_HEIGHT / 2;
		float playerBottom = playerMotion.position.y + PROTAGONIST_BB_HEIGHT / 2;
		Mesh* mesh = registry.meshPtrs.get(obstacle);
		Motion& obstacleMotion = registry.motions.get(obstacle);
		/*float angleRad = -playerMotion.angle;
		glm::mat2 rotationMatrix = glm::mat2(cos(angleRad), -sin(angleRad), sin(angleRad), cos(angleRad));*/
		for (auto v : mesh->vertices) {
			vec2 transformedVertex = vec2(v.position.x * obstacleMotion.scale.x, v.position.y * obstacleMotion.scale.y);
			//vec2 rotatedVertex = rotationMatrix * scaledVertex;
			transformedVertex += obstacleMotion.position;
			/*printf("transformedvertex x %f transformedvertex y %f\n", transformedVertex.x, transformedVertex.y);
			printf("playerleft %f playerright %f playertop %f playerbottom %f\n", playerLeft, playerRight, playerTop, playerBottom);*/

			if (transformedVertex.x < playerRight && transformedVertex.x > playerLeft && transformedVertex.y < playerBottom + 1 && transformedVertex.y > playerBottom && transformedVertex.y > playerTop) {
				isUnder = true;
				break;
			}
		}
		if (isUnder)
			break;
	}

	if (!isUnder)
		protagonist_center_pos_y = GROUND_POSITION - PROTAGONIST_BB_HEIGHT / 2;

	//auto& foreground_velocity = registry.motions.get(foreground_entity).velocity;
	foregroundVelocity = registry.motions.get(foreground_entity).velocity;





	// make scorpions follow player
	if (!registry.deathTimers.has(player_protagonist)) {
		for (Entity ent : registry.deadlys.entities) {
			if (!registry.deathTimers.has(ent)) {

				Motion& deadlyMotion = registry.motions.get(ent);
				vec2 deadlyPosition = deadlyMotion.position;

				Motion& playerMotion = registry.motions.get(player_protagonist);
				vec2 playerPosition = playerMotion.position;
				bool isRockBetween = false;
				for (Entity rock : registry.obstacles.entities) {
					Motion& rockMotion = registry.motions.get(rock);
					vec2 rockPosition = rockMotion.position;

					if ((rockPosition.x < playerPosition.x && rockPosition.x > deadlyPosition.x) || (rockPosition.x > playerPosition.x && rockPosition.x < deadlyPosition.x)) {
						isRockBetween = true;
						break;
					}

				}

				Deadly& deadly = registry.deadlys.get(ent);
				if (isRockBetween) {
					deadly.followingPlayer = false;
					continue;
				}
				deadly.followingPlayer = true;

				// calculate direction from scorpion to player
				vec2 directionToPlayer = playerPosition - deadlyPosition;
				vec2 horizontalDirectionToPlayer = vec2(directionToPlayer.x, 0.0f);

				// normalize the direction vec2
				vec2 normalizedDirection = normalize(horizontalDirectionToPlayer);

				/*float deadlySpeed = 35.0f;
				if (abs(directionToPlayer.x) > 350) {
					deadlySpeed = 42.0f;
				}
				*/

				deadlyMotion.velocity = foregroundVelocity + normalizedDirection * abs(deadlyMotion.velocity);

                if (directionToPlayer.x > 0) {
                    deadlyMotion.scale = vec2(-abs(deadlyMotion.scale.x), abs(deadlyMotion.scale.y));
#if 0
					if (bossAlive) {
						deadlyMotion.scale = vec2(-BOSS_BB_WIDTH, BOSS_BB_HEIGHT); // facing right
					}
					else {
						deadlyMotion.scale = vec2(-SCORPION_BB_WIDTH, SCORPION_BB_HEIGHT); // facing right
					}
#endif
				}
				else {
                    deadlyMotion.scale = vec2(abs(deadlyMotion.scale.x), abs(deadlyMotion.scale.y));
#if 0
					if (bossAlive) {
						deadlyMotion.scale = vec2(BOSS_BB_WIDTH, BOSS_BB_HEIGHT); // facing left
					}
					else {
						deadlyMotion.scale = vec2(SCORPION_BB_WIDTH, SCORPION_BB_HEIGHT); // facing left
					}
#endif
				}

				float barLength = (deadlyMotion.scale.x - 20.0f);

                if (registry.healths.has(ent)) {
                    if ((forestBossAlive && registry.forestBosses.has(ent)) || (iceBossAlive && registry.iceBosses.has(ent)) || (desertBossAlive && registry.desertBosses.has(ent))) {
                        Health& bossHealth = registry.healths.get(ent);
						if (forestBossAlive) {
							barLength *= float(bossHealth.health / static_cast<double>(40));
						}
						else if (desertBossAlive) {
							barLength *= float(bossHealth.health / static_cast<double>(50));
						}
						else {
							barLength *= float(bossHealth.health / static_cast<double>(60));
						}
					}
                    else {
                        Health& enemyHealth = registry.healths.get(ent);
                        barLength *= float(enemyHealth.health / static_cast<double>(2));
						printf("Enemy health: %d\n", enemyHealth.health);
						printf("Bar length: %f\n", barLength);

                    }
                }

				// draw top line
				createLine({ deadlyMotion.position.x  , deadlyMotion.position.y - deadlyMotion.scale.y * 0.5f - 10.0f }, { barLength, 2.0f });
			}
		}
	}

    // Extract current level information
    float& next_scorpion_spawn = currentLevel.next_scorpion_spawn;
    float& next_snake_spawn = currentLevel.next_snake_spawn;
    float& next_boss_spawn = currentLevel.next_boss_spawn;
    float& next_forest_enemy_spawn = currentLevel.next_forest_enemy_spawn;
    float& next_forest_boss_spawn = currentLevel.next_forest_boss_spawn;
	float& next_ice_boss_spawn = currentLevel.next_ice_boss_spawn;
	float& next_ice_monster_1_spawn = currentLevel.next_ice_monster_1_spawn;
    float& next_ice_monster_2_spawn = currentLevel.next_ice_monster_2_spawn;


    bool scorpion_spawn_enable = currentLevel.scorpion_spawn_enable;
    bool snake_spawn_enable = currentLevel.snake_spawn_enable;
    bool boss_spawn_enable = currentLevel.boss_spawn_enable;
    bool forest_enemy_spawn_enable = currentLevel.forest_enemy_enable;
    bool forest_boss_spawn_enable = currentLevel.forest_boss_enable;
	bool ice_monster_1_spawn_enable = currentLevel.ice_monster_1_spawn_enable;
	bool ice_monster_2_spawn_enable = currentLevel.ice_monster_2_spawn_enable;
	bool ice_boss_spawn_enable = currentLevel.ice_boss_enable;


	// Spawning new scorpion
	next_scorpion_spawn -= elapsed_ms_since_last_update * current_speed;
	if (scorpion_spawn_enable && scorpionsSpawned < MAX_SCORPIONS && next_scorpion_spawn < 0.f) {
		next_scorpion_spawn = (SCORPION_DELAY_MS / 2) + uniform_dist(rng) * (SCORPION_DELAY_MS / 2);
		// - 100 is out of bounds at the top
		// random number between 0 and 1
		float randomVar = uniform_dist(rng);
		float xPos = playerMotion.position.x;
		if (randomVar < 0.5) {
			xPos = xPos + static_cast<float>(window_width_px) / 2;	// spawn on the right side of the screen
		}
		else {
			xPos = xPos - static_cast<float>(window_width_px) / 2;	// spawn on the left side of the screen
		}
		createScorpion(renderer, vec2(xPos, GROUND_POSITION - SCORPION_BB_HEIGHT / 3), foregroundVelocity); // div by 3 to make scorpion legs on platform
		// createScorpion(renderer, vec2(window_width_px, window_height_px - SCORPION_BB_HEIGHT*2));
		scorpionsSpawned++;
	}

    // Spawn forest enemy
    next_forest_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
	if (forest_enemy_spawn_enable && forestEnemySpawned < MAX_SPIDERS && next_forest_enemy_spawn < 0.f) {
		next_forest_enemy_spawn = (SCORPION_DELAY_MS / 2) + uniform_dist(rng) * (SCORPION_DELAY_MS / 2);
		// - 100 is out of bounds at the top
		// random number between 0 and 1
		float randomVar = uniform_dist(rng);
		float xPos = playerMotion.position.x;
		if (randomVar < 0.5) {
			xPos = xPos + static_cast<float>(window_width_px) / 2;	// spawn on the right side of the screen
		}
		else {
			xPos = xPos - static_cast<float>(window_width_px) / 2;	// spawn on the left side of the screen
		}
		createSpider(renderer, vec2(xPos, GROUND_POSITION - SCORPION_BB_HEIGHT / 3), foregroundVelocity); // div by 3 to make scorpion legs on platform
		// createScorpion(renderer, vec2(window_width_px, window_height_px - SCORPION_BB_HEIGHT*2));
		
        forestEnemySpawned++;
	}

	if (forest_boss_spawn_enable && spiderKilled == MAX_SPIDERS && currentForestBosses < MAX_FOREST_BOSSES) {
		next_forest_boss_spawn -= elapsed_ms_since_last_update * current_speed;
		// we are ready for the boss
		if (next_forest_boss_spawn < 0.f) {
			forestBoss = createForestBoss(renderer, vec2(playerMotion.position.x + static_cast<float>(window_width_px) / 2 + 100.0f, GROUND_POSITION - FOREST_BOSS_BB_HEIGHT / 2), foregroundVelocity);
            timeSinceGrenadeSpawn = 0;
            createGrenade(renderer, forestBoss, player_protagonist);
            currentForestBosses++;
			forestBossAlive = true;
		}

	}
	next_snake_spawn -= elapsed_ms_since_last_update * current_speed;

	if (snake_spawn_enable == true && scorpionsKilled == MAX_SCORPIONS) {
		if (snakesSpawned < MAX_SNAKES && next_snake_spawn < 0.f) {
			// spawn snakes
			next_snake_spawn = (SNAKE_DELAY_MS / 2) + uniform_dist(rng) * (SNAKE_DELAY_MS / 2);
			// - 100 is out of bounds at the top
			createSnake(renderer, vec2(playerMotion.position.x + static_cast<float>(window_width_px) / 2, GROUND_POSITION - SNAKE_BB_HEIGHT / 3 + 10.0f), foregroundVelocity); // div by 3 to make scorpion legs on platform
			// createScorpion(renderer, vec2(window_width_px, window_height_px - SCORPION_BB_HEIGHT*2));
			snakesSpawned++;
		}
	}

    if (boss_spawn_enable && snakesKilled == MAX_SNAKES && currentDesertBosses < MAX_DESERT_BOSSES) {
        next_boss_spawn -= elapsed_ms_since_last_update * current_speed;
        // we are ready for the boss
        if (next_boss_spawn < 0.f) {
            timeSinceDragonSwitch = 0;
            desertBoss = createDesertBoss(renderer, vec2(playerMotion.position.x + static_cast<float>(window_width_px) / 2 + 100.0f, GROUND_POSITION - 250.f), foregroundVelocity);
            currentDesertBosses++;
            desertBossAlive = true;
            timeSinceTornadoSpawn = 0;
            createTornado(renderer, desertBoss, player_protagonist);
        }
    }

	// Spawning new ice monster
	next_ice_monster_1_spawn -= elapsed_ms_since_last_update * current_speed;
	if (ice_monster_1_spawn_enable && ice1Spawned < MAX_ICE_1 && next_ice_monster_1_spawn < 0.f) {
		next_ice_monster_1_spawn = (ICE1_DELAY_MS / 2) + uniform_dist(rng) * (ICE1_DELAY_MS / 2);
		// - 100 is out of bounds at the top
		// random number between 0 and 1
		float randomVar = uniform_dist(rng);
		float xPos = playerMotion.position.x;
		if (randomVar < 0.5) {
			xPos = xPos + static_cast<float>(window_width_px) / 2;	// spawn on the right side of the screen
		}
		else {
			xPos = xPos - static_cast<float>(window_width_px) / 2;	// spawn on the left side of the screen
		}
		createIceMonster1(renderer, vec2(xPos, GROUND_POSITION - ICE1_BB_HEIGHT / 3 - 5.0f), foregroundVelocity); // div by 3 to make scorpion legs on platform
		// createScorpion(renderer, vec2(window_width_px, window_height_px - SCORPION_BB_HEIGHT*2));
		ice1Spawned++;
	}

	next_ice_monster_2_spawn -= elapsed_ms_since_last_update * current_speed;
	if (ice_monster_2_spawn_enable == true && iceMonster1Killed == MAX_ICE_1) {
		if (ice2Spawned < MAX_ICE_2 && next_ice_monster_2_spawn < 0.f) {
			next_ice_monster_2_spawn = (ICE2_DELAY_MS / 2) + uniform_dist(rng) * (ICE2_DELAY_MS / 2);
			// - 100 is out of bounds at the top
			createIceMonster2(renderer, vec2(playerMotion.position.x + static_cast<float>(window_width_px) / 2, GROUND_POSITION - ICE2_BB_HEIGHT / 3), foregroundVelocity); // div by 3 to make scorpion legs on platform
			// createScorpion(renderer, vec2(window_width_px, window_height_px - SCORPION_BB_HEIGHT*2));
			ice2Spawned++;
		}
	}

	
	

	if (ice_boss_spawn_enable == true && iceMonster2Killed == MAX_ICE_2 && currentIceBosses < MAX_ICE_BOSSES) {
		next_ice_boss_spawn -= elapsed_ms_since_last_update * current_speed;
		// we are ready for the boss
		if (next_ice_boss_spawn < 0.f) {
			iceBoss = createIceBoss(renderer, vec2(playerMotion.position.x + static_cast<float>(window_width_px) / 2 + 100.0f, GROUND_POSITION - ICE_BOSS_BB_HEIGHT / 2 + 20.0f), foregroundVelocity);
			timeSinceSnowballSpawn = 0;
			createSnowball(renderer, iceBoss, player_protagonist);
			currentIceBosses++;
			iceBossAlive = true;
		}

	}

	// once the boss reaches a third from the right of the screen, stop its movement
	/*
	if (forestBossAlive) {
		Entity entity = registry.forestbosses.entities[0];
		Motion& boss_motion = registry.motions.get(entity);
		 if (boss_motion.position.x <= (window_width_px / 3) * 2) {
			boss_motion.velocity.x = 0;

		}

	}
	*/

	// Spawn random obstacle
	//next_obstacle_spawn -= elapsed_ms_since_last_update * current_speed;
	//obstaclesSpawned = registry.obstacles.size();
	// printf("next_obstacle_spawn %f obstaclesSpawned %i\n", next_obstacle_spawn, obstaclesSpawned);
	
	vec2 rockPositionLeft = { 0, GROUND_POSITION };
	vec2 rockPositionRight = { window_width_px, GROUND_POSITION };
	vec2 rockPosition = { 0, GROUND_POSITION };
	
	
	while (numRocks < 50) {
		//	next_obstacle_spawn = (OBSTACLE_DELAY_MS / 2) + uniform_dist(rng) * (OBSTACLE_DELAY_MS / 2);
		GEOMETRY_BUFFER_ID rockId;
		float scaleMultiplier = 1;
		float randomVar = uniform_dist(rng);
		if (randomVar < 0.5) {
			rockPositionLeft.x -= window_width_px / 0.9 + uniform_dist(rng) * window_width_px / 0.9;
			rockPosition = { rockPositionLeft.x, GROUND_POSITION };
		}
		else {
			rockPositionRight.x += window_width_px / 0.9 + uniform_dist(rng) * window_width_px / 0.9;
			rockPosition = { rockPositionRight.x, GROUND_POSITION };
		}
		if (randomVar < 0.2) {
			rockId = GEOMETRY_BUFFER_ID::ROCK0;
			scaleMultiplier = 50.f;
		}
		else if (randomVar < 0.4) {
			rockId = GEOMETRY_BUFFER_ID::ROCK1;
			scaleMultiplier = 20.f;
		}
		else if (randomVar < 0.6) {
			rockId = GEOMETRY_BUFFER_ID::ROCK2;

		}
		else if (randomVar < 0.8) {
			rockId = GEOMETRY_BUFFER_ID::ROCK3;
		}
		else {
			rockId = GEOMETRY_BUFFER_ID::ROCK4;
		}

		Entity rock = createRock(renderer, rockPosition, rockId, scaleMultiplier, foregroundVelocity);
        registry.colors.insert(rock, currentLevel.rocks_color);
        //registry.colors.insert(rock, { 1, 0.8f, 0.8f });
		numRocks++;
	}

	// Creates the visual bounding boxes in debug mode
	if (debugging.in_debug_mode) {
		// Bugs bounding boxes
		//for (Entity e : registry.eatables.entities) {
		//	Motion& motion = registry.motions.get(e);
		//	vec2 bounding_box = { abs(motion.scale.x), abs(motion.scale.y) };
		//	bounding_box /= 2.f;

		//	// Left line
		//	vec2 leftLinePosition = { motion.position.x - bounding_box.x, motion.position.y };
		//	createLine(leftLinePosition, { 5, motion.scale.y });

		//	// Right line
		//	vec2 rightLinePosition = { motion.position.x + bounding_box.x, motion.position.y };
		//	createLine(rightLinePosition, { 5, motion.scale.y });

		//	// Top line
		//	vec2 topLinePosition = { motion.position.x, motion.position.y - bounding_box.y };
		//	createLine(topLinePosition, { motion.scale.x, 5 });

		//	// Bot line
		//	vec2 botLinePosition = { motion.position.x, motion.position.y + bounding_box.y };
		//	createLine(botLinePosition, { motion.scale.x, 5 });

		//	// Computed direction line if avoiding chicken
		//	Eatable& eatable = registry.eatables.get(e);
		//	if (eatable.isChased) {
		//		float xPosition;
		//		if (motion.velocity.x < 0) {
		//			xPosition = motion.position.x - bounding_box.x / 2;
		//		}
		//		else {
		//			xPosition = motion.position.x + bounding_box.x / 2;
		//		}
		//		createLine({ xPosition , motion.position.y }, { motion.scale.x / 2, 5 });
		//	}
		//}

		// Items bounding boxes
		for (Entity e : registry.items.entities) {
			Motion& motion = registry.motions.get(e);
			vec2 bounding_box = { abs(motion.scale.x), abs(motion.scale.y) };
			bounding_box /= 2.f;

			// Left line
			vec2 leftLinePosition = { motion.position.x - bounding_box.x, motion.position.y };
			Entity l = createLine(leftLinePosition, { 5, motion.scale.y });

			// Right line
			vec2 rightLinePosition = { motion.position.x + bounding_box.x, motion.position.y };
			Entity r = createLine(rightLinePosition, { 5, motion.scale.y });

			// Top line
			vec2 topLinePosition = { motion.position.x, motion.position.y - bounding_box.y };
			Entity t = createLine(topLinePosition, { motion.scale.x, 5 });

			// Bot line
			vec2 botLinePosition = { motion.position.x, motion.position.y + bounding_box.y };
			Entity b = createLine(botLinePosition, { motion.scale.x, 5 });
		}

		// Deadly bounding boxes
		for (Entity e : registry.deadlys.entities) {
			Motion& motion = registry.motions.get(e);
			vec2 bounding_box = { abs(motion.scale.x), abs(motion.scale.y) };
			bounding_box /= 2.f;

			// Left line
			vec2 leftLinePosition = { motion.position.x - bounding_box.x, motion.position.y };
			Entity l = createLine(leftLinePosition, { 5, motion.scale.y });

			// Right line
			vec2 rightLinePosition = { motion.position.x + bounding_box.x, motion.position.y };
			Entity r = createLine(rightLinePosition, { 5, motion.scale.y });

			// Top line
			vec2 topLinePosition = { motion.position.x, motion.position.y - bounding_box.y };
			Entity t = createLine(topLinePosition, { motion.scale.x, 5 });

			// Bot line
			vec2 botLinePosition = { motion.position.x, motion.position.y + bounding_box.y };
			Entity b = createLine(botLinePosition, { motion.scale.x, 5 });

			/*if (registry.bbLightUps.has(e)) {
				registry.colors.insert(l, { 0, 0, 8 });
				registry.colors.insert(r, { 0, 0, 8 });
				registry.colors.insert(t, { 0, 0, 8 });
				registry.colors.insert(b, { 0, 0, 8 });
			}*/

			//if (advancedMode) {
			//	// Ring within which eagle follows chicken
			//	for (float a = 0; a <= 2 * M_PI; a += M_PI / 6) {
			//		vec2 position = { cos(a), sin(a) };
			//		position *= SAFE_DISTANCE;
			//		Entity rl = createLine(motion.position + position, { 110, 5 });

			//		Motion& rlMotion = registry.motions.get(rl);
			//		rlMotion.angle = a + M_PI / 2;

			//		registry.colors.insert(rl, { 0, 8, 0 });
			//	}
			//}
		}

		// Obstacle bounding boxes
		for (Entity e : registry.obstacles.entities) {
			Motion& motion = registry.motions.get(e);
			vec2 bounding_box = { abs(motion.scale.x), abs(motion.scale.y) };
			bounding_box /= 2.f;

			// Left line
			vec2 leftLinePosition = { motion.position.x - bounding_box.x, motion.position.y };
			Entity l = createLine(leftLinePosition, { 5, motion.scale.y });

			// Right line
			vec2 rightLinePosition = { motion.position.x + bounding_box.x, motion.position.y };
			Entity r = createLine(rightLinePosition, { 5, motion.scale.y });

			// Top line
			vec2 topLinePosition = { motion.position.x, motion.position.y - bounding_box.y };
			Entity t = createLine(topLinePosition, { motion.scale.x, 5 });

			// Bot line
			vec2 botLinePosition = { motion.position.x, motion.position.y + bounding_box.y };
			Entity b = createLine(botLinePosition, { motion.scale.x, 5 });

			/*if (registry.bbLightUps.has(e)) {
				registry.colors.insert(l, { 0, 0, 8 });
				registry.colors.insert(r, { 0, 0, 8 });
				registry.colors.insert(t, { 0, 0, 8 });
				registry.colors.insert(b, { 0, 0, 8 });
			}*/
		}

		Motion& motion = registry.motions.get(player_protagonist);
		vec2 bounding_box = { PROTAGONIST_BB_WIDTH, PROTAGONIST_BB_HEIGHT };
		bounding_box /= 2.f;

		// Left line
		vec2 leftLinePosition = { motion.position.x - bounding_box.x, motion.position.y };
		Entity l = createLine(leftLinePosition, { 5, bounding_box.y * 2 });

		// Right line
		vec2 rightLinePosition = { motion.position.x + bounding_box.x, motion.position.y };
		Entity r = createLine(rightLinePosition, { 5, bounding_box.y * 2 });

		// Top line
		vec2 topLinePosition = { motion.position.x, motion.position.y - bounding_box.y };
		Entity t = createLine(topLinePosition, { bounding_box.x * 2, 5 });

		// Bot line
		vec2 botLinePosition = { motion.position.x, motion.position.y + bounding_box.y };
		Entity b = createLine(botLinePosition, { bounding_box.x * 2, 5 });

		/*if (registry.bbLightUps.has(player)) {
			registry.colors.insert(l, { 0, 0, 8 });
			registry.colors.insert(r, { 0, 0, 8 });
			registry.colors.insert(t, { 0, 0, 8 });
			registry.colors.insert(b, { 0, 0, 8 });
		}*/
	}

	updateInventory();
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG SPAWN HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Processing the chicken state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		float percentDead = 1 - counter.counter_ms / 3000.f;
        if (registry.colors.has(entity)) {
            vec3& color = registry.colors.get(entity);
            color = percentDead * vec3(1.0f, 0.f, 0.f) + (1 - percentDead) * vec3(1.0f, 1.0f, 1.0f);
        }
		if (counter.counter_ms < min_counter_ms && registry.players.has(entity)) {
			min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			if (registry.players.has(entity)) {
				screen.darken_screen_factor = 0;
                gameConfig.did_user_fail = true;
                gameConfig.fail_screen_pause_time = 3000.f;
				restart_game(LevelType::FOREST_LEVEL, true);
			}
			else if (registry.enemies.has(entity)) {
                
                // calculate next armor color to spawn
                assert(registry.healths.has(player_protagonist));
                Health health = registry.healths.get(player_protagonist);
                unsigned int health_count = floorf(health.health / PROTAGONIST_HEALTH);
                unsigned int next_shield_color = clamp((int)health.armor_level, 0, MAX_ARMOR_LEVEL); // clamp((int)health_count, 0, MAX_ARMOR_LEVEL-1);
                
                // calculate next sword color to spawn
                Entity gunEnt = registry.guns.entities[0];
                assert(registry.guns.has(gunEnt));
                Gun& gun = registry.guns.get(gunEnt);
                unsigned int sword_count = floorf(gun.damage / BULLET_DAMAGE);
                unsigned int next_sword_color = clamp((int)sword_count, 0, MAX_ARMOR_LEVEL-1);
                
                float random_choise = uniform_dist(rng) * 3;
                float random = uniform_dist(rng) * 100.0f;

                if(random <= 50.0f) {
                    if(random_choise < 1.0f) {
                        if(sword_count < MAX_ARMOR_LEVEL) {
                            createSword(renderer, entity, color_list[next_sword_color]);
                        } else {
                            random_choise = uniform_dist(rng) * 3;
                        }

                    } else if(random_choise < 2.0f) {
                        if(health.armor_level < MAX_ARMOR_LEVEL) {
                            createShield(renderer, entity, color_list[next_shield_color]);
                        } else {
                            random_choise = uniform_dist(rng) * 3;
                        }
                    } else {
                        if(health.health < MAX_PROTAGONIST_HEALTH) {
                            createHeart(renderer, entity, vec3(1, 1, 1));                    
                        }
                    }
                }

				registry.remove_all_components_of(entity);

			}
			else {
				registry.remove_all_components_of(entity);
			}
			return true;
		}
	}

	// progress damage timer
	for (Entity entity : registry.damageTimers.entities) {
		// progress timer
		DamageTimer& counter = registry.damageTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.damageTimers.remove(entity);
		}
	}

	// reduce window brightness if any of the present chickens is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// !!! TODO A1: update LightUp timers and remove if time drops below zero, similar to the death counter
	// iterate through all the LightUp entities
	for (Entity ent : registry.lightUps.entities) {
		LightUp& light_up = registry.lightUps.get(ent);
		light_up.counter_ms -= elapsed_ms_since_last_update;
		if (light_up.counter_ms < 0) {
			registry.lightUps.remove(ent);
		}
	}

    if (!registry.deathTimers.has(player_protagonist)) {
        if (iceBossesKilled >= MAX_ICE_BOSSES) {

            gameConfig.did_user_succeed = true;
            gameConfig.success_screen_pause_time = 5000.f;
            Mix_PlayChannel(-1, clapping_sound, 0);
            restart_game(LevelType::FOREST_LEVEL,  true);
            return true;
        }
    }

    // If the transition timer is on we update it
    if(on_level_transition_timer > 0) {
        on_level_transition_timer -= elapsed_ms_since_last_update;
        screen.darken_screen_factor = 1.0f - (on_level_transition_timer / LEVEL_TRANSITION_TIME_IN_MS);
        if(on_level_transition_timer <= 0) {
            // Time to change the level

			if (forestBossesKilled >= MAX_FOREST_BOSSES && desertBossesKilled < MAX_DESERT_BOSSES) {
				restart_game(LevelType::DESERT_LEVEL, false);
			}
			else if (desertBossesKilled >= MAX_DESERT_BOSSES && iceBossesKilled < MAX_ICE_BOSSES) {
				restart_game(LevelType::ICE_LEVEL, false);
			}
            screen.darken_screen_factor = 0;
        }
    }

	return true;
}

static Level createForestLevel(RenderSystem* renderer) {
    
    Level level = {};
    
    level.type = LevelType::FOREST_LEVEL;

    // Set level rocks color
    level.rocks_color = vec3( 82, 41, 6  ) * 1.0f/255.0f;

    level.next_forest_enemy_spawn = 100.0f;
    
    level.forest_enemy_enable = true;
    level.forest_boss_enable = true;
    
    // Extract needed entities from the level
    Entity &background_entity = level.background_entity;
    Entity &background_entity2 = level.background_entity2;
    Entity &foreground_entity = level.foreground_entity;
    Entity &foreground_entity2 = level.foreground_entity2;

    Entity &player_protagonist = level.player_protagonist;
    
	background_entity = createBackground(renderer, vec2(window_width_px / 2, window_height_px / 2 - PLATFORM_HEIGHT * 3 / 4), TEXTURE_ASSET_ID::BACKGROUND2);
	background_entity2 = createBackground(renderer, vec2(window_width_px + window_width_px / 2, window_height_px / 2 - PLATFORM_HEIGHT *3/4), TEXTURE_ASSET_ID::BACKGROUND2);
	foreground_entity = createForeground(renderer, vec2(window_width_px / 2, PLATFORM_CENTER), vec2(window_width_px, PLATFORM_HEIGHT), TEXTURE_ASSET_ID::FOREST_FOREGROUND);
	foreground_entity2 = createForeground(renderer, vec2(window_width_px + window_width_px / 2, PLATFORM_CENTER), vec2(window_width_px, PLATFORM_HEIGHT), TEXTURE_ASSET_ID::FOREST_FOREGROUND);

	level.help_text_entity_0 = createHelpText("Use Left and Right arrow keys to move,       ", vec2(window_width_px / 6, window_height_px * 1 / 10), { 0, 0 });
	level.help_text_entity_1 = createHelpText("                              Up key to jump,", vec2(window_width_px / 6, window_height_px * 2 / 10), { 0, 0 });
	level.help_text_entity_2 = createHelpText("           And Space to shoot enemies        ", vec2(window_width_px / 6, window_height_px * 3 / 10), { 0, 0 });

	// Create the protagonist
	vec2 protagonist_init_pos = {window_width_px/2, protagonist_center_pos_y + 50.0f };
	player_protagonist = createProtagonist(renderer, protagonist_init_pos);
	registry.colors.insert(player_protagonist, { 1, 0.8f, 0.8f });

    return level;
}

static Level createDesertLevel(RenderSystem* renderer) {

    Level level = {};

    level.type = LevelType::DESERT_LEVEL;

    // Set level rocks color
    level.rocks_color = { 1, 0.8f, 0.8f };

    level.next_boss_spawn = 100.0f;

    level.scorpion_spawn_enable = true;
    level.snake_spawn_enable = true;
    level.boss_spawn_enable = true;

    // Extract needed entities from the level
    Entity &background_entity = level.background_entity;
    Entity &background_entity2 = level.background_entity2;
    Entity &foreground_entity = level.foreground_entity;
    Entity &foreground_entity2 = level.foreground_entity2;

    Entity &player_protagonist = level.player_protagonist;
    
	background_entity = createBackground(renderer, vec2(window_width_px / 2, window_height_px / 2 - PLATFORM_HEIGHT * 3 / 4), TEXTURE_ASSET_ID::BACKGROUND);
	background_entity2 = createBackground(renderer, vec2(window_width_px + window_width_px / 2, window_height_px / 2 - PLATFORM_HEIGHT *3/4), TEXTURE_ASSET_ID::BACKGROUND);
	foreground_entity = createForeground(renderer, vec2(window_width_px / 2, PLATFORM_CENTER), vec2(window_width_px, PLATFORM_HEIGHT), TEXTURE_ASSET_ID::FOREGROUND);
	foreground_entity2 = createForeground2(renderer, vec2(window_width_px + window_width_px / 2, PLATFORM_CENTER), vec2(window_width_px, PLATFORM_HEIGHT));

	// Create the protagonist
	vec2 protagonist_init_pos = {window_width_px/2, protagonist_center_pos_y };
	player_protagonist = createProtagonist(renderer, protagonist_init_pos);
	registry.colors.insert(player_protagonist, { 1, 0.8f, 0.8f });
    
    return level;
}

static Level createIceLevel(RenderSystem* renderer) {

	Level level = {};

	level.type = LevelType::ICE_LEVEL;

	// Set level rocks color
	level.rocks_color = { 0, 128.0f, 128.0f };

	level.next_ice_monster_1_spawn = 100.0f;
	level.next_ice_monster_2_spawn = 100.0f;

	level.ice_monster_1_spawn_enable = true;
	level.ice_monster_2_spawn_enable = true;
	level.ice_boss_enable = true;

	// Extract needed entities from the level
	Entity& background_entity = level.background_entity;
	Entity& background_entity2 = level.background_entity2;
	Entity& foreground_entity = level.foreground_entity;
	Entity& foreground_entity2 = level.foreground_entity2;

	Entity& player_protagonist = level.player_protagonist;

	background_entity = createBackground(renderer, vec2(window_width_px / 2, window_height_px / 2 - PLATFORM_HEIGHT * 3 / 4), TEXTURE_ASSET_ID::ICE_BACKGROUND);
	background_entity2 = createBackground(renderer, vec2(window_width_px + window_width_px / 2, window_height_px / 2 - PLATFORM_HEIGHT * 3 / 4), TEXTURE_ASSET_ID::ICE_BACKGROUND);
	foreground_entity = createForeground(renderer, vec2(window_width_px / 2, PLATFORM_CENTER), vec2(window_width_px, PLATFORM_HEIGHT + 15.0f), TEXTURE_ASSET_ID::ICE_FOREGROUND);
	foreground_entity2 = createForeground(renderer, vec2(window_width_px + window_width_px / 2, PLATFORM_CENTER), vec2(window_width_px, PLATFORM_HEIGHT + 15.0f), TEXTURE_ASSET_ID::ICE_FOREGROUND);

	// Create the protagonist
	vec2 protagonist_init_pos = { window_width_px / 2, protagonist_center_pos_y };
	player_protagonist = createProtagonist(renderer, protagonist_init_pos);
	registry.colors.insert(player_protagonist, { 1, 0.8f, 0.8f });

	return level;
}

// Reset the world state to its initial state
void WorldSystem::restart_game(LevelType level_type, bool reset_stats) {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;
    
    Health saved_player_health;
    Gun saved_player_gun;
    if(!reset_stats) {
        saved_player_health = registry.healths.get(currentLevel.player_protagonist);
        Entity gunEnt = registry.guns.entities[0];
        assert(registry.guns.has(gunEnt));
        saved_player_gun = registry.guns.get(gunEnt);
    }

    
	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

    // Setup game levels
    if(level_type == LevelType::DESERT_LEVEL) {
        currentLevel = createDesertLevel(renderer);
        desertBossesKilled = 0;
        scorpionsKilled = 0;
        snakesKilled = 0;
        currentDesertBosses = 0;
        scorpionsSpawned = 0;
        snakesSpawned = 0;
        timeSinceTornadoSpawn = 0;
        desertBossKilled = false;
        desertBossAlive = false;
    } else if(level_type == LevelType::FOREST_LEVEL) {
        currentLevel = createForestLevel(renderer);
	}
	else if (level_type == LevelType::ICE_LEVEL) {
		currentLevel = createIceLevel(renderer);
	} else {
        assert(!"invalid level type!");
    }


    // Setup transition initial state
    on_level_transition_timer = 0;

	chickenFalling = false;
	keyPressed = false;

	scorpionsKilled = 0;
    snakesKilled = 0;
    spiderKilled = 0;
	iceMonster1Killed = 0;
	iceMonster2Killed = 0;
    forestBossesKilled = 0;
    desertBossesKilled = 0;
	iceBossesKilled = 0;
	currentForestBosses = 0;
    currentDesertBosses = 0;
	currentIceBosses = 0;
	scorpionsSpawned = 0;
	snakesSpawned = 0;
	ice1Spawned = 0;
	ice2Spawned = 0;
    forestEnemySpawned = 0;
	obstaclesSpawned = 0;
	numRocks = 0;
    timeSinceGrenadeSpawn = 0;
    timeSinceTornadoSpawn = 0;
	timeSinceSnowballSpawn = 0;
	forestbossKilled = false;
	forestBossAlive = false;
    desertBossKilled = false;
    desertBossAlive = false;
	iceBossKilled = false;
	iceBossAlive = false;


	createGun(renderer);
    
    if(!reset_stats) {
        Health& playerHealth = registry.healths.get(currentLevel.player_protagonist);
        playerHealth = saved_player_health;
        Entity gunEnt = registry.guns.entities[0];
        assert(registry.guns.has(gunEnt));
        Gun& gun = registry.guns.get(gunEnt);
        gun = saved_player_gun;
    }

}

void handle_player_enemy_weapon_collisions(Entity entity, Entity weapon) {
	if ((registry.players.has(entity) || registry.enemies.has(entity)) && (registry.healths.has(entity))) {
		Health& entityHealth = registry.healths.get(entity);
		if (registry.swords.has(weapon)) {
			entityHealth.health -= SWORD_DAMAGE;
		}
		else if (registry.bullets.has(weapon)) {
			entityHealth.health -= BULLET_DAMAGE;
		}
		if (entityHealth.health <= 0) {
			if (registry.players.has(entity)) {
				registry.deathTimers.emplace(entity);
			}
			else if (registry.enemies.has(entity)) {
				registry.remove_all_components_of(entity);
			}
		}
	}
}

void handle_player_item_collisions(Entity playerEnt, Entity itemEnt) {

	Player player = registry.players.get(playerEnt);
	Item item = registry.items.get(itemEnt);

    switch(item.type) {
        case Item::TYPE_ID::SWORD: {
            // Get gun entity
            Entity gunEnt = registry.guns.entities[0];
            // Some sanity check
            assert(registry.guns.has(gunEnt));
            Gun& gun = registry.guns.get(gunEnt);
            // Increase gun damage
            gun.damage += BULLET_DAMAGE;
        } break;
        case Item::TYPE_ID::SHIELD: {
            // Some sanity check
            assert(registry.healths.has(playerEnt));

            Health& health = registry.healths.get(playerEnt);

            if(health.armor_level < MAX_ARMOR_LEVEL) {
                health.armor_level += 1;
            }
        } break;
        case Item::TYPE_ID::HEART: {
            // Some sanity check
            assert(registry.healths.has(playerEnt));

            Health& health = registry.healths.get(playerEnt);

            // Round up the current player heath
            health.health = health.health - fmodf(health.health+PROTAGONIST_HEALTH, PROTAGONIST_HEALTH);
            // Increase player heath
            health.health += PROTAGONIST_HEALTH;

        } break;
    }
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;


		// Handle collisions between player/enemy and weapons
		// handle_player_enemy_weapon_collisions(entity, entity_other);

        if (registry.grenades.has(entity) &&
            (registry.players.has(entity_other) || registry.obstacles.has(entity_other))) {
            Grenade grenade = registry.grenades.get(entity);
            Motion grenadeMotion = registry.motions.get(entity);
            if (!registry.deathTimers.has(entity_other) && registry.players.has(entity_other)) {
                Health& playerHealth = registry.healths.get(entity_other);
                playerHealth.health -= (int) grenade.damage / playerHealth.armor_level;

                if (playerHealth.health <= 0) {
                    Mix_PlayChannel(-1, game_over_sound, 0);
                    registry.deathTimers.emplace(entity_other);
                    registry.colors.emplace_with_duplicates(entity_other, vec3(1.0f, 1.0f, 1.0f));
                    for (auto& motion : registry.motions.components) {
                        motion.velocity.x = 0.f;
                    }
                }
            }
            timeSinceExplosionSwitch = 0;
            Mix_PlayChannel(-1, explosion_sound, 0);
            createExplosion(renderer, grenadeMotion.position);
            registry.remove_all_components_of(entity);
        }


		if (registry.snowballs.has(entity) &&
			(registry.players.has(entity_other) || registry.obstacles.has(entity_other))) {
			Snowball snowball = registry.snowballs.get(entity);
			Motion snowballMotion = registry.motions.get(entity);
			if (!registry.deathTimers.has(entity_other) && registry.players.has(entity_other)) {
				Health& playerHealth = registry.healths.get(entity_other);
				playerHealth.health -= (int)snowball.damage / playerHealth.armor_level;

				if (playerHealth.health <= 0) {
					Mix_PlayChannel(-1, game_over_sound, 0);
					registry.deathTimers.emplace(entity_other);
					registry.colors.emplace_with_duplicates(entity_other, vec3(1.0f, 1.0f, 1.0f));
					for (auto& motion : registry.motions.components) {
						motion.velocity.x = 0.f;
					}
				}
			}
			registry.remove_all_components_of(entity);
		}


        if (registry.tornados.has(entity) &&
            (registry.players.has(entity_other) || registry.obstacles.has(entity_other))) {
            Tornado tornado = registry.tornados.get(entity);
            Motion tornadoMotion = registry.motions.get(entity);
            if (!registry.deathTimers.has(entity_other) && registry.players.has(entity_other)) {
                Health& playerHealth = registry.healths.get(entity_other);
                playerHealth.health -= (int) tornado.damage / playerHealth.armor_level;

                if (playerHealth.health <= 0) {
                    Mix_PlayChannel(-1, game_over_sound, 0);
                    registry.deathTimers.emplace(entity_other);
                    registry.colors.emplace_with_duplicates(entity_other, vec3(1.0f, 1.0f, 1.0f));
                    for (auto& motion : registry.motions.components) {
                        motion.velocity.x = 0.f;
                    }
                }
            }
            registry.remove_all_components_of(entity);
        }


		if (registry.obstacles.has(entity_other) && registry.enemies.has(entity)) {

			if (!registry.deathTimers.has(entity)) {
				Motion& deadlyMotion = registry.motions.get(entity);
				Motion& rockMotion = registry.motions.get(entity_other);

				if (deadlyMotion.position.x - rockMotion.position.x < 0) {
					// hits left side of rock
					deadlyMotion.position.x = rockMotion.position.x - abs(rockMotion.scale.x) / 2 - abs(deadlyMotion.scale.x) / 2;

				}
				else {
					// hits right side of rock
					deadlyMotion.position.x = rockMotion.position.x + abs(rockMotion.scale.x) / 2 + abs(deadlyMotion.scale.x) / 2;

				}

				Deadly& deadly = registry.deadlys.get(entity);
				if (!deadly.followingPlayer) {
					deadlyMotion.velocity.x = -1 * deadlyMotion.velocity.x;
					// Adjust scorpion scale to face left or right
                    if (deadlyMotion.velocity.x > 0) {
						deadlyMotion.scale.x = -1 * abs(deadlyMotion.scale.x); // facing right
					}
					else {
						deadlyMotion.scale.x = abs(deadlyMotion.scale.x); // facing left
					}
				}
			}
		}

		if (registry.obstacles.has(entity_other) && registry.forestBosses.has(entity)) {
			// boss destroys rock
			registry.remove_all_components_of(entity_other);

		}

		if (registry.obstacles.has(entity_other) && registry.iceBosses.has(entity)) {
			// boss destroys rock
			registry.remove_all_components_of(entity_other);

		}

		if (registry.bullets.has(entity) && registry.obstacles.has(entity_other)) {
			// rock destroys bullet
			registry.remove_all_components_of(entity);
		}

        if (registry.bullets.has(entity) && registry.deadlys.has(entity_other)) {
            if (!registry.deathTimers.has(entity_other)) {
                /*
                registry.deathTimers.emplace(entity_other);
                registry.colors.emplace_with_duplicates(entity_other, vec3(1.0f, 1.0f, 1.0f));
                */
                Motion &deadlyMotion = registry.motions.get(entity_other);
                /*
                deadlyMotion.velocity = vec2({0.0f, 0.0f});
                registry.remove_all_components_of(entity);
                if (registry.enemies.has(entity_other)) {
                    scorpionsKilled++;
                }
                */
                registry.remove_all_components_of(entity);
                Entity gun = registry.guns.entities[0];
                Gun& gunComponent = registry.guns.get(gun);

                if (forestBossAlive && registry.forestBosses.has(entity_other)) {
                    Health &bossHealth = registry.healths.get(entity_other);
                    bossHealth.health -= gunComponent.damage;
                    if (bossHealth.health <= 0) {
                        forestBossesKilled++;
                        forestBossAlive = false;
                        forestbossKilled = true;

                        if(currentLevel.forest_boss_enable) {
                            Mix_PlayChannel(-1, player_win_sound, 0);
                            on_level_transition_timer = LEVEL_TRANSITION_TIME_IN_MS;
                        }

                    } else {
                        continue;
                    }
                }
                if (desertBossAlive && registry.desertBosses.has(entity_other)) {
                    Health &bossHealth = registry.healths.get(entity_other);
                    bossHealth.health -= gunComponent.damage;
                    if (bossHealth.health <= 0) {
                        desertBossesKilled++;
                        desertBossAlive = false;
                        desertBossKilled = true;

						if (currentLevel.boss_spawn_enable) {
							Mix_PlayChannel(-1, player_win_sound, 0);
							on_level_transition_timer = LEVEL_TRANSITION_TIME_IN_MS;
						}

                    } else {
                        continue;
                    }
                }
				if (iceBossAlive && registry.iceBosses.has(entity_other)) {
					Health& bossHealth = registry.healths.get(entity_other);
					bossHealth.health -= gunComponent.damage;
					if (bossHealth.health <= 0) {
						iceBossesKilled++;
						iceBossAlive = false;
						iceBossKilled = true;
					}
					else {
						continue;
					}
				}
                if (registry.scorpions.has(entity_other)) {
                    Health &scorpionHealth = registry.healths.get(entity_other);
                    scorpionHealth.health -= gunComponent.damage;;
                    if (scorpionHealth.health <= 0) {
                        scorpionsKilled++;
                    } else {
                        continue;
                    }
                }
                if (registry.snakes.has(entity_other)) {
                    Health &snakeHealth = registry.healths.get(entity_other);
                    snakeHealth.health -= gunComponent.damage;;
                    if (snakeHealth.health <= 0) {
                        snakesKilled++;
                    } else {
                        continue;
                    }
                }
                if (registry.spiders.has(entity_other)) {
                    Health &spiderHealth = registry.healths.get(entity_other);
                    spiderHealth.health -= gunComponent.damage;
                    if (spiderHealth.health <= 0) {
                        spiderKilled++;
						if (spiderKilled == 1 && currentLevel.type == LevelType::FOREST_LEVEL) {
							registry.remove_all_components_of(currentLevel.help_text_entity_0);
							registry.remove_all_components_of(currentLevel.help_text_entity_1);
							registry.remove_all_components_of(currentLevel.help_text_entity_2);

							currentLevel.help_text_entity_0 = createHelpText("Dead enemies can drop damage boosts, armour, and hearts                ", vec2(window_width_px / 90, window_height_px * 1 / 10), { 0, 0 });
							currentLevel.help_text_entity_1 = createHelpText("           Kill the enemies and bosses of all 3 levels to win          ", vec2(window_width_px / 90, window_height_px * 2 / 10), { 0, 0 });
							currentLevel.help_text_entity_2 = createHelpText("                                           Press H for Help. Good Luck!", vec2(window_width_px / 90, window_height_px * 3 / 10), { 0, 0 });

							DeathTimer& d0 = registry.deathTimers.emplace(currentLevel.help_text_entity_0);
							d0.counter_ms = 20000;
							DeathTimer& d1 = registry.deathTimers.emplace(currentLevel.help_text_entity_1);
							d1.counter_ms = 20000;
							DeathTimer& d2 = registry.deathTimers.emplace(currentLevel.help_text_entity_2);
							d2.counter_ms = 20000;
						}
                    } else {
                        continue;
                    }
                }
				if (registry.ice1Monsters.has(entity_other)) {
					Health& ice1Health = registry.healths.get(entity_other);
					ice1Health.health -= gunComponent.damage;;
					if (ice1Health.health <= 0) {
						iceMonster1Killed++;
					}
					else {
						continue;
					}
				}
				if (registry.ice2Monsters.has(entity_other)) {
					Health& ice2Health = registry.healths.get(entity_other);
					ice2Health.health -= gunComponent.damage;;
					if (ice2Health.health <= 0) {
						iceMonster2Killed++;
						printf("Increments iceMonster2 killed\n");
					}
					else {
						continue;
					}
				}
                if (!registry.deathTimers.has(entity_other)) {
                    registry.deathTimers.emplace(entity_other);
                    deadlyMotion.velocity = foregroundVelocity;
                    registry.colors.emplace_with_duplicates(entity_other, vec3(1.0f, 1.0f, 1.0f));
                    registry.deadlys.remove(entity_other);
                }
            }
        }


		if (registry.players.has(entity)) {
			// Checking Player - Scorpion collisions
			if (registry.items.has(entity_other)) {
				// Cheking Player - Item collisions
				handle_player_item_collisions(entity, entity_other);
				// Remove item entity
				registry.remove_all_components_of(entity_other);

			}
			else if (registry.deadlys.has(entity_other)) {
				Motion& playerMotion = registry.motions.get(entity);
				Motion& otherMotion = registry.motions.get(entity_other);
				const vec2 player_bounding_box = { PROTAGONIST_BB_WIDTH / 2.f, PROTAGONIST_BB_HEIGHT / 2.f };
				const vec2 other_bounding_box = { abs(otherMotion.scale.x) / 2.f, abs(otherMotion.scale.y) / 2.f };

				float motion1Left = playerMotion.position.x - player_bounding_box.x;
				float motion1Right = playerMotion.position.x + player_bounding_box.x;
				float motion1Top = playerMotion.position.y - player_bounding_box.y;
				float motion1Bottom = playerMotion.position.y + player_bounding_box.y;

				float motion2Left = otherMotion.position.x - other_bounding_box.x;
				float motion2Right = otherMotion.position.x + other_bounding_box.x;
				float motion2Top = otherMotion.position.y - other_bounding_box.y;
				float motion2Bottom = otherMotion.position.y + other_bounding_box.y;

				if (motion1Top < motion2Bottom && motion2Top < motion1Bottom && motion1Left < motion2Right && motion2Left < motion1Right) {

					if (!registry.deathTimers.has(entity) && !registry.damageTimers.has(entity)) {
						assert(registry.healths.has(entity));
						Health& playerHealth = registry.healths.get(entity);
						
                        // Dependes the level we are in the damage we take
                        if(currentLevel.type == LevelType::FOREST_LEVEL) {
                            playerHealth.health -= forestBossAlive ? (FOREST_BOSS_DAMAGE / playerHealth.armor_level) : (BUG_DAMAGE / playerHealth.armor_level);
                        } else if(currentLevel.type == LevelType::DESERT_LEVEL) {
                                playerHealth.health -= desertBossAlive ? (DESERT_BOSS_DAMAGE / playerHealth.armor_level) : (SCORPION_DAMAGE / playerHealth.armor_level);
                        } else if (currentLevel.type == LevelType::ICE_LEVEL) {
							playerHealth.health -= iceBossAlive ? (ICE_BOSS_DAMAGE / playerHealth.armor_level) : (ICE1_DAMAGE / playerHealth.armor_level);
						} else {
                            assert(!"invalid level!");
                        }

                        // The player has just been damage so we add a timer so he can scape the scorpion
						registry.damageTimers.emplace(entity);

						if (playerHealth.health <= 0) {
							Mix_PlayChannel(-1, game_over_sound, 0);
							registry.deathTimers.emplace(entity);
							registry.colors.emplace_with_duplicates(entity, vec3(1.0f, 1.0f, 1.0f));
							for (auto& motion : registry.motions.components) {
								motion.velocity.x = 0.f;
							}
						}
					}
				}
			}
			else if (registry.obstacles.has(entity_other)) {
				// printf("bb collision with obstacle\n");

				Motion& playerMotion = registry.motions.get(entity);
				float playerLeft = playerMotion.position.x - PROTAGONIST_BB_WIDTH / 2;
				float playerRight = playerMotion.position.x + PROTAGONIST_BB_WIDTH / 2;
				float playerTop = playerMotion.position.y - PROTAGONIST_BB_HEIGHT / 2;
				float playerBottom = playerMotion.position.y + PROTAGONIST_BB_HEIGHT / 2;
				Mesh* mesh = registry.meshPtrs.get(entity_other);
				Motion& obstacleMotion = registry.motions.get(entity_other);
				bool isCollision = false;
				/*float angleRad = -playerMotion.angle;
				glm::mat2 rotationMatrix = glm::mat2(cos(angleRad), -sin(angleRad), sin(angleRad), cos(angleRad));*/
				for (auto v : mesh->vertices) {
					vec2 transformedVertex = vec2(v.position.x * obstacleMotion.scale.x, v.position.y * obstacleMotion.scale.y);
					//vec2 rotatedVertex = rotationMatrix * scaledVertex;
					transformedVertex += obstacleMotion.position;
					/*printf("transformedvertex x %f transformedvertex y %f\n", transformedVertex.x, transformedVertex.y);
					printf("playerleft %f playerright %f playertop %f playerbottom %f\n", playerLeft, playerRight, playerTop, playerBottom);*/

					if (transformedVertex.x < playerRight && transformedVertex.x > playerLeft && transformedVertex.y < playerBottom && transformedVertex.y > playerTop) {
						isCollision = true;
						break;
					}
				}

				if (isCollision) {
					playerMotion.velocity.x = 0.f;

                    // Extract needed entities from current level
                    Entity background_entity = currentLevel.background_entity;
                    Entity background_entity2 = currentLevel.background_entity2;
                    Entity foreground_entity = currentLevel.foreground_entity;
                    Entity foreground_entity2 = currentLevel.foreground_entity2;
                    Entity player_protagonist = currentLevel.player_protagonist;

					Motion& background_motion = registry.motions.get(background_entity);
					background_motion.velocity.x = 0;
					Motion& background_motion2 = registry.motions.get(background_entity2);
					background_motion2.velocity.x = 0;

					playerMoving = false;

					float obstacleLeft = obstacleMotion.position.x - abs(obstacleMotion.scale.x) / 2;
					float obstacleRight = obstacleMotion.position.x + abs(obstacleMotion.scale.x) / 2;
					if (playerMotion.position.x < obstacleLeft) {
						for (auto& motion : registry.motions.components) {
							motion.position.x += 1;
						}

						float oldPlayerXPosition = playerMotion.position.x;
						playerMotion.position.x = obstacleMotion.position.x - abs(obstacleMotion.scale.x) / 2 - abs(playerMotion.scale.x) / 2;
						float positionDiff = playerMotion.position.x - oldPlayerXPosition;

						// deal with background (parallaxing)
						Motion& background_motion = registry.motions.get(background_entity);
						background_motion.position.x += positionDiff / 2;
						Motion& background_motion2 = registry.motions.get(background_entity2);
						background_motion2.position.x += positionDiff / 2;
					}
					else if (playerMotion.position.x >= obstacleLeft && playerMotion.position.x < obstacleRight) {
						playerMotion.position.y -= 1;
						protagonist_center_pos_y = playerMotion.position.y;
					}
					else {
						for (auto& motion : registry.motions.components) {
							motion.position.x -= 1;
						}

						float oldPlayerXPosition = playerMotion.position.x;
						playerMotion.position.x = obstacleMotion.position.x + abs(obstacleMotion.scale.x) / 2 + abs(playerMotion.scale.x) / 2;
						float positionDiff = playerMotion.position.x - oldPlayerXPosition;

						// deal with background (parallaxing)
						Motion& background_motion = registry.motions.get(background_entity);
						background_motion.position.x += positionDiff / 2;
						Motion& background_motion2 = registry.motions.get(background_entity2);
						background_motion2.position.x += positionDiff / 2;
					}
				}
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

/*
void move_protagonist(int key, int action, Entity protagonist) {
	auto& motionRegistry = registry.motions;
	Motion& protagonistMotion = motionRegistry.get(protagonist);
	if (!registry.deathTimers.has(protagonist)) {
		if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
			// May want to replace ground position with colliding with walkable entities
			if (protagonistMotion.position.y == protagonist_center_pos_y) {
				protagonistMotion.velocity.x = 0.f;
				protagonistMotion.velocity.y = -100.f;
			}
		} else if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
			// protagonist should crouch
		} else if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
			protagonistMotion.scale.y = -1.f * abs(protagonistMotion.scale.y);
			protagonistMotion.angle = acos(-1);
			protagonistMotion.velocity.x = -100.f;
		} else if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
			protagonistMotion.scale.y = abs(protagonistMotion.scale.y);
			protagonistMotion.angle = 0.0f;
			protagonistMotion.velocity.x = 100.f;
		} else {
		   //  protagonistMotion.velocity.x = 0.0f;
		   // protagonistMotion.velocity.y = 0.0f;
		}
	}

	// std::cout << "Protagonist position: " << protagonistMotion.position.x << ", " << protagonistMotion.position.y << "\n" << std::endl;
	// std::cout << "Protagonist velocity: " << protagonistMotion.velocity.x << ", " << protagonistMotion.velocity.y << "\n" << std::endl;

}
*/


// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE CHICKEN MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// assuming 0,0 is top left of the window
	// up = y-, down = y+, left = x-, right = x+

	//std::cout << "Key: " << key << " Action: " << action << std::endl;

    // Extract needed entities from current level
    Entity background_entity = currentLevel.background_entity;
    Entity background_entity2 = currentLevel.background_entity2;
    Entity foreground_entity = currentLevel.foreground_entity;
    Entity foreground_entity2 = currentLevel.foreground_entity2;
    Entity player_protagonist = currentLevel.player_protagonist;

	auto& motionRegistry = registry.motions;
	if (!registry.deathTimers.has(player_protagonist)) {
		if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
			// protagonist should jump
			Motion& protagonist_motion = motionRegistry.get(player_protagonist);
			protagonist_motion.velocity.y = -300.f;
			Mix_PlayChannel(-1, player_jump_sound, 0);
		}
		else if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
			// protagonist should crouch
		}
		else if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_LEFT) {
			// DEAL WITH PROTAGONIST
			Motion& protagonist_motion = motionRegistry.get(player_protagonist);
			protagonist_motion.scale.x = -abs(protagonist_motion.scale.x);
			protagonist_motion.velocity.x = -200.0f; // temporary until we implement a bounding box for character movement
			// deal with background (parallaxing)
			Motion& background_motion = motionRegistry.get(background_entity);
			background_motion.velocity.x = -100;
			Motion& background_motion2 = motionRegistry.get(background_entity2);
			background_motion2.velocity.x = -100;

			playerMoving = true;
		}
		else if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_RIGHT) {
			Motion& protagonist_motion = motionRegistry.get(player_protagonist);
			protagonist_motion.scale.x = abs(protagonist_motion.scale.x);
			protagonist_motion.velocity.x = 200.0f; // temporary until we implement a bounding box for character movement
			// deal with background (parallaxing)
			Motion& background_motion = motionRegistry.get(background_entity);
			background_motion.velocity.x = 100;
			Motion& background_motion2 = motionRegistry.get(background_entity2);
			background_motion2.velocity.x = 100;

			playerMoving = true;
		}
		else if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT) {
			Motion& protagonist_motion = motionRegistry.get(player_protagonist);
			if (protagonist_motion.velocity.x < 0) {
				protagonist_motion.velocity.x = 0.0f; // temporary until we implement a bounding box for character movement

				Motion& background_motion = motionRegistry.get(background_entity);
				background_motion.velocity.x = 0;
				Motion& background_motion2 = motionRegistry.get(background_entity2);
				background_motion2.velocity.x = 0;

				playerMoving = false;
			}
		}
		else if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT) {
			Motion& protagonist_motion = motionRegistry.get(player_protagonist);
			if (protagonist_motion.velocity.x > 0) {
				protagonist_motion.velocity.x = 0.0f; // temporary until we implement a bounding box for character movement

				Motion& background_motion = motionRegistry.get(background_entity);
				background_motion.velocity.x = 0;
				Motion& background_motion2 = motionRegistry.get(background_entity2);
				background_motion2.velocity.x = 0;

				playerMoving = false;
			}
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		// close
		glfwSetWindowShouldClose(window, 1);
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart_game(LevelType::FOREST_LEVEL, true);
	}

	// Debugging
	if (action == GLFW_RELEASE && key == GLFW_KEY_D) {
		debugging.in_debug_mode = !debugging.in_debug_mode;
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		display_fps = !display_fps;
	}

	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        gameConfig.show_help = !gameConfig.show_help;
        // renderer->show_help = !renderer->show_help;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
	}
	current_speed = fmax(0.f, current_speed);
	for (auto& motion : registry.motions.components) {
		//std::cout << "Motion velocity finish x: " << motion.velocity.x << std::endl;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		if (timeSinceBulletSpawn <= 0) {
			createBullet(renderer);
			timeSinceBulletSpawn = 500;
		}
	}

}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE CHICKEN ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the chicken's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
