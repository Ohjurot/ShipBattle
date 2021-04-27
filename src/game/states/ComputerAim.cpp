#include "pch.h"
#include "ComputerAim.h"

void GState_ComputerAim::init(GlobalState* ptrGlobalState) {
	m_ptrGlobalState = ptrGlobalState;
}

void GState_ComputerAim::activate() {
	// Call AI
	UINT ax, ay;
	GameAI::aiPlaceShot(m_ptrGlobalState, &ax, &ay);

	// Mark aim
	m_ptrGlobalState->aimX = ax;
	m_ptrGlobalState->aimY = ay;

	// Evaluate aim
	for (unsigned int i = 0; i < 5; i++) {
		Ship* pShip = &m_ptrGlobalState->playerShips[i];

		if (
			(!pShip->isVertical && pShip->posy == ay && pShip->posx <= ax && pShip->posx + pShip->length > ax) ||
			(pShip->isVertical && pShip->posx == ax && pShip->posy <= ay && pShip->posy + pShip->length > ay)
			) {
			m_ptrGlobalState->lastShotShipIdx = i;
		}
	}
}

void GState_ComputerAim::deactivate(){
	m_elapsedTime = 0;
}

INT GState_ComputerAim::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_WAIT);
	refManager.addQuad(qInfobox);

	// Radar
	auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_OWN);
	refManager.addQuad(qRadar);

	// Draw computer ships
	ShipFunctions::drawShips(m_ptrGlobalState->playerShips, 5, refManager);

	// Draw hitmarkers
	GeneralHelpers::drawHitMarkers(m_ptrGlobalState->playfieldPlayer, refManager);
	
	// Wait time laps
	m_elapsedTime += deltaTMs;
	if (m_elapsedTime >= 1000) {
		// Computer shot traveling
		return 7;
	}

	// Stay
	return -1;
}
