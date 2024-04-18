// internal
#include "physics_system.hpp"
#include "world_init.hpp"

#define M_PI 3.14159265358979323846f

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// AABB: Intersection Tests
bool collides(const Motion& motion1, const Motion& motion2)
{
	const vec2 motion1_bounding_box = get_bounding_box(motion1) / 2.f;
	const vec2 motion2_bounding_box = get_bounding_box(motion2) / 2.f;

	float motion1Left = motion1.position.x - motion1_bounding_box.x;
	float motion1Right = motion1.position.x + motion1_bounding_box.x;
	float motion1Top = motion1.position.y - motion1_bounding_box.y;
	float motion1Bottom = motion1.position.y + motion1_bounding_box.y;

	float motion2Left = motion2.position.x - motion2_bounding_box.x;
	float motion2Right = motion2.position.x + motion2_bounding_box.x;
	float motion2Top = motion2.position.y - motion2_bounding_box.y;
	float motion2Bottom = motion2.position.y + motion2_bounding_box.y;

	return motion1Top < motion2Bottom && motion2Top < motion1Bottom && motion1Left < motion2Right && motion2Left < motion1Right;
}

void PhysicsSystem::step(float elapsed_ms)
{

    // Change the velocity_y and angle of grenade
    for (Entity grenadeE : registry.grenades.entities) {
        Grenade grenade = registry.grenades.get(grenadeE);
        float vely = grenade.deltaVy * (elapsed_ms / 1000);
        Motion& grenadeMotion = registry.motions.get(grenadeE);
        grenadeMotion.velocity.y += vely;
        grenadeMotion.angle += -30 * (elapsed_ms / 1000) * (2 * M_PI / 180);
    }

	for (Entity snowballE : registry.snowballs.entities) {
		Snowball snowball = registry.snowballs.get(snowballE);
		float vely = snowball.deltaVy * (elapsed_ms / 1000);
		Motion& snowballMotion = registry.motions.get(snowballE);
		snowballMotion.velocity.y += vely;
		snowballMotion.angle += -30 * (elapsed_ms / 1000) * (2 * M_PI / 180);
	}


	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		if (!registry.deathTimers.has(entity)) {
			float step_seconds = elapsed_ms / 1000.f;

			motion.position += motion.velocity * (step_seconds);
		}
	}

    // Animate Item y position
    static unsigned int item_animation_timer = 0;
    item_animation_timer++;
    float offset = 0;
    for(Entity item : registry.items.entities) {
        Motion& motion = registry.motions.get(item);
        motion.position.y = motion.position.y + sinf((offset + item_animation_timer)*0.2f) * 0.7;
        offset += M_PI;
    }

    for (Entity gun : registry.guns.entities) {
        Motion& gunMotion = registry.motions.get(gun);
        Entity protagonist = registry.players.entities[0];
        Motion playerMotion = registry.motions.get(protagonist);
        gunMotion.position = playerMotion.position;
        gunMotion.scale = (playerMotion.scale / abs(playerMotion.scale)) * abs(gunMotion.scale);
    }

    for (Entity bullet : registry.bullets.entities) {
        Motion& bulletMotion = registry.motions.get(bullet);
        bulletMotion.velocity = (bulletMotion.scale / abs(bulletMotion.scale)) * vec2({400.f, 0.0f});
    }

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j<motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Keep protagonist within the boundaries of the window
    for (Entity player : registry.players.entities) {
        if (registry.motions.has(player)) {
            Motion& playerMotion = registry.motions.get(player);
            // Check for player collision with window boundary
            vec2 playerBB = get_bounding_box(playerMotion);
			/*
            if (playerMotion.position.x < (playerBB.x / 2.f)) {
                playerMotion.velocity = {0.0f, 0.0f};
                playerMotion.position.x = (playerBB.x / 2.f);
            }
            if (playerMotion.position.x > (window_width_px - (playerBB.x / 2.f))) {
                playerMotion.velocity = {0.0f, 0.0f};
                playerMotion.position.x = (window_width_px - (playerBB.x / 2.f));
            }
			*/
            if (playerMotion.position.y < (playerBB.y / 2.f)) {
				playerMotion.velocity.y = 0.0f;
                playerMotion.position.y = (playerBB.y / 2.f);
            }
            if (playerMotion.position.y > (window_height_px - (playerBB.y / 2.f))) {
				playerMotion.velocity.y = 0.0f;
				playerMotion.position.y = (window_height_px - (playerBB.y / 2.f));
            }

			//apply gravity
			if (playerMotion.position.y < protagonist_center_pos_y) {
				playerMotion.velocity.y += 12.0f;
			} else {
				playerMotion.velocity.y = 0.f;
			}
			if (playerMotion.position.y > protagonist_center_pos_y) {
				playerMotion.position.y = protagonist_center_pos_y;
			}
        }
    }
}