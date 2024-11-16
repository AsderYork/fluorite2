#pragma once
#include <map>
#include <functional>
#include <vector>

namespace fluorite {

	/**
	 * @brief Controlls the execution flow of the game. Performs initialization and shutdown, controlls main loop
	 */
    class GameloopController {
		public:
			enum Gamestage {
				PRE_INIT,
				INIT,
				PRE_FRAME,
				FRAME,
				POST_FRAME,
				SHUTDOWN,
			};
			
			/**
			 * @brief Add method for execution on one of the game stages
			 * During a single stage, order of registered events executions is not guaranteed
			 * so if there is a need to enforce order, it is recomended to register events on separate
			 * stages.
			 * 
			 * @param stage Stage on which event callback is executed
			 * @param callback Callback which would be called
			 */
			void registerEvent(Gamestage stage, std::function<bool(GameloopController*, float)> callback);
			
			/**
			 * @brief Executes all registered events in stage order, including main game loop
			 */
			void start();

			/**
			 * @brief finish current game loop and proceed to shutdown
			 * 
			 */
			void initiateShutdown();

		private:

			struct GamestageStorage {
				std::vector<std::function<bool(GameloopController*, float)>> events;
				bool execute(GameloopController* self, float delta);
				void registerEvent(std::function<bool(GameloopController*, float)> callback);
			};

			struct StageSetExecutionResult {
				bool isSuccessfull;
				float executionTime;
			};

			std::map<Gamestage, GamestageStorage> gamestages;
			bool continueRunning = true;

			bool executeStage(Gamestage stage, float delta = 0);
			StageSetExecutionResult executeStagesSet(std::vector<Gamestage> set, float delta = 0);

    };

}