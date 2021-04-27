#include "pch.h"
#include "MainMenue.h"

void GState_MainMenue::init(GlobalState* ptrGlobalState) {
	// ...
}

void GState_MainMenue::activate() {
	// TODO: Start menu soundtrack
}

void GState_MainMenue::deactivate() {
	// TODO: Stop menu soundtrack 
}

INT GState_MainMenue::update(UINT64 deltaTMs, QuadManger& refManager, const GameIOState& refIOState) {
	// Draw Background
	auto qBackground = EasyTex::textureAt(ET_TEXTURE_BACKGROUND_MAIN, 0, 0, 5);
	refManager.addQuad(qBackground);

	// Draw cursor
	auto qCursor = GENNERATE_CURSOR();
	refManager.addQuad(qCursor);
	
	// Check if enter was pressed
	if (refIOState.enterIsDown) {
		// Forward to state 1
		return 1;
	}

	// Kepp state
	return -1;
}
