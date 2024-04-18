#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{

};

// Health component
struct Health
{
    int health;
    unsigned int armor_level = 1;
};

struct Bullet
{

};

struct Grenade
{
    float time; // in ms
    float initialVely;
    float deltaVy;
    float damage;
    float spawnTime; // in ms
};

struct Snowball
{
    float time; // in ms
    float initialVely;
    float deltaVy;
    float damage;
    float spawnTime; // in ms
};

struct Explosion
{
    float timeSwitch;
};

struct Tornado
{
    float timeSwitch;
    float damage;
    float time;
    float spawnTime;
    int spriteIndex;
    float timeSinceSwitch;
};

struct Gun
{
    float damage;
};

struct Sword
{

};

// Eagles have a hard shell
struct Deadly
{
	bool followingPlayer = false;
};

// Eagles have a hard shell
struct Scorpion
{

};

// Eagles have a hard shell
struct Snake
{

};

struct Spider {
};

struct IceMonster1 {

};

struct IceMonster2 {

};

// Scorpions are enemies
struct Enemy
{

};

// final bosses
struct Boss
{

};

struct DesertBoss
{
    float timeSwitch;
};

struct ForestBoss
{

};

struct IceBoss
{

};

// Bug and Chicken have a soft shell
struct Eatable
{

};

// Rocks, walls, etc.
struct Obstacle
{

};

// Light Up
struct LightUp
{
	float counter_ms = 1000;
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

struct GameConfig
{
    bool show_help = false;
    bool did_user_fail = false;
    float fail_screen_pause_time = 0.0f;
    float success_screen_pause_time = 0.0f;
    bool did_user_succeed = false;
};
extern GameConfig gameConfig;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A struct to refer to help graphics in the ECS
struct HelpText
{
	std::string s;
};

// A struct to refer to items in the ECS
struct Item {

    enum class TYPE_ID {
        SWORD,
        SHIELD,
        HEART
    };

    TYPE_ID type;
};

// A timer that will be associated to dying chicken
struct DeathTimer
{
	float counter_ms = 3000;
};

#define DAMAGE_TIMER_TIME 4000.0f
// A timer that will say if the player can be damage
struct DamageTimer {
    float counter_ms = DAMAGE_TIMER_TIME;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	BUG = 0,
	EAGLE = BUG + 1,
    PROTAGONIST = EAGLE + 1,
    SCORPION = PROTAGONIST + 1,
	SNAKE = SCORPION + 1,
	BOSS = SNAKE + 1,
	BACKGROUND = BOSS + 1,
    BACKGROUND2 = BACKGROUND + 1,
	FOREGROUND = BACKGROUND2 + 1,
	FOREGROUND2 = FOREGROUND + 1,
    FOREST_FOREGROUND = FOREGROUND2 + 1,
    GUN = FOREST_FOREGROUND + 1,
    BULLET = GUN + 1,
    PROTAGONST21 = BULLET + 1,
    PROTAGONST22 = PROTAGONST21 + 1,
    PROTAGONST23 = PROTAGONST22 + 1,
    PROTAGONST24 = PROTAGONST23 + 1,
    HELPTEXT = PROTAGONST24 + 1,
    SWORD = HELPTEXT + 1,
    SHIELD = SWORD + 1,
    WHITE = SHIELD + 1,
    HEART = WHITE + 1,
    GRENADE = HEART + 1,
    EXPLOSION1 = GRENADE + 1,
    EXPLOSION2 = EXPLOSION1 + 1,
    EXPLOSION3 = EXPLOSION2 + 1,
    EXPLOSION4 = EXPLOSION3 + 1,
    EXPLOSION5 = EXPLOSION4 + 1,
    EXPLOSION6 = EXPLOSION5 + 1,
    EXPLOSION7 = EXPLOSION6 + 1,
    EXPLOSION8 = EXPLOSION7 + 1,
    EXPLOSION9 = EXPLOSION8 + 1,
    EXPLOSION10 = EXPLOSION9 + 1,
    DRAGON1 = EXPLOSION10 + 1,
    DRAGON2 = DRAGON1 + 1,
    DRAGON3 = DRAGON2 + 1,
    TORNADO1 = DRAGON3 + 1,
    TORNADO2 = TORNADO1 + 1,
    TORNADO3 = TORNADO2 + 1,
    TORNADO4 = TORNADO3 + 1,
    HEART_L = TORNADO4 + 1,
    SPIDER = HEART_L + 1,
    ICE_BACKGROUND = SPIDER + 1,
    ICE_FOREGROUND = ICE_BACKGROUND + 1,
    ICE_BOSS = ICE_FOREGROUND + 1,
    ICE_MONSTER_1 = ICE_BOSS + 1,
    ICE_MONSTER_2 = ICE_MONSTER_1 + 1,
    SNOWBALL = ICE_MONSTER_2 + 1,
	TEXTURE_COUNT = SNOWBALL + 1,
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	EGG = COLOURED + 1,
    PROTAGONIST = EGG + 1,
	TEXTURED = PROTAGONIST + 1,
	WIND = TEXTURED + 1,
	EFFECT_COUNT = WIND + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	PROTAGONIST = 0,
	SPRITE = PROTAGONIST + 1,
	EGG = SPRITE + 1,
	DEBUG_LINE = EGG + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	ROCK0 = SCREEN_TRIANGLE + 1,
	ROCK1 = ROCK0 + 1,
	ROCK2 = ROCK1 + 1,
	ROCK3 = ROCK2 + 1,
	ROCK4 = ROCK3 + 1,
	GEOMETRY_COUNT = ROCK4 + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

