#include "pch.h"
#include "ShipFunctions.h"

bool ShipFunctions::shipCheckBounds(Ship* ptrShip) {
    if (ptrShip->isVertical) {
        return ((ptrShip->posy + ptrShip->length) <= 10);
    }
    else {
        return ((ptrShip->posx + ptrShip->length) <= 10);
    }
}

bool ShipFunctions::shipCheckCollision(Ship* arrShips, unsigned int count, unsigned int index) {
    if (count == 0) {
        return true;
    }

    // Bounds
    if (index >= count) {
        return false;
    }

    // Get current ship and rect
    Ship* placeShip = &arrShips[index];

    return shipCheckCollision(arrShips, placeShip, count, index);
}

bool ShipFunctions::shipCheckCollision(Ship* arrShips, Ship* checker, unsigned int count, unsigned int index) {
    if (count == 0) {
        return true;
    }

    // Bounds
    if (index >= count) {
        return false;
    }

    // Get current ship and rect
    Ship* placeShip = checker;

    // Check for each ship
    for (unsigned int i = 0; i < count; i++) {
        // Get other pointer
        Ship* otherShip = &arrShips[i];

        // Only for other ships
        if (i != index && shipIntersect(placeShip, otherShip)) {
            return false;
        }
    }

    // Passed
    return true;
}

void ShipFunctions::randomShips(Ship* arrShips, unsigned int count) {
    // Gennerate all ships
    for (unsigned int i = 0; i < count; i++) {
        do {
            // Ge random doubles
            double rDX = std::rand() / (double)RAND_MAX;
            double rDY = std::rand() / (double)RAND_MAX;
            double rDD = std::rand() / (double)RAND_MAX;

            // Compute int vars
            arrShips[i].posx = std::min<UINT8>(rDX * 10, 9);
            arrShips[i].posy = std::min<UINT8>(rDY * 10, 9);
            arrShips[i].isVertical = rDD >= 0.5f;

        } while (!shipCheckBounds(&arrShips[i]) || !shipCheckCollision(arrShips, i + 1, i));
    }
}

void ShipFunctions::drawShip(Ship* ship, QuadManger& manager) {
    // Base index
    unsigned int shipTexture = (ship->isVertical ? 21 : 0);
    // Increment by length
    switch (ship->length) {
        // L2
        case 2:
            shipTexture += 0;
            break;
            
        // L3
        case 3:
            shipTexture += 1;
            break;

        // L4
        case 4:
            if (ship->alternate) {
                shipTexture += 2;
            }
            else {
                shipTexture += 3;
            }
            break;

        // L5
        case 5:
            shipTexture += 4;
            break;
    }

    // Craft ship
    auto shipQuad = EasyTex::textureAt(
        shipTexture,
        GRID_ORIGINE_LEFT + GRID_STEP_RATE * ship->posx,
        GRID_ORIGINE_TOP + GRID_STEP_RATE * ship->posy,
        DRAWPLANE_SHIP,
        GRID_SHIP_SCALE
    );

    // Draw ship
    manager.addQuad(shipQuad);
}

void ShipFunctions::drawShips(Ship* arrShips, unsigned int count, QuadManger& manager) {
    for (unsigned int i = 0; i < count; i++) {
        if (arrShips[i].visible) {
            drawShip(&arrShips[i], manager);
        }
    }
}

bool ShipFunctions::shipIntersect(Ship* s1, Ship* s2)
{
    // Line 1
    int l1x1 = s1->posx;
    int l1y1 = s1->posy;
    int l1x2 = s1->isVertical ? l1x1 : s1->posx + s1->length - 1;
    int l1y2 = s1->isVertical ? s1->posy + s1->length - 1 : l1y1;

    // Line 2
    int l2x1 = s2->posx;
    int l2y1 = s2->posy;
    int l2x2 = s2->isVertical ? l2x1 : s2->posx + s2->length - 1;
    int l2y2 = s2->isVertical ? s2->posy + s2->length - 1 : l2y1;

    /*
    *  Intersection test seen as AABB
    * 
    *  cs  yo              Conditions (or):   
    *  xo  a ------ o      1) b2 > a1 
    *      |   s1   |      2) a2 > b1
    *      |        |   
    *      o ------ b  xo
    *              yo  cs
    */

    //  cs                              xo                              yo
    if ((l2x2 < l1x1 && l2y2 < l1y1) || (l2x1 < l1x1 && l2x2 < l1x1) || (l2y1 < l1y1 && l2y2 < l1y1)) return false;
    if ((l2x1 > l1x2 && l2y1 > l1y2) || (l2x1 > l1x2 && l2x2 > l1x2) || (l2y1 > l1y2 && l2y2 > l1y2)) return false;
    return true;
}
