#include "pch.h"
#include "ComputerNoHit.h"

void GState_ComputerNoHit::init(GlobalState* ptrGlobalState) {
	m_ptrGlobalState = ptrGlobalState;
}

void GState_ComputerNoHit::activate() {
	// Play SFX
	m_ptrGlobalState->voiceArray[2]->play();
	m_ptrGlobalState->voiceArray[2]->setVolume(4.0f);

	m_timeEllapsed = 0;
}

void GState_ComputerNoHit::deactivate() {

}

INT GState_ComputerNoHit::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_NO_HIT);
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

	// Check time
	m_timeEllapsed += deltaTMs;
	if (m_timeEllapsed >= 1500) {
		// Player aim
		return 2;
	}

	// Stay
	return -1;
}
