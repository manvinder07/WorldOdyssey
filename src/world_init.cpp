#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>

Entity createProtagonist(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.0f;
    motion.velocity = { 0.f, 0.f };
    // motion.scale = mesh.original_size * 100.f;
    // motion.scale.y = abs(motion.scale.y); // point front to the right
    motion.scale = vec2({ PROTAGONIST_BB_WIDTH, PROTAGONIST_BB_HEIGHT });

    // Create and (empty) Chicken component to be able to refer to all eagles
    registry.players.emplace(entity);

    Health& protagonistHealth = registry.healths.emplace(entity);
    protagonistHealth.health = PROTAGONIST_HEALTH;

    registry.player_sprites.push_back({ TEXTURE_ASSET_ID::PROTAGONST21,
                              EFFECT_ASSET_ID::TEXTURED,
                              GEOMETRY_BUFFER_ID::SPRITE});
    registry.player_sprites.push_back({ TEXTURE_ASSET_ID::PROTAGONST22,
                              EFFECT_ASSET_ID::TEXTURED,
                              GEOMETRY_BUFFER_ID::SPRITE});
    registry.player_sprites.push_back({ TEXTURE_ASSET_ID::PROTAGONST23,
                              EFFECT_ASSET_ID::TEXTURED,
                              GEOMETRY_BUFFER_ID::SPRITE});
    registry.player_sprites.push_back({ TEXTURE_ASSET_ID::PROTAGONST24,
                              EFFECT_ASSET_ID::TEXTURED,
                              GEOMETRY_BUFFER_ID::SPRITE});


    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::PROTAGONST24, // TEXTURE_COUNT indicates that no txture is needed
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createGrenade(RenderSystem* renderer, Entity bossE, Entity playerE)
{
    auto entity = Entity();

    Motion bossMotion = registry.motions.get(bossE);
    Motion playerMotion = registry.motions.get(playerE);

    vec2 direction = (bossMotion.scale / abs(bossMotion.scale));

    float grenadeTime = 1500; // The time it takes for the grenade to reach the player in ms
    float grenadeTimeps = grenadeTime / 1000;
    float initialVely = -100;

    float playerGrenadeDistX = abs(playerMotion.position.x - bossMotion.position.x);
    float velX = playerGrenadeDistX  / grenadeTimeps;
    float playerGrenadeDistY = abs(playerMotion.position.y - bossMotion.position.y);
    float velY = playerGrenadeDistY / grenadeTimeps;
    float deltaVy = 50.0f;

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.0f;
    if (playerMotion.position.y - bossMotion.position.y < 0) {
        motion.velocity = direction * vec2({ -velX, velY });
    } else {
        motion.velocity = direction * vec2({ -velX, initialVely });
        deltaVy = (2 * (playerGrenadeDistY - (initialVely * grenadeTimeps))) / (pow(grenadeTimeps, 2));
    }
    motion.velocity = direction * vec2({ -velX, initialVely });
    motion.position = bossMotion.position;
    motion.position.x += direction.x * -100.f;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = direction * vec2({GRENADE_BB_WIDTH, GRENADE_BB_HEIGHT});

    // Create and (empty) scorpion component to be able to refer to all scorpions
    Grenade& grenade = registry.grenades.emplace(entity);
    grenade.damage = GRENADE_DAMAGE;
    grenade.time = grenadeTime;
    grenade.spawnTime = 5000;
    grenade.deltaVy = deltaVy;
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::GRENADE,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}


Entity createSnowball(RenderSystem* renderer, Entity bossE, Entity playerE)
{
    auto entity = Entity();

    Motion bossMotion = registry.motions.get(bossE);
    Motion playerMotion = registry.motions.get(playerE);

    vec2 direction = (bossMotion.scale / abs(bossMotion.scale));

    float grenadeTime = 1500; // The time it takes for the grenade to reach the player in ms
    float grenadeTimeps = grenadeTime / 1000;
    float initialVely = -100;

    float playerGrenadeDistX = abs(playerMotion.position.x - bossMotion.position.x);
    float velX = playerGrenadeDistX  / grenadeTimeps;
    float playerGrenadeDistY = abs(playerMotion.position.y - bossMotion.position.y);
    float velY = playerGrenadeDistY / grenadeTimeps;
    float deltaVy = 50.0f;

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.0f;
    if (playerMotion.position.y - bossMotion.position.y < 0) {
        motion.velocity = direction * vec2({ -velX, velY });
    } else {
        motion.velocity = direction * vec2({ -velX, initialVely });
        deltaVy = (2 * (playerGrenadeDistY - (initialVely * grenadeTimeps))) / (pow(grenadeTimeps, 2));
    }
    motion.velocity = direction * vec2({ -velX, initialVely });
    motion.position = bossMotion.position;
    motion.position.x += direction.x * -100.f;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = direction * vec2({ SNOWBALL_BB_WIDTH, SNOWBALL_BB_HEIGHT });

    // Create and (empty) scorpion component to be able to refer to all scorpions
    Snowball& snowball = registry.snowballs.emplace(entity);
    snowball.damage = SNOWBALL_DAMAGE;
    snowball.time = grenadeTime;
    snowball.spawnTime = 5000;
    snowball.deltaVy = deltaVy;
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::SNOWBALL,
          EFFECT_ASSET_ID::TEXTURED,
          GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}


Entity createExplosion(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.0f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = vec2({ EXPLOSION_BB_WIDTH, EXPLOSION_BB_HEIGHT });

    // Create and (empty) Chicken component to be able to refer to all eagles
    Explosion explosion = registry.explosions.emplace(entity);
    explosion.timeSwitch = 500;

    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION1,
                                        EFFECT_ASSET_ID::TEXTURED,
                                        GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION2,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION3,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION4,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION5,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION6,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION7,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION8,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION9,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});
    registry.explosion_sprites.push_back({ TEXTURE_ASSET_ID::EXPLOSION10,
                                           EFFECT_ASSET_ID::TEXTURED,
                                           GEOMETRY_BUFFER_ID::SPRITE});

    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::EXPLOSION1,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createTornado(RenderSystem* renderer, Entity bossE, Entity playerE)
{
    auto entity = Entity();

    Motion bossMotion = registry.motions.get(bossE);
    Motion playerMotion = registry.motions.get(playerE);

    vec2 direction = (bossMotion.scale / abs(bossMotion.scale));

    float tornadoTime = 1500; // The time it takes for the grenade to reach the player in ms
    float tornadoTimeps = tornadoTime / 1000;

    float playerGrenadeDistX = abs(playerMotion.position.x - bossMotion.position.x);
    float velX = playerGrenadeDistX  / tornadoTimeps;
    float playerGrenadeDistY = playerMotion.position.y - bossMotion.position.y;
    float velY = playerGrenadeDistY / tornadoTimeps;

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.0f;
    motion.velocity = direction * vec2({ -velX, velY });
    motion.position = bossMotion.position;
    motion.position.x += direction.x * -100.f;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = direction * vec2({ TORNADO_BB_WIDTH, TORNADO_BB_HEIGHT });

    // Create and (empty) scorpion component to be able to refer to all scorpions
    Tornado& tornado = registry.tornados.emplace(entity);
    tornado.damage = TORNADO_DAMAGE;
    tornado.time = tornadoTime;
    tornado.timeSwitch = 250;
    tornado.spawnTime = 2000;
    tornado.spriteIndex = 0;
    tornado.timeSinceSwitch = 0;

    if (registry.tornado_sprites.empty()) {
        registry.tornado_sprites.push_back({ TEXTURE_ASSET_ID::TORNADO1,
                                             EFFECT_ASSET_ID::TEXTURED,
                                             GEOMETRY_BUFFER_ID::SPRITE});
        registry.tornado_sprites.push_back({ TEXTURE_ASSET_ID::TORNADO2,
                                             EFFECT_ASSET_ID::TEXTURED,
                                             GEOMETRY_BUFFER_ID::SPRITE});
        registry.tornado_sprites.push_back({ TEXTURE_ASSET_ID::TORNADO3,
                                             EFFECT_ASSET_ID::TEXTURED,
                                             GEOMETRY_BUFFER_ID::SPRITE});
        registry.tornado_sprites.push_back({ TEXTURE_ASSET_ID::TORNADO4,
                                             EFFECT_ASSET_ID::TEXTURED,
                                             GEOMETRY_BUFFER_ID::SPRITE});
    }

    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::TORNADO1,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createSpider(RenderSystem* renderer, vec2 position, vec2 velocity)
{
    auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

    vec2 bug_dim = vec2(578, 421) * 0.2f;

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity + vec2({ -150.f, 0});
	motion.position = position + vec2(0, bug_dim.y/4);


	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = bug_dim;//vec2({ SCORPION_BB_WIDTH, SCORPION_BB_WIDTH }) * 0.1f;

	// Create and (empty) scorpion component to be able to refer to all scorpions
	registry.enemies.emplace(entity);
    registry.spiders.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = SPIDER_HEALTH;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BUG,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });
	registry.deadlys.emplace(entity);

	return entity;

}

