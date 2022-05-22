#include "GameManager.h"

#include "Network/NetworkPacket.h"

std::chrono::steady_clock::time_point GameManager::curr_time_ = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point GameManager::last_time_ = std::chrono::steady_clock::now();

std::vector<GameEntity*> GameManager::game_entities = {};
PhysicsEngine GameManager::physics = PhysicsEngine();
std::vector<PhysicsObject*> GameManager::physics_objects = {};


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
		// TODO: Check which player has the most money and break ties somehow
		if (players_.size() > 0) {
			players_[0]->playerHeight = goldPosition.y;
			players_[0]->SetWorldPosition(goldPosition);
			players_[0]->SetRotation(glm::vec3(0, 3.1415, 0));
			players_[0]->modelAnim = DANCE;
		}
		if (players_.size() > 1) {
			players_[1]->playerHeight = silverPosition.y;
			players_[1]->SetWorldPosition(silverPosition);
			players_[1]->SetRotation(glm::vec3(0, 1, 0));
			players_[1]->modelAnim = DANCE;
		}
		if (players_.size() > 2) {
			players_[2]->playerHeight = bronzePosition.y;
			players_[2]->SetWorldPosition(bronzePosition);
			players_[2]->SetRotation(glm::vec3(0, 1, 0));
			players_[2]->modelAnim = DANCE;
		}
		if (players_.size() > 3) {
			players_[3]->playerHeight = loserPosition.y;
			players_[3]->SetWorldPosition(loserPosition);
			players_[3]->SetRotation(glm::vec3(0, 1, 0));
			players_[3]->modelAnim = DANCE;
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

void GameManager::FixedUpdate()
{
	GameManager::UpdateFixedDeltaTime();

	bool victory_condition_met = GameManager::GetRemainingSeconds() <= timeToEndGame;
	if (victory_condition_met)
	{
		WinningFixedUpdate();
	}
	else {
		if (!eggplantSpawned && GameManager::GetRemainingSeconds() <= timeToSpawnEggplant) {
			AddEntities({ new Vegetable(VegetableType::GOLDEN_EGGPLANT, VEG_GOLDEN_EGGPLANT) });
			eggplantSpawned = true;
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
		//delete entity; TODO :/
		auto iter = std::find(GameManager::game_entities.begin(), GameManager::game_entities.end(), entity);
		GameManager::game_entities.erase(iter);

		if(auto phys_object = dynamic_cast<PhysicsObject*>(entity)) {
			physics.RemovePhysObject(phys_object);
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
	}

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
		sheader.player_transform = player->GetParentTransform();
		sheader.player_sprinting = player->sprint;
		sheader.ui_open = player->ui_open;
		sheader.time_remaining_seconds = GameManager::GetRemainingSeconds();
		sheader.time_max_seconds = GAME_TIME_MINS * 60.f;
		sheader.stamina_bar = player->curr_stamina;
		for (int i = 0; i < players_.size(); i++) {
			sheader.balance[i] = players_[i]->curr_balance;
		}
		

		auto server_buf = static_cast<char*>(malloc(GetBufSize(&sheader)));
		serverSerialize(server_buf, &sheader, model_infos.data(), sound_infos.data());
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
		game_end_time_ = std::chrono::steady_clock::now() + std::chrono::minutes(15);
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
