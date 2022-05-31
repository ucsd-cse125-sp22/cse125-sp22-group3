#include "GameManager.h"

#include "Network/NetworkPacket.h"

std::chrono::steady_clock::time_point GameManager::curr_time_ = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point GameManager::last_time_ = std::chrono::steady_clock::now();

std::vector<GameEntity*> GameManager::game_entities = {};
PhysicsEngine GameManager::physics = PhysicsEngine();
std::vector<PhysicsObject*> GameManager::physics_objects = {};
std::vector<uintptr_t> GameManager::entities_pending_client_delete = {};

GameManager::GameManager()
{
	physics = PhysicsEngine();
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	generator = new std::default_random_engine(seed);
}

GameManager::GameManager(std::vector<Player*> players)
	: GameManager()
{
	// Initialize Players
	players_ = players;
	int i = 0;
	for (Player* player : players_) {
		// Set Player Positions
		//player->SetWorldPosition({i * 10,0,0});
		// Add Players to Entities list
		GameManager::game_entities.push_back(player);
		//i++;
	}

	// Instantiate Physics Engine
	//std::vector<PhysicsObject*> physics_objects;
	for (Player* player : players_) {
		physics_objects.push_back(player);
	}
	physics = PhysicsEngine(physics_objects);

}

inline void GameManager::WinningFixedUpdate()
{
	if (!podiumSpawned) {
		printf("moving players to winning area\n");
		// Checks which player has the most money and break ties somehow
		std::vector<std::pair<float, Player*>> tieBreaker;
		for (int i = 0; i < players_.size(); i++) {
			tieBreaker.push_back(std::make_pair(players_[i]->curr_balance, players_[i]));
		}
		sort(tieBreaker.begin(), tieBreaker.end());
		if (players_.size() > 0) {
			tieBreaker[tieBreaker.size()-1].second->playerHeight = goldPosition.y;
			tieBreaker[tieBreaker.size() - 1].second->SetWorldPosition(goldPosition);
			tieBreaker[tieBreaker.size() - 1].second->SetRotation(glm::vec3(0, glm::pi<float>(), 0));
			tieBreaker[tieBreaker.size() - 1].second->modelAnim = DANCE;
		}
		if (players_.size() > 1) {
			tieBreaker[tieBreaker.size() - 2].second->playerHeight = silverPosition.y;
			tieBreaker[tieBreaker.size() - 2].second->SetWorldPosition(silverPosition);
			tieBreaker[tieBreaker.size() - 2].second->SetRotation(glm::vec3(0, glm::pi<float>() , 0));
			tieBreaker[tieBreaker.size() - 2].second->modelAnim = DANCE;
		}
		if (players_.size() > 2) {
			tieBreaker[tieBreaker.size() - 3].second->playerHeight = bronzePosition.y;
			tieBreaker[tieBreaker.size() - 3].second->SetWorldPosition(bronzePosition);
			tieBreaker[tieBreaker.size() - 3].second->SetRotation(glm::vec3(0, glm::pi<float>() , 0));
			tieBreaker[tieBreaker.size() - 3].second->modelAnim = DANCE;
		}
		if (players_.size() > 3) {
			tieBreaker[tieBreaker.size() - 4].second->playerHeight = loserPosition.y;
			tieBreaker[tieBreaker.size() - 4].second->SetWorldPosition(loserPosition);
			tieBreaker[tieBreaker.size() - 4].second->SetRotation(glm::vec3(0, glm::pi<float>(), 0));
			tieBreaker[tieBreaker.size() - 4].second->modelAnim = DANCE;
		}

		WinningDecoration* podium = new WinningDecoration(DecorationType::PODIUM, podiumPosition, generator);
		AddEntities({ podium });

		for (int _ = 0; _ < 50; _++) {
			WinningDecoration* balon = new WinningDecoration(DecorationType::BALLOON, podiumPosition, generator);
			AddEntities({ balon });
		}

		podiumSpawned = true;
	}

	for (GameEntity* entity : GameManager::game_entities) {
		if (auto winning_entity = dynamic_cast<WinningDecoration*>(entity)) {
			winning_entity->FixedUpdate();
		}
	}
}

