#include "pch.h"
#include "PlayerShooting.h"

void GState_PlayerShooting::init(GlobalState* ptrGlobalState) {
	m_ptrGlobalState = ptrGlobalState;
}

void GState_PlayerShooting::activate() {
	// Play SFX
	m_ptrGlobalState->voiceArray[0]->play();

	m_timeEllapsed = 0;
}

void GState_PlayerShooting::deactivate() {
	// Place shot
	if (m_ptrGlobalState->lastShotShipIdx >= 0) {
		m_ptrGlobalState->playfieldComputer[m_ptrGlobalState->aimX][m_ptrGlobalState->aimY] = 2;
		m_ptrGlobalState->computerShips[m_ptrGlobalState->lastShotShipIdx].hp--;
		if (m_ptrGlobalState->computerShips[m_ptrGlobalState->lastShotShipIdx].hp == 0) {
			m_ptrGlobalState->computerShips[m_ptrGlobalState->lastShotShipIdx].visible = true;
		}
	} else {
		m_ptrGlobalState->playfieldComputer[m_ptrGlobalState->aimX][m_ptrGlobalState->aimY] = 1;
	}

	// Clear hit ship
	m_ptrGlobalState->lastShotShipIdx = -1;
}

INT GState_PlayerShooting::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_SHOT_TRAVEL);
	refManager.addQuad(qInfobox);

	// Radar
	auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_ENEMY);
	refManager.addQuad(qRadar);

	// Draw computer ships
	ShipFunctions::drawShips(m_ptrGlobalState->computerShips, 5, refManager);

	// Draw hitmarkers
	GeneralHelpers::drawHitMarkers(m_ptrGlobalState->playfieldComputer, refManager);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);

	auto qCrosshair = EasyTex::textureAt(ET_TEXTURE_CROSSHAIR, GRID_ORIGINE_LEFT + m_ptrGlobalState->aimX * GRID_STEP_RATE, GRID_ORIGINE_TOP + m_ptrGlobalState->aimY * GRID_STEP_RATE, 30, GRID_SHIP_SCALE);
	refManager.addQuad(qCrosshair);
	
	// Advance time
	m_timeEllapsed += deltaTMs;
	if (m_timeEllapsed >= 3000) {
		// Transition to hit / no hit
		if (m_ptrGlobalState->lastShotShipIdx >= 0) {
			return 5;
		}
		else {
			return 4;
		}
	}

	// Stay
	return -1;
}
