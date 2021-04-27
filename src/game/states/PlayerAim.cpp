#include "pch.h"
#include "PlayerAim.h"

void GState_PlayerAim::init(GlobalState* ptrGlobalState) {
	m_ptrGlobalState = ptrGlobalState;
}

void GState_PlayerAim::activate() {

}

void GState_PlayerAim::deactivate() {

}

INT GState_PlayerAim::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_PLACE_SHOT);
	refManager.addQuad(qInfobox);

	// Radar
	auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_ENEMY);
	refManager.addQuad(qRadar);

	// Draw computer ships
	ShipFunctions::drawShips(m_ptrGlobalState->computerShips, 5, refManager);
	
	// Draw hitmarkers
	GeneralHelpers::drawHitMarkers(m_ptrGlobalState->playfieldComputer, refManager);

	// Evaluate mouse
	auto me = GeneralHelpers::evaluateMouse(refIOState.mouseX, refIOState.mouseY);

	// Draw crossir aim
	if (me.area == MouseClickArea::PLAYFIELD) {
		auto qCrosshair = EasyTex::textureAt(ET_TEXTURE_CROSSHAIR, GRID_ORIGINE_LEFT + me.tx * GRID_STEP_RATE, GRID_ORIGINE_TOP + me.ty * GRID_STEP_RATE, 30, GRID_SHIP_SCALE);
		refManager.addQuad(qCrosshair);

		// Check if shot needs to be placed
		if (refIOState.mouseIsDown) {
			// Check if area is not shot
			if (m_ptrGlobalState->playfieldComputer[me.tx][me.ty] == 0) {	
				// Check hit
				for (unsigned int i = 0; i < 5; i++) {
					Ship* pShip = &m_ptrGlobalState->computerShips[i];

					if (
						(!pShip->isVertical && pShip->posy == me.ty && pShip->posx <= me.tx && pShip->posx + pShip->length > me.tx) ||
						(pShip->isVertical && pShip->posx == me.tx && pShip->posy <= me.ty && pShip->posy + pShip->length > me.ty)
					) {
						m_ptrGlobalState->lastShotShipIdx = i;
					}
				}

				// Next state
				m_ptrGlobalState->aimX = me.tx;
				m_ptrGlobalState->aimY = me.ty;
				return 3;
			}
		}
	}

	// Keep state
	return -1;
}