inline void GameManager::FirefliesFixedUpdate(){
	if (numTimesMakingFireflies>0) {
		for (int i = 0; i < numFireFlies; i++) {
			//std::uniform_int_distribution<int> color_dist{ ModelEnum::BALLOON_YELLOW, ModelEnum::BALLOON_GREEN };
			std::uniform_real_distribution<float> velocity_dist{ 0.05f, 0.5f };
			std::uniform_real_distribution<float> pos_x_dist{ -135.f,135.f };
			std::uniform_real_distribution<float> pos_y_dist{ -1.f,10.f };
			std::uniform_real_distribution<float> pos_z_dist{ -135.f,135.f };
			std::uniform_real_distribution<float> x_rotation{ 0.f,360.f };
			std::uniform_real_distribution<float> y_rotation{ 0.f,360.f };
			std::uniform_real_distribution<float> z_rotation{ 0.f,360.f };
			std::uniform_int_distribution<int> particle_num{ 0,1 };

			//this->model = static_cast<ModelEnum>(color_dist(*generator));
			//this->upVelocity = velocity_dist(*generator);


			float x = pos_x_dist(*generator);
			float y = pos_y_dist(*generator);
			float z = pos_z_dist(*generator);
			float xRot = x_rotation(*generator);
			float yRot = y_rotation(*generator);
			float zRot = z_rotation(*generator);
			int particleNum = particle_num(*generator);

			//You can write this in a better way but its 3am and I dont feel like doing math
			// not gonna lie, i dont really like the white, pink, red and orange fireflies :( if you're reading this, uncomment the code to try it out
			ModelEnum tempEnum = PARTICLE_FIREFLIES;
			/**
			if (x < 0 && z > 0) tempEnum = particleNum?PARTICLE_FIREFLIES_ORANGE:PARTICLE_FIREFLIES_PINK; 
			else if(x<0 && z<0) tempEnum = particleNum ? PARTICLE_FIREFLIES_BLUE : PARTICLE_FIREFLIES_WHITE;
			else if (x > 0 && z<0) tempEnum = particleNum ? PARTICLE_FIREFLIES_GREEN : PARTICLE_FIREFLIES;
			else tempEnum = particleNum ? PARTICLE_FIREFLIES_RED : PARTICLE_FIREFLIES_PURPLE;
			*/

			if (x < 0 && z > 0) tempEnum = PARTICLE_FIREFLIES;
			else if (x < 0 && z < 0) tempEnum = PARTICLE_FIREFLIES_BLUE;
			else if (x > 0 && z < 0) tempEnum = PARTICLE_FIREFLIES_GREEN;
			else tempEnum = tempEnum = PARTICLE_FIREFLIES_PURPLE;

			// TODO: ask cynthia if we can check if particle is on last frame so we can move it to a random spot
			Particle* tempParticle = new Particle(tempEnum);
			tempParticle->SetPosition(glm::vec3(x,y,z));
			glm::mat4 randomRotation = glm::rotate(xRot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(yRot, glm::vec3(0.f, 1.f, 0.0f))* glm::rotate(zRot, glm::vec3(0.0f, 1.f, 0.0f));
			tempParticle->SetRotation(randomRotation);
			tempParticle->modelAnim = PARTICLE_PLAY;
			AddEntities({ tempParticle });
			fireflies.push_back(tempParticle);
			
		}
		numTimesMakingFireflies--;
	}
	else {

		firefliesSpawned = true;
	}

}

void GameManager::FixedUpdate()
{
	GameManager::UpdateFixedDeltaTime();

	bool victory_condition_met = GameManager::GetRemainingSeconds() <= 0;
	if (victory_condition_met)
	{
		WinningFixedUpdate();
	}
	else {
		if (!eggplantSpawned && GameManager::GetRemainingSeconds() <= timeToSpawnEggplant) {
			AddEntities({ new Vegetable(VegetableType::GOLDEN_EGGPLANT, VEG_GOLDEN_EGGPLANT) });
			eggplantSpawned = true;
		}
		// TODO: change 840 to timeToSpawnFireflies
		if (!firefliesSpawned && GameManager::GetRemainingSeconds() <= timeToSpawnFireflies && GameManager::GetRemainingSeconds() > timeToDespawnFireflies) {
			FirefliesFixedUpdate();
			
		}
		else if (firefliesSpawned && GameManager::GetRemainingSeconds() <= timeToDespawnFireflies) {
			for (Particle* p : fireflies) {
				RemoveEntities({ p });
			}
			firefliesSpawned = false;
		}

		for (GameEntity* entity : GameManager::game_entities) {
			entity->FixedUpdate();
		}
	}

	// printf("size: %d\n", physics.moving_collidables_.size());

	// Check collisions
	GameManager::physics.Compute();
}

void GameManager::AddEntities(std::vector<GameEntity*> entities)
{
	for (GameEntity* entity : entities) {
		GameManager::game_entities.push_back(entity);
		if (auto phys_obj = dynamic_cast<PhysicsObject*>(entity)) {
			GameManager::physics.AddPhysObject(phys_obj);
		}
	}
}

void GameManager::RemoveEntities(std::vector<GameEntity*> entities) {
	for (GameEntity* entity : entities) {
		if (entity != nullptr) {
			auto iter = std::find(GameManager::game_entities.begin(), GameManager::game_entities.end(), entity);
			if (iter != GameManager::game_entities.end() && *iter != nullptr) {
				GameManager::game_entities.erase(iter);

				if (auto phys_object = dynamic_cast<PhysicsObject*>(entity)) {
					physics.RemovePhysObject(phys_object);
				}

				fprintf(stderr, "Server: deleting physics object with ID %lld\n", reinterpret_cast<uintptr_t>(entity));
				// delete entity;
			}
			entities_pending_client_delete.push_back(reinterpret_cast<uintptr_t>(entity));
		}
	}
}

std::vector<std::pair<char*, int>> GameManager::GetServerBuf()
{
	std::vector<ModelInfo> model_infos;
	int particles = 0;
	for (GameEntity* entity : GameManager::game_entities) {

		auto drawable = dynamic_cast<Drawable*>(entity);
		if (drawable) {
			model_infos.push_back(ModelInfo{
				reinterpret_cast<uintptr_t>(entity),
				drawable->GetModelEnum(),
				drawable->GetAniMode(),
				drawable->GetParentTransform(),
				dynamic_cast<Player*>(entity) != nullptr
			});
		}

		// COLLIDER DEBUG VISUALIZERS
		auto collide = dynamic_cast<PhysicsObject*>(entity);
		if (collide) {
			std::vector<Collider*> colliders = collide->GetColliders();

			for each (Collider* collider in colliders)
			{
				if (collider->GetColliderShape() == Collider::Shape::CIRCLE) {
					auto circle = dynamic_cast<ColliderCircle*>(collider);
					model_infos.push_back(ModelInfo{
						reinterpret_cast<uintptr_t>(entity),
						DEBUG_CIRCLE,
						drawable->GetAniMode(),
						glm::translate(glm::vec3((circle->center.x), 0.0f, -(circle->center.y))) *
						glm::scale(glm::vec3(circle->radius)),
						false
						});
				}

				else {
					auto aabb = dynamic_cast<ColliderAABB*>(collider);
					const glm::vec2 avg = (aabb->minimum + aabb->maximum) / 2.0f;
					model_infos.push_back(ModelInfo{
						reinterpret_cast<uintptr_t>(entity),
						DEBUG_SQUARE,
						drawable->GetAniMode(),
						glm::translate(glm::vec3(avg.x, 0.0f, -(avg.y))),
						false
						});
				}
			}
		}
	}

	std::vector<PendingDeleteInfo> pending_delete_infos;
	for (uintptr_t model_id : entities_pending_client_delete) {
		pending_delete_infos.push_back(PendingDeleteInfo{ model_id });
	}
	entities_pending_client_delete = {};

	int i = 0;
	std::vector<std::pair<char*, int>> out_vec;
	for (auto & player : players_) {
		std::vector<SoundInfo> sound_infos = player->GetSounds();
		//Draw particles!
		if (player->dust_particle != nullptr)
		{
			model_infos.push_back(ModelInfo{
				reinterpret_cast<uintptr_t>(player->dust_particle),
				player->dust_particle->GetModelEnum(),
				player->dust_particle->GetAniMode(),
				player->dust_particle->GetParentTransform(),
				false
				});
		}
		
		ServerHeader sheader{};
		sheader.num_models = model_infos.size();
		sheader.num_sounds = sound_infos.size();
		sheader.num_pending_delete = pending_delete_infos.size();
		sheader.player_transform = player->GetParentTransform();
		sheader.player_sprinting = player->sprint;
		sheader.ui_open = player->ui_open;
		sheader.sale_confirm_ui_open = player->sale_confirm_ui_open; 
		sheader.holding_veggie = ModelEnum(0); 
		if (player->GetIsHolding()){
			if(auto vegetable = dynamic_cast<Vegetable*>(player->GetHoldEntity())) {
				if (vegetable != nullptr) {
					sheader.holding_veggie = vegetable->GetModelEnum(); 
				}
			}
		}
		sheader.time_remaining_seconds = GameManager::GetRemainingSeconds();
		sheader.time_max_seconds = GAME_TIME_MINS * 60.f;
		sheader.stamina_bar = player->curr_stamina;
		for (int i = 0; i < players_.size(); i++) {
			sheader.balance[i] = players_[i]->curr_balance;
		}
		

		auto server_buf = static_cast<char*>(malloc(GetBufSize(&sheader)));
		serverSerialize(server_buf, &sheader, model_infos.data(), sound_infos.data(), pending_delete_infos.data());
		out_vec.push_back(std::make_pair(server_buf, static_cast<int>(GetBufSize(&sheader))));

		player->ResetSoundBools();
	}
	
	return out_vec;
}

void GameManager::SetPlayerInput(const glm::vec2 move_input, const int player_index)
{
	players_[player_index]->move_input = move_input;
}

//TODO Merge these two with SetPlayerInput when NetworkPacket.h is updated
void GameManager::SetPlayerUse(const int player_index) {
	players_[player_index]->Use();
}

void GameManager::SetPlayerDrop(const int player_index) {
	players_[player_index]->Drop();
}
void GameManager::SetPlayerSprint(const int player_index, const bool sprinting)
{
	players_[player_index]->SetSprint(sprinting);
}
void GameManager::SetPlayerDance(const int player_index)
{
	players_[player_index]->Dance();
}

void GameManager::SetPlayerBuy(int player_index, VegetableType bought_vegetable)
{
	players_[player_index]->Buy(bought_vegetable);
}

void GameManager::SetPlayerBuy(int player_index, ModelEnum tool)
{
	if(tool == POISON)
		printf("buying poison\n");
	else if (tool == GLUE)
		printf("buying GLUE\n");
	else if (tool == SOJU)
		printf("buying SOJU\n");
	else if (tool == HOE)
		printf("buying HOE\n");
	players_[player_index]->Buy(tool);
}

void GameManager::SetPlayerSell(int player_index)
{
	players_[player_index]->Sell();
}

void GameManager::SetClosePlayerUI(int player_index) {
	players_[player_index]->CloseUI();
	players_[player_index]->CloseSaleUI();
}



double GameManager::GetFixedDeltaTime() {
	const std::chrono::duration<double> elapsed_seconds = curr_time_ - last_time_;
    return elapsed_seconds.count();
}


void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = std::chrono::steady_clock::now();
}

void GameManager::StartGameTime() {
	if (!isStarted) {
		game_end_time_ = std::chrono::steady_clock::now() + std::chrono::minutes(GAME_TIME_MINS);
		isStarted = true;
	}
}

double GameManager::GetRemainingSeconds() {
	auto curr_time = std::chrono::steady_clock::now();
	const std::chrono::duration<double> elapsed_seconds = game_end_time_ - curr_time;
	return elapsed_seconds.count();
}

std::default_random_engine* GameManager::GetRandomGenerator()
{
	return generator;
}
