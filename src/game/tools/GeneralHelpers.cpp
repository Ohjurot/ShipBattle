#include "pch.h" 
#include "GeneralHelpers.h"

MouseClickBounds GeneralHelpers::evaluateMouse(UINT x, UINT y) {
    // Moude Mounds
    const RECT bounds[] = {
        //  Left   Top      Right           Bottom
        {   390,   210,     390 + 310,      210 + 310       },
        {   365,   600,     365 + 359,      600 + 359       },
        {   762,   183,     762 + 83 * 10,  183 + 83 * 10   },
    };

    // Test bounds
    for (unsigned int i = 0; i < 3; i++) {
        // Check area bound
        if (
            (bounds[i].left <= x) && (x <= bounds[i].right) &&
            (bounds[i].top <= y) && (y <= bounds[i].bottom)
        ) {
            // Create object
            MouseClickBounds bounds;
            bounds.area = (MouseClickArea)(i + 1);
            
            // Check and calc kick tile
            if (i == 2) {
                bounds.tx = std::min<INT>(9, (x - 762) / 83);
                bounds.ty = std::min<INT>(9, (y - 183) / 83);
            }

            // Return
            return bounds;
        }
    }

    // Fallback
    return { MouseClickArea::NONE, 0, 0};
}

void GeneralHelpers::drawHitMarkers(UINT8 area[10][10], QuadManger& refManager) {
    for (unsigned int x = 0; x < 10; x++) {
        for (unsigned int y = 0; y < 10; y++) {
            switch (area[x][y]) {
                case 1:
                    auto qNoHit = EasyTex::textureAt(ET_TEXTURE_NO_HIT, GRID_ORIGINE_LEFT + GRID_STEP_RATE * x, GRID_ORIGINE_TOP + GRID_STEP_RATE * y, 50, GRID_SHIP_SCALE);
                    refManager.addQuad(qNoHit);
                    break;

                case 2:
                    auto qHit = EasyTex::textureAt(ET_TEXTURE_HIT, GRID_ORIGINE_LEFT + GRID_STEP_RATE * x, GRID_ORIGINE_TOP + GRID_STEP_RATE * y, 50, GRID_SHIP_SCALE);
                    refManager.addQuad(qHit);
                    break;
            }
        }
    }
}
