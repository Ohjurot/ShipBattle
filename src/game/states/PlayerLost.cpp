#include "pch.h"
#include "PlayerLost.h"

void GState_PlayerLost::init(GlobalState* ptrGlobalState) {
	m_ptrGlobalState = ptrGlobalState;
}

void GState_PlayerLost::activate() {
	m_debounce = false;
	m_showOwnShips = false;

	// Stop background
	m_ptrGlobalState->voiceArray[5]->stop();

	// Play lost sfx
	m_ptrGlobalState->voiceArray[3]->play();
	m_ptrGlobalState->voiceArray[3]->setVolume(3.0f);

	// Enable all enemy ships
	for (unsigned int i = 0; i < 5; i++) {
		m_ptrGlobalState->computerShips[i].visible = true;
	}
}

void GState_PlayerLost::deactivate() {
	// Play background
	m_ptrGlobalState->voiceArray[5]->play(true);
}

INT GState_PlayerLost::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_GAME_LOST);
	refManager.addQuad(qInfobox);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);

	// Check witch set to show
	if (m_showOwnShips) {
		// Radar
		auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_OWN);
		refManager.addQuad(qRadar);

		// Draw computer ships
		ShipFunctions::drawShips(m_ptrGlobalState->playerShips, 5, refManager);

		// Draw hitmarkers
		GeneralHelpers::drawHitMarkers(m_ptrGlobalState->playfieldPlayer, refManager);
	}
	else {
		// Radar
		auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_ENEMY);
		refManager.addQuad(qRadar);

		// Draw computer ships
		ShipFunctions::drawShips(m_ptrGlobalState->computerShips, 5, refManager);

		// Draw hitmarkers
		GeneralHelpers::drawHitMarkers(m_ptrGlobalState->playfieldComputer, refManager);
	}

	// Evalue Own <--> Enemy ships change
	auto me = GeneralHelpers::evaluateMouse(refIOState.mouseX, refIOState.mouseY);
	if (me.area == MouseClickArea::RADAR && refIOState.mouseIsDown && !m_debounce) {
		m_debounce = true;
		m_showOwnShips = !m_showOwnShips;
	}
	else if (!refIOState.mouseIsDown) {
		m_debounce = false;
	}

	// Evaluate exit
	if (me.area == MouseClickArea::INFOBOX && refIOState.mouseIsDown) {
		// Back to start
		return  0;
	}

	// Stay
	return -1;
}