Entity createEagle(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 100.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -EAGLE_BB_WIDTH, EAGLE_BB_HEIGHT });

	// Create and (empty) Eagle component to be able to refer to all eagles
	registry.deadlys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EAGLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createScorpion(RenderSystem* renderer, vec2 position, vec2 velocity)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity + vec2({ -155.f, 0});
	motion.position = position;


	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ SCORPION_BB_WIDTH, SCORPION_BB_HEIGHT });

	// Create and (empty) scorpion component to be able to refer to all scorpions
	registry.enemies.emplace(entity);
    registry.scorpions.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = SCORPION_HEALTH;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SCORPION,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.deadlys.emplace(entity);

	return entity;
}

#if 0
Entity createForestBoss(RenderSystem* renderer, vec2 position, vec2 velocity)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity + vec2({ -100.f, 0 });
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ FOREST_BOSS_BB_WIDTH, FOREST_BOSS_BB_HEIGHT });

	// Create and (empty) boss component to be able to refer to all bosses
	registry.bosses.emplace(entity);
    registry.forestBosses.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = FOREST_BOSS_HEALTH;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOSS,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.deadlys.emplace(entity);


	return entity;
}

#endif

Entity createForestBoss(RenderSystem* renderer, vec2 position, vec2 velocity)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity + vec2({ -150.f, 0 });
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ FOREST_BOSS_BB_WIDTH, FOREST_BOSS_BB_HEIGHT });

	// Create and (empty) boss component to be able to refer to all bosses
	registry.bosses.emplace(entity);
    registry.forestBosses.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = FOREST_BOSS_HEALTH;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SPIDER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.deadlys.emplace(entity);


	return entity;
}

