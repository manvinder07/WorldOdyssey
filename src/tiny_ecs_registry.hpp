#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Eatable> eatables;
	ComponentContainer<Deadly> deadlys;
    ComponentContainer<Scorpion> scorpions;
    ComponentContainer<Snake> snakes;
    ComponentContainer<Spider> spiders;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Boss> bosses;
    ComponentContainer<ForestBoss> forestBosses;
    ComponentContainer<DesertBoss> desertBosses;
	ComponentContainer<IceBoss> iceBosses;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<HelpText> helpTexts;
	ComponentContainer<vec3> colors;
	ComponentContainer<LightUp> lightUps;
    ComponentContainer<Bullet> bullets;
    ComponentContainer<Grenade> grenades;
	ComponentContainer<Snowball> snowballs;
    ComponentContainer<Explosion> explosions;
    ComponentContainer<Tornado> tornados;
    ComponentContainer<Gun> guns;
    ComponentContainer<Sword> swords;
    ComponentContainer<Health> healths;
	ComponentContainer<Obstacle> obstacles;
    ComponentContainer<Item> items;
    ComponentContainer<DamageTimer> damageTimers;
	ComponentContainer<IceMonster1> ice1Monsters;
	ComponentContainer<IceMonster2> ice2Monsters;



    std::vector<RenderRequest> player_sprites;
    std::vector<RenderRequest> explosion_sprites;
    std::vector<RenderRequest> dragon_sprites;
    std::vector<RenderRequest> tornado_sprites;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&lightUps);
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&eatables);
		registry_list.push_back(&deadlys);
        registry_list.push_back(&scorpions);
        registry_list.push_back(&snakes);
        registry_list.push_back(&spiders);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
        registry_list.push_back(&bullets);
        registry_list.push_back(&grenades);
        registry_list.push_back(&explosions);
        registry_list.push_back(&tornados);
        registry_list.push_back(&guns);
        registry_list.push_back(&swords);
        registry_list.push_back(&enemies);
        registry_list.push_back(&bosses);
        registry_list.push_back(&forestBosses);
        registry_list.push_back(&desertBosses);
		registry_list.push_back(&ice1Monsters);
		registry_list.push_back(&ice2Monsters);
		registry_list.push_back(&iceBosses);
		registry_list.push_back(&snowballs);

        registry_list.push_back(&healths);
		registry_list.push_back(&obstacles);
        registry_list.push_back(&items);
        registry_list.push_back(&damageTimers);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;