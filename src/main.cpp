#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GJGameLevel.hpp>

using namespace geode::prelude;

bool enabled = Mod::get()->getSettingValue<bool>("enable");
bool jump = Mod::get()->getSettingValue<bool>("jump");
bool left = Mod::get()->getSettingValue<bool>("left");
bool right = Mod::get()->getSettingValue<bool>("right");
bool p1 = Mod::get()->getSettingValue<bool>("p1");
bool p2 = Mod::get()->getSettingValue<bool>("p2");

bool isSafeMode(GJBaseGameLayer* bgl) {
	if (!bgl) return false;

	bool isPlatformer = bgl->m_levelSettings->m_platformerMode;
	bool isTwoPlayer = bgl->m_levelSettings->m_twoPlayerMode;

	if (!enabled) 
		return false;

	if ((!isTwoPlayer && !p1) || (isTwoPlayer && (!p1 && !p2)))
		return false;

	if ((!isPlatformer && !jump) || (isPlatformer && (!jump && !left && !right)))
		return false;

	return true;
}

class $modify(GJBaseGameLayer) {
	void handleButton(bool down, int button, bool isPlayer1) {

		if (enabled)
			if ((isPlayer1 && p1) || (!isPlayer1 && p2))
				if ((jump && button == 1) || (left && button == 2) || (right && button == 3))
					down = !down;

		return GJBaseGameLayer::handleButton(down, button, isPlayer1);
	}
};

class $modify(PlayLayer) {
	void resetLevel() {
		PlayLayer::resetLevel();

		if (enabled) {
			for (int i = 1; i < ((m_levelSettings->m_platformerMode) ? 4 : 2); i++) {
				if (p1) 
					handleButton(false, i, true);
				if (p2 && m_levelSettings->m_twoPlayerMode) 
					handleButton(false, i, false);
			}
		}

		return;
	}

	void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
		if (!isSafeMode(this)) 
			PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
	}

	void levelComplete() {
		bool wasTestMode = m_isTestMode;

		if (isSafeMode(this))
			m_isTestMode = true;

		PlayLayer::levelComplete();
		m_isTestMode = wasTestMode;
	}

};

class $modify(EditorUI) {
	void onPlaytest(CCObject* sender) {
		EditorUI::onPlaytest(sender);

		auto layer = GJBaseGameLayer::get();

		if (enabled) {
			for (int i = 1; i < ((layer->m_levelSettings->m_platformerMode) ? 4 : 2); i++) {
				if (p1) 
					layer->handleButton(false, i, true);
				if (p2 && layer->m_levelSettings->m_twoPlayerMode) 
					layer->handleButton(false, i, false);
			}
		}

		return;
	}
};

class $modify(GJGameLevel) {
	void savePercentage(int percent, bool isPracticeMode, int clicks, int attempts, bool isChkValid) {
		if (!isSafeMode(GJBaseGameLayer::get()))
			GJGameLevel::savePercentage(percent, isPracticeMode, clicks, attempts, isChkValid);
	}
};

$execute {
    geode::listenForSettingChanges("enable", +[](bool  value) {
        enabled = value;
    });
	
	geode::listenForSettingChanges("jump", +[](bool  value) {
        jump = value;
    });
	
	geode::listenForSettingChanges("left", +[](bool  value) {
        left = value;
    });
	
	geode::listenForSettingChanges("right", +[](bool  value) {
        right = value;
    });
	
	geode::listenForSettingChanges("p1", +[](bool  value) {
        p1 = value;
    });

	geode::listenForSettingChanges("p2", +[](bool  value) {
        p2 = value;
    });
};