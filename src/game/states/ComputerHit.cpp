#include "pch.h"
#include "ComputerHit.h"

void GState_ComputerHit::init(GlobalState* ptrGlobalState) {
	m_ptrGlobalState = ptrGlobalState;
}

void GState_ComputerHit::activate() {
	// Play SFX
	m_ptrGlobalState->voiceArray[1]->play();
	m_ptrGlobalState->voiceArray[1]->setVolume(2.0f);

	m_timeEllapsed = 0;
}

void GState_ComputerHit::deactivate(){

}

INT GState_ComputerHit::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_HIT);
	refManager.addQuad(qInfobox);

	// Radar
	auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_OWN);
	refManager.addQuad(qRadar);

	// Draw computer ships
	ShipFunctions::drawShips(m_ptrGlobalState->playerShips, 5, refManager);

	// Draw hitmarkers
	GeneralHelpers::drawHitMarkers(m_ptrGlobalState->playfieldPlayer, refManager);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);

	// Advance time
	m_timeEllapsed += deltaTMs;
	if (m_timeEllapsed >= 1500) {
		// Count total PC hp
		unsigned int totalHp = 0;
		for (unsigned int i = 0; i < 5; i++) {
			totalHp += m_ptrGlobalState->playerShips[i].hp;
		}

		// Check if user has won
		if (totalHp == 0) {
			return 11;
		}
		else {
			// Player aim
			return 2;
		}
	}

	// Stay
	return -1;
}