Entity createDesertBoss(RenderSystem* renderer, vec2 position, vec2 velocity)
{
    auto entity = Entity();

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = velocity + vec2({ -150.f, 0 });
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ DESERT_BOSS_BB_WIDTH, DESERT_BOSS_BB_HEIGHT });

    // Create and (empty) boss component to be able to refer to all bosses
    DesertBoss& boss = registry.desertBosses.emplace(entity);
    boss.timeSwitch = 1000;
    registry.bosses.emplace(entity);
    registry.deadlys.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = DESERT_BOSS_HEALTH;

    registry.dragon_sprites.push_back({ TEXTURE_ASSET_ID::DRAGON1,
                                        EFFECT_ASSET_ID::TEXTURED,
                                        GEOMETRY_BUFFER_ID::SPRITE});
    registry.dragon_sprites.push_back({ TEXTURE_ASSET_ID::DRAGON2,
                                        EFFECT_ASSET_ID::TEXTURED,
                                        GEOMETRY_BUFFER_ID::SPRITE});
    registry.dragon_sprites.push_back({ TEXTURE_ASSET_ID::DRAGON3,
                                        EFFECT_ASSET_ID::TEXTURED,
                                        GEOMETRY_BUFFER_ID::SPRITE});

    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::DRAGON1,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });


    return entity;
}

Entity createIceBoss(RenderSystem* renderer, vec2 position, vec2 velocity)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = velocity + vec2({ -150.f, 0 });
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ ICE_BOSS_BB_WIDTH, ICE_BOSS_BB_HEIGHT });

    // Create and (empty) boss component to be able to refer to all bosses
    registry.bosses.emplace(entity);
    registry.iceBosses.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = ICE_BOSS_HEALTH;

    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::ICE_BOSS,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });
    registry.deadlys.emplace(entity);


    return entity;
}

Entity createIceMonster1(RenderSystem* renderer, vec2 position, vec2 velocity)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = velocity + vec2({ -150.f, 0 });
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ ICE1_BB_WIDTH, ICE1_BB_HEIGHT });

    // Create and (empty) boss component to be able to refer to all bosses
    registry.enemies.emplace(entity);
    registry.ice1Monsters.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = ICE1_HEALTH;

    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::ICE_MONSTER_1,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });
    registry.deadlys.emplace(entity);


    return entity;
}

Entity createIceMonster2(RenderSystem* renderer, vec2 position, vec2 velocity)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = velocity + vec2({ -150.f, 0 });
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ ICE2_BB_WIDTH, ICE2_BB_HEIGHT });

    // Create and (empty) boss component to be able to refer to all bosses
    registry.enemies.emplace(entity);
    registry.ice2Monsters.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = ICE2_HEALTH;

    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::ICE_MONSTER_2,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });
    registry.deadlys.emplace(entity);


    return entity;
}

Entity createSnake(RenderSystem* renderer, vec2 position, vec2 velocity)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity + vec2({ -155.f, 0 });
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ SNAKE_BB_WIDTH, SNAKE_BB_HEIGHT });

	// Create and (empty) scorpion component to be able to refer to all scorpions
	registry.enemies.emplace(entity);
    registry.snakes.emplace(entity);
    auto& health = registry.healths.emplace(entity);
    health.health = SNAKE_HEALTH;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SNAKE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.deadlys.emplace(entity);

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::EGG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createRock(RenderSystem* renderer, vec2 pos, GEOMETRY_BUFFER_ID rock, float scaleMultiplier, vec2 velocity)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(rock);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity;
	motion.scale = mesh.original_size * scaleMultiplier;
	motion.scale.y *= -1;
	pos.x += abs(motion.scale.x) / 2;
	pos.y -= abs(motion.scale.y) / 2;
	motion.position = pos;

	registry.obstacles.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::EGG,
			rock });

	return entity;
}

Entity createEgg(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.deadlys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::EGG,
			GEOMETRY_BUFFER_ID::EGG });

	return entity;
}

