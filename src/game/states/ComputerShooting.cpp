#include "pch.h"
#include "ComputerShooting.h"

void GState_ComputerShooting::init(GlobalState* ptrGlobalState) {
	m_ptrGlobalState = ptrGlobalState;
}

void GState_ComputerShooting::activate() {
	// Play SFX
	m_ptrGlobalState->voiceArray[0]->play();
	
	m_timeEllapsed = 0;
}

void GState_ComputerShooting::deactivate() {
	// Reset
	if (m_ptrGlobalState->lastShotShipIdx >= 0) {
		m_ptrGlobalState->playerShips[m_ptrGlobalState->lastShotShipIdx].hp--;
	}
	m_ptrGlobalState->lastShotShipIdx = -1;
}

INT GState_ComputerShooting::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_SHOT_TRAVEL);
	refManager.addQuad(qInfobox);

	// Radar
	auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_OWN);
	refManager.addQuad(qRadar);

	// Draw computer ships
	ShipFunctions::drawShips(m_ptrGlobalState->playerShips, 5, refManager);

	// Draw hitmarkers
	GeneralHelpers::drawHitMarkers(m_ptrGlobalState->playfieldPlayer, refManager);

	// Draw crosshair
	auto qCrosshair = EasyTex::textureAt(ET_TEXTURE_CROSSHAIR, GRID_ORIGINE_LEFT + m_ptrGlobalState->aimX * GRID_STEP_RATE, GRID_ORIGINE_TOP + m_ptrGlobalState->aimY * GRID_STEP_RATE, 30, GRID_SHIP_SCALE);
	refManager.addQuad(qCrosshair);

	// Elaps time
	m_timeEllapsed += deltaTMs;
	if (m_timeEllapsed >= 3000) {
		// Check which state
		if (m_ptrGlobalState->lastShotShipIdx >= 0) {
			m_ptrGlobalState->playfieldPlayer[m_ptrGlobalState->aimX][m_ptrGlobalState->aimY] = 2;
			return 9;
		}
		else {
			m_ptrGlobalState->playfieldPlayer[m_ptrGlobalState->aimX][m_ptrGlobalState->aimY] = 1;
			return 8;
		}
	}

	// Stay 
	return -1;
}
