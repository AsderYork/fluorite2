#include <SystemServices/GameloopController/GameloopController.hpp>
#include <chrono>

//we just assume that first frame would be 15mils. For every frame after that we use duration of the previous frame
#define DEFAULT_FRAME_DELTA 0.15f

namespace fluorite {

    bool GameloopController::executeStage(GameloopController::Gamestage stage, float delta) {
        auto currGamestage = gamestages.find(stage);
        if (currGamestage != gamestages.end()) {
            return currGamestage->second.execute(this, delta);
        }
        return true;
    }

    void GameloopController::registerEvent(GameloopController::Gamestage stage, std::function<bool(GameloopController*, float)> callback) {
        
        auto currGamestage = gamestages.find(stage);
        if (currGamestage == gamestages.end()) {
            currGamestage = gamestages.insert({stage, GamestageStorage()}).first;
        }
        currGamestage->second.registerEvent(callback);

    }

    GameloopController::StageSetExecutionResult GameloopController::executeStagesSet(std::vector<Gamestage> set, float delta) {
        auto timeFrameStart = std::chrono::high_resolution_clock::now();
        
        bool stageSetSuccessful = true;

        for(auto stageInSet : set) {
            stageSetSuccessful = executeStage(stageInSet, delta);
            if(!stageSetSuccessful) {break;}
        }
        
        auto timeFrameEnd = std::chrono::high_resolution_clock::now();

        return {stageSetSuccessful, std::chrono::duration<float>(timeFrameEnd - timeFrameStart).count()};
    }

    void GameloopController::start() {

        bool initSuccess = executeStagesSet({Gamestage::PRE_INIT, Gamestage::INIT}).isSuccessfull;

        if(initSuccess) {

            auto frameExecResult = StageSetExecutionResult({true, DEFAULT_FRAME_DELTA});
			while (frameExecResult.isSuccessfull && continueRunning) {
				frameExecResult = executeStagesSet({Gamestage::PRE_FRAME, Gamestage::FRAME, Gamestage::POST_FRAME}, frameExecResult.executionTime);
			}

        }

        executeStagesSet({Gamestage::SHUTDOWN});
    }

    void GameloopController::GamestageStorage::registerEvent(std::function<bool(GameloopController*, float)> callback) {
            events.push_back(callback);
    }

    bool GameloopController::GamestageStorage::execute(GameloopController* self, float delta) {
        for(auto& event : events) {
            if(!event(self, delta)) {
                return false;
            }
        }
        return true;
    }

}
