#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float PLATFORM_HEIGHT = 150.f;
const float PLATFORM_CENTER = window_height_px - PLATFORM_HEIGHT / 2;
const float GROUND_POSITION = window_height_px - PLATFORM_HEIGHT;


const float BUG_BB_WIDTH = 0.6f * 165.f;
const float BUG_BB_HEIGHT = 0.6f * 165.f;
const float EAGLE_BB_WIDTH = 0.6f * 300.f;
const float EAGLE_BB_HEIGHT = 0.6f * 202.f;
const float SCORPION_BB_WIDTH = 0.6f * 300.f;
const float SCORPION_BB_HEIGHT = 0.6f * 202.f;
const float SNAKE_BB_WIDTH = 0.6f * 300.f;
const float SNAKE_BB_HEIGHT = 0.6f * 202.f;
const float FOREST_BOSS_BB_WIDTH = 0.6f * 850.f;
const float FOREST_BOSS_BB_HEIGHT = 0.6f * 702.f;
const float DESERT_BOSS_BB_WIDTH = 0.6f * 850.f;
const float DESERT_BOSS_BB_HEIGHT = 0.6f * 850.f;
const float ICE_BOSS_BB_WIDTH = 0.6f * 850.f;
const float ICE_BOSS_BB_HEIGHT = 0.6f * 650.f;
const float ICE1_BB_HEIGHT = 0.6f * 275.f;
const float ICE1_BB_WIDTH = 0.6f * 320.f;
const float ICE2_BB_HEIGHT = 0.6f * 275.f;
const float ICE2_BB_WIDTH = 0.6f * 360.f;

const float PROTAGONIST_BB_WIDTH = 0.6f * 65.f; // approximate
const float PROTAGONIST_BB_HEIGHT = 0.6f * 170.f; // approximate
extern float protagonist_center_pos_y;
const float GUN_BB_WIDTH = 0.6f * 100.f;
const float GUN_BB_HEIGHT = 0.6f * 75.f;
const float BULLET_BB_WIDTH = 0.6f * 50.f;
const float BULLET_BB_HEIGHT = 0.6f * 25.f;
const float GRENADE_BB_WIDTH = 0.6f * 50.f;
const float GRENADE_BB_HEIGHT = 0.6f * 50.f;
const float SNOWBALL_BB_WIDTH = 0.6f * 75.f;
const float SNOWBALL_BB_HEIGHT = 0.6f * 75.f;

const float EXPLOSION_BB_WIDTH = 0.6f * 100.f;
const float EXPLOSION_BB_HEIGHT = 0.6f * 75.f;
const float TORNADO_BB_WIDTH = 0.6f * 120.f;
const float TORNADO_BB_HEIGHT = 0.6f * 100.f;

const float PROTAGONIST_HEALTH = 3.f;
const float MAX_PROTAGONIST_HEALTH = PROTAGONIST_HEALTH * 3;
const float SCORPION_HEALTH = 6.f;
const float SPIDER_HEALTH = 3.0f;
const float SNAKE_HEALTH = 8.f;
const float ICE1_HEALTH = 9.f;
const float ICE2_HEALTH = 10.f;
const float ICE_BOSS_HEALTH = 60.f;
const float FOREST_BOSS_HEALTH = 40.f; // prev was 20
const float DESERT_BOSS_HEALTH = 50.f; // prev was 20
const float BULLET_DAMAGE = 1.0f;
const float GRENADE_DAMAGE = 2.0f;
const float TORNADO_DAMAGE = 4.0f;
const float SNOWBALL_DAMAGE = 6.0f;

const unsigned int SCORPION_DAMAGE = 2;
const unsigned int BUG_DAMAGE = 4;
const unsigned int ICE1_DAMAGE = 5;
const float ICE2_DAMAGE = 6.0f;
const unsigned int FOREST_BOSS_DAMAGE = 5;
const unsigned int DESERT_BOSS_DAMAGE = 10;
const unsigned int ICE_BOSS_DAMAGE = 15;

// background
Entity createBackground(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID background);
// foreground
Entity createForeground(RenderSystem* renderer, vec2 position, vec2 size, TEXTURE_ASSET_ID foreground);
Entity createForeground2(RenderSystem* renderer, vec2 position, vec2 size);
Entity createHelpText(RenderSystem* renderer, vec2 position);
// the player
Entity createProtagonist(RenderSystem* renderer, vec2 pos);
// the prey
Entity createSpider(RenderSystem* renderer, vec2 position, vec2 velocity);
// the enemy
Entity createEagle(RenderSystem* renderer, vec2 position);
// scorpion
Entity createScorpion(RenderSystem* renderer, vec2 position, vec2 velocity);
Entity createIceMonster1(RenderSystem* renderer, vec2 position, vec2 velocity);
Entity createIceMonster2(RenderSystem* renderer, vec2 position, vec2 velocity);

//snake
Entity createSnake(RenderSystem* renderer, vec2 position, vec2 velocity);
// boss
Entity createForestBoss(RenderSystem* renderer, vec2 position, vec2 velocity);
Entity createDesertBoss(RenderSystem* renderer, vec2 position, vec2 velocity);
Entity createIceBoss(RenderSystem* renderer, vec2 position, vec2 velocity);

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a rock
Entity createRock(RenderSystem* renderer, vec2 pos, GEOMETRY_BUFFER_ID rock, float scaleMultiplier, vec2 velocity);
// a egg
Entity createEgg(vec2 pos, vec2 size);
Entity createGun(RenderSystem* renderer);
Entity createBullet(RenderSystem* renderer);
// spawns a Sword
Entity createSword(RenderSystem* renderer, Entity parent, vec3 color);
// spawns a Shield
Entity createShield(RenderSystem* renderer, Entity parent, vec3 color);
// spawns a Heart
Entity createHeart(RenderSystem* renderer, Entity parent, vec3 color);
// a texture to render in the inventory
void createInventoryTexture(RenderSystem* renderer, TEXTURE_ASSET_ID id, vec2 position, vec2 scale, vec3 color = vec3(1));
// a text
Entity createHelpText(const std::string& s, vec2 pos, vec2 velocity);

Entity createGrenade(RenderSystem* renderer, Entity boss, Entity player);

Entity createSnowball(RenderSystem* renderer, Entity boss, Entity player);

Entity createExplosion(RenderSystem* renderer, vec2 pos);

Entity createTornado(RenderSystem* renderer, Entity bossE, Entity playerE);