Entity createBackground(RenderSystem* renderer, vec2 position,  TEXTURE_ASSET_ID background)
{
	auto entity = Entity();

	// Assuming the background uses a sprite mesh that covers the entire screen.
	// The mesh could be a simple quad that's scaled to the size of the screen.
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values, the position would be the center of the screen.
	// Assuming motion system is used for positioning entities.
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.0f; // No rotation.
	motion.velocity = { 0.f, 0.f }; // Static, no movement.
	motion.scale = { window_width_px, window_height_px }; // Scale to cover the full screen.

	// Assign the background texture to this entity.
	registry.renderRequests.insert(
		entity,
		{ background,
          EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createForeground(RenderSystem* renderer, vec2 position, vec2 size, TEXTURE_ASSET_ID foreground) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.0f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	registry.renderRequests.insert(
		entity,
		{ foreground, 
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createForeground2(RenderSystem* renderer, vec2 position, vec2 size) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.0f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FOREGROUND2, 
		  EFFECT_ASSET_ID::TEXTURED, 
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createHelpText(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Assuming the background uses a sprite mesh that covers the entire screen.
	// The mesh could be a simple quad that's scaled to the size of the screen.
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values, the position would be the center of the screen.
	// Assuming motion system is used for positioning entities.
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.0f; // No rotation.
	motion.velocity = { 0.f, 0.f }; // Static, no movement.
	motion.scale = { window_width_px *5/11, window_height_px*7/10 }; // Scale to cover the full screen.

	// Assign the background texture to this entity.
	registry.helpTexts.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELPTEXT,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createGun(RenderSystem* renderer)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Entity protagonist = registry.players.entities[0];
    Motion& protagonistMotion = registry.motions.get(protagonist);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.0f;
    motion.velocity = protagonistMotion.velocity;
    motion.position = protagonistMotion.position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({GUN_BB_WIDTH, GUN_BB_HEIGHT});

    // Create and (empty) scorpion component to be able to refer to all scorpions
    Gun& gun = registry.guns.emplace(entity);
    gun.damage = BULLET_DAMAGE;
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::GUN,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createBullet(RenderSystem* renderer)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Entity gun = registry.guns.entities[0];
    Motion gunMotion = registry.motions.get(gun);

    vec2 direction = (gunMotion.scale / abs(gunMotion.scale));

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.0f;
    motion.velocity = direction * vec2({ 400.f, 0 });
    motion.position = gunMotion.position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = direction * vec2({BULLET_BB_WIDTH, BULLET_BB_HEIGHT});

    // Create and (empty) scorpion component to be able to refer to all scorpions
    registry.bullets.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::BULLET,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

// spawns a Sword
Entity createSword(RenderSystem* renderer, Entity parent, vec3 color) {
    Entity entity = Entity();
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Get the current parent motion
    Motion parentMotion = registry.motions.get(parent);

    // Initialize motion for sword
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0;
    motion.velocity = vec2(0, 0); // TODO calculate random velocity for the object
    motion.position = parentMotion.position + vec2(24, 0); // offset the sword 10 pixels
    motion.scale = vec2(48, 48);

    registry.colors.emplace(entity, color);

    // Save the sword in the items component
    Item& item = registry.items.emplace(entity);
    item.type = Item::TYPE_ID::SWORD;

    // Create a render request for the sword
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::SWORD,
          EFFECT_ASSET_ID::TEXTURED,
          GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

// spawns a Shield
Entity createShield(RenderSystem* renderer, Entity parent, vec3 color) {
    Entity entity = Entity();
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Get the current parent motion
    Motion parentMotion = registry.motions.get(parent);

    // Initialize motion for sword
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0;
    motion.velocity = vec2(0, 0); // TODO calculate random velocity for the object
    motion.position = parentMotion.position - vec2(24, 0); // offset the sword 10 pixels
    motion.scale = vec2(48, 48);

    registry.colors.emplace(entity, color);

    // Save the sword in the items component
    Item& item = registry.items.emplace(entity);
    item.type = Item::TYPE_ID::SHIELD;

    // Create a render request for the sword
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::SHIELD,
          EFFECT_ASSET_ID::TEXTURED,
          GEOMETRY_BUFFER_ID::SPRITE });

    return entity;

    return entity;
}

// spawns a Heart
Entity createHeart(RenderSystem* renderer, Entity parent, vec3 color) {
    Entity entity = Entity();
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Get the current parent motion
    Motion parentMotion = registry.motions.get(parent);

    // Initialize motion for sword
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0;
    motion.velocity = vec2(0, 0); // TODO calculate random velocity for the object
    motion.position = parentMotion.position - vec2(24, 0); // offset the sword 10 pixels
    motion.scale = vec2(48, 48);

    registry.colors.emplace(entity, color);

    // Save the sword in the items component
    Item& item = registry.items.emplace(entity);
    item.type = Item::TYPE_ID::HEART;

    // Create a render request for the sword
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::HEART,
          EFFECT_ASSET_ID::TEXTURED,
          GEOMETRY_BUFFER_ID::SPRITE });

    return entity;

  //  return entity;
}

void createInventoryTexture(RenderSystem* renderer, TEXTURE_ASSET_ID id, vec2 position, vec2 scale, vec3 color) {
    ImmediateTexture texture = {};
    texture.id = id;
    texture.position = position;
    texture.scale = scale;
    texture.color = color;
    renderer->immediate_textures.push_back(texture);
}

Entity createHelpText(const std::string& s, vec2 pos, vec2 velocity)
{
	Entity entity = Entity();

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity;
	motion.position = pos;
	//motion.scale = { 1.f, 1.f };

	HelpText& text = registry.helpTexts.emplace(entity);
	text.s = s;
	return entity;
}
