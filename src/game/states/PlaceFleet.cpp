#include "pch.h"
#include "PlaceFleet.h"

void GState_PlaceFleet::init(GlobalState* ptrGlobalState) {
	// Set global states
	m_ptrGlobalState = ptrGlobalState;
}

void GState_PlaceFleet::activate() {
	// Clear hit arrray
	memset(m_ptrGlobalState->playfieldComputer, 0, 10 * 10 * sizeof(UINT8));
	memset(m_ptrGlobalState->playfieldPlayer, 0, 10 * 10 * sizeof(UINT8));
	m_ptrGlobalState->aiPossibleHits = 10 * 10;

	// Clear ai array
	memset(m_ptrGlobalState->aiPlayField, (UINT8)GameAIOportunity::RANDOM_PLACE, sizeof(UINT8) * 10 * 10);

	// Craft player ships
	m_ptrGlobalState->playerShips[0] = { 0, 0, 2, false, false, true, 2 };
	m_ptrGlobalState->playerShips[1] = { 0, 0, 3, false, false, true, 3 };
	m_ptrGlobalState->playerShips[2] = { 0, 0, 4, false, false, true, 4 };
	m_ptrGlobalState->playerShips[3] = { 0, 0, 4, false, true, true, 4 };
	m_ptrGlobalState->playerShips[4] = { 0, 0, 5, false, false, true, 5 };

	// Copy to computer
	memcpy(m_ptrGlobalState->computerShips, m_ptrGlobalState->playerShips, sizeof(Ship) * 5);

	// Set computer to invisible
	for (unsigned int i = 0; i < 5; i++) {
		m_ptrGlobalState->computerShips[i].visible = false;
	}

	// Randomize positions
	ShipFunctions::randomShips(m_ptrGlobalState->playerShips, 5);
	ShipFunctions::randomShips(m_ptrGlobalState->computerShips, 5);
}

void GState_PlaceFleet::deactivate() {

}

INT GState_PlaceFleet::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_GAME, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);

	// Info box
	auto qInfobox = GENNERATE_INFO_BOX(ET_TEXTURE_STAT_PLACE_FLEET);
	refManager.addQuad(qInfobox);

	// Radar
	auto qRadar = GENNERATE_RADAR(ET_TEXTURE_RADAR_OWN);
	refManager.addQuad(qRadar);

	// Draw players ships
	ShipFunctions::drawShips(m_ptrGlobalState->playerShips, 5, refManager);

	// Mouse
	auto me = GeneralHelpers::evaluateMouse(refIOState.mouseX, refIOState.mouseY);

	// Klick actions
	if (refIOState.mouseIsDown && !m_debounce) {
		m_debounce = true;
		switch (me.area) {
			// Finish loadout
			case MouseClickArea::INFOBOX:
			case MouseClickArea::RADAR: {
				// Drop is not allowd
				if (m_pickedShipIdx == -1) {
					return 2;
				}
				break;
			}

			// Pick / Drop ship
			case MouseClickArea::PLAYFIELD: {
				// DROP
				if (m_pickedShipIdx > -1) {
					// Check if pos is valid
					if (
						ShipFunctions::shipCheckBounds(&m_pickShip) &&
						ShipFunctions::shipCheckCollision(m_ptrGlobalState->playerShips, &m_pickShip, 5, m_pickedShipIdx)
					) {
						// Drop ship
						m_ptrGlobalState->playerShips[m_pickedShipIdx] = m_pickShip;
						m_pickedShipIdx = -1;
					}
				}
				// PICK
				else {
					// Find ship
					Ship* ptrPick = nullptr;
					for (unsigned int i = 0; i < 5; i++) {
						// Check coords
						if (m_ptrGlobalState->playerShips[i].posx == me.tx && m_ptrGlobalState->playerShips[i].posy == me.ty) {
							ptrPick = &m_ptrGlobalState->playerShips[i];
							m_pickedShipIdx = i;
						}
					}

					// Pick ship
					if (ptrPick) {
						memcpy(&m_pickShip, ptrPick, sizeof(Ship));
						ptrPick->visible = false;
					}
				}

				break;
			}
		}
	}
	else if (!refIOState.mouseIsDown) {
		m_debounce = false;
	}

	// Rotate ship
	if (m_pickedShipIdx > -1) {
		if (refIOState.enterIsDown && !m_debounceKey) {
			m_debounceKey = true;
			m_pickShip.isVertical = !m_pickShip.isVertical;
		} else if (!refIOState.enterIsDown) {
			m_debounceKey = false;
		}
	}

	// Draw moving ship
	if (m_pickedShipIdx > -1) {
		// Update pos
		if (me.area == MouseClickArea::PLAYFIELD) {
			m_pickShip.posx = me.tx;
			m_pickShip.posy = me.ty;
		}

		// Draw ship
		ShipFunctions::drawShip(&m_pickShip, refManager);
	}
	
	// No change
	return -1;
}
