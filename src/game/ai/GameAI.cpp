#include "pch.h"
#include "GameAI.h"

void GameAI::aiPlaceShot(GlobalState* ptrState, UINT* ptrTargetPX, UINT* ptrTargetPY){
	// Update AI Map
	updateAiMap(ptrState);

	// Check if use random or kill
	if ((std::rand() / RAND_MAX) < 0.75f) {
		// Find oportunity
		POINT opertunitys[10 * 10];
		unsigned int opertunityCount = aiKillShips(ptrState, opertunitys, 10 * 10);
		
		// Only if oportunity was found
		if (opertunityCount) {
			// Weight opertunity
			float opertunityWeights[10 * 10];
			aiWeightOpertunitys(ptrState, opertunitys, opertunityWeights, opertunityCount);

			// Max opertunity
			float maxWeight;
			UINT maxWeightCount;
			aiWeightCompare(opertunityWeights, opertunityCount, &maxWeightCount, &maxWeight);

			// Random oportunity index
			UINT opIndex = std::min<UINT>(maxWeightCount - 1, (std::rand() / (float)RAND_MAX) * maxWeightCount);

			// Find
			UINT opDistance = 0;
			for (unsigned int i = 0; i < opertunityCount; i++) {
				// Compare if matched
				if (opertunityWeights[i] == maxWeight) {
					opDistance++;
					// Check if reached
					if (opDistance == maxWeightCount) {
						// Set oportunity
						*ptrTargetPX = opertunitys[i].x;
						*ptrTargetPY = opertunitys[i].y;
						return;
					}
				}
			}
		}
	}
	
	// Gennerate random samples
	POINT ramdomSamples[8];
	float randomWeights[8];
	UINT randowmCount = std::min<UINT>(8, ptrState->aiPossibleHits);

	// Sample random
	for (unsigned int i = 0; i < randowmCount; i++) {
		// Gennerate
		do {
			ramdomSamples[i].x = std::min<INT>(9, (std::rand() / (float)RAND_MAX) * 10);
			ramdomSamples[i].y = std::min<INT>(9, (std::rand() / (float)RAND_MAX) * 10);
		} while (ptrState->aiPlayField[ramdomSamples[i].x][ramdomSamples[i].y] != GameAIOportunity::RANDOM_PLACE);

		// Analyse
		randomWeights[i] = aiWeightPoint(ptrState, ramdomSamples[i].x, ramdomSamples[i].y, 2);
	}

	// Evaluate weight
	float topWeight;
	UINT topWeightCount;
	aiWeightCompare(randomWeights, randowmCount, &topWeightCount, &topWeight);

	// Decide if use weight or totaly random
	if (topWeightCount && (std::rand() / (float)RAND_MAX) < 0.75) {
		// Random index
		UINT rindex = std::min<UINT>(topWeightCount - 1, (std::rand() / (float)RAND_MAX) * topWeightCount);

		// Find apropriat
		UINT countPos = 0;
		for (unsigned int i = 0; i < randowmCount; i++) {
			// check equal
			if (fabs(randomWeights[i] - topWeight) <= 0.2f) {
				countPos++;
				// Check if reached
				if (countPos == topWeightCount) {
					*ptrTargetPX = ramdomSamples[i].x;
					*ptrTargetPY = ramdomSamples[i].y;
					return;
				}
			}
		}
	}
	// Full random
	else {
		// Random index
		UINT rindex = std::min<UINT>(randowmCount - 1, (std::rand() / (float)RAND_MAX) * randowmCount);
		*ptrTargetPX = ramdomSamples[rindex].x;
		*ptrTargetPY = ramdomSamples[rindex].y;
	}
}

void GameAI::aiWeightOpertunitys(GlobalState* ptrState, POINT* arrayOportunitys, float* arrayWeights, unsigned int opertunitysCount) {
	// Min ship
	UINT shipMin = 5;
	for (unsigned int i = 0; i < 5; i++) {
		if (ptrState->playerShips[i].hp != 0 && ptrState->playerShips[i].length < shipMin) {
			shipMin = ptrState->playerShips[i].length;
		}
	}
	
	// Weight oportuniys by souroundings
	for (unsigned int i = 0; i < opertunitysCount; i++) {
		// Basic weighing
		arrayWeights[i] = aiWeightPoint(ptrState, arrayOportunitys[i].x, arrayOportunitys[i].y, shipMin);
	}
}

void GameAI::aiWeightCompare(float* arrWeights, unsigned int count, unsigned int* ptrCount, float* ptrMax){
	// Find top weight and count
	float topWeight = 0.0f;
	UINT topWeightCount = 0;

	for (unsigned int i = 0; i < count; i++) {
		// Weight same ?
		if (fabs(arrWeights[i] - topWeight) <= 0.2f) {
			topWeightCount++;
			// Weight highter
		}
		else if (fabs(arrWeights[i] - topWeight) > 0.2f && arrWeights[i] > topWeight) {
			topWeight = arrWeights[i];
			topWeightCount = 1;
		}
	}

	// Output
	*ptrCount = topWeightCount;
	*ptrMax = topWeight;
}

float GameAI::aiWeightPoint(GlobalState* ptrState, UINT px, UINT py, UINT r) {
	// Sample all directions
	return (
		aiWeightDirection(ptrState, px, py, r, 0, +1) + 
		aiWeightDirection(ptrState, px, py, r, 0, -1) +
		aiWeightDirection(ptrState, px, py, r, +1, 0) +
		aiWeightDirection(ptrState, px, py, r, -1, 0)
	);
}

float GameAI::aiWeightDirection(GlobalState* ptrState, UINT px, UINT py, UINT r, INT xd, INT yd) {
	float weight = 0.0f;
	
	// Sample radius
	for (unsigned int o = 0; o < r; o++) {
		// New coords
		INT x = px + ((o + 1) * xd);
		INT y = py + ((o + 1) * yd);

		// Check if in bounds
		if (x >= 0 && y >= 0) {
			// Check current state
			switch (ptrState->aiPlayField[x][y]) {
				// Return if not possible
				case GameAIOportunity::SHIP_IMPOSSIBLE:
					return weight;
					break;

				// Add weight
				case GameAIOportunity::RANDOM_PLACE:
					weight += 1.0f;
					break;

				// Hit
				case GameAIOportunity::HIT_SHIP_ALIVE:
					weight += 2.5f;
					break;
			}
		}
		// Out of bounds
		else {
			// Small weight
			weight += 0.5f;
		}
	}
	
	// Return weight
	return weight;
}

unsigned int GameAI::aiKillShips(GlobalState* ptrState, POINT* pointArray, unsigned int size) {
	unsigned int pointsCount = 0;

	// Walk array
	for (LONG x = 0; x < 10; x++) {
		for (LONG y = 0; y < 10; y++) {
			// Check if ship was hit there
			if (ptrState->aiPlayField[x][y] == GameAIOportunity::HIT_SHIP_ALIVE) {
				// Check all avalilble directions
				// Up
				if (pointsCount < size && y != 0 && ptrState->aiPlayField[x][y - 1] == GameAIOportunity::RANDOM_PLACE) {
					pointArray[pointsCount] = {x, y - 1};
					pointsCount++;
				}
				// Down
				if (pointsCount < size && y != 9 && ptrState->aiPlayField[x][y + 1] == GameAIOportunity::RANDOM_PLACE) {
					pointArray[pointsCount] = { x, y + 1 };
					pointsCount++;
				}
				// Left
				if (pointsCount < size && x != 0 && ptrState->aiPlayField[x - 1][y] == GameAIOportunity::RANDOM_PLACE) {
					pointArray[pointsCount] = { x - 1, y };
					pointsCount++;
				}
				// Right
				if (pointsCount < size && x != 9 && ptrState->aiPlayField[x + 1][y] == GameAIOportunity::RANDOM_PLACE) {
					pointArray[pointsCount] = { x + 1, y };
					pointsCount++;
				}
			}
		}
	}

	// Return count
	return pointsCount;
}

void GameAI::updateAiMap(GlobalState* ptrState) {
	// Comput shortest alive ship
	UINT shipMin = 5;
	for (unsigned int i = 0; i < 5; i++) {
		if (ptrState->playerShips[i].hp != 0 && ptrState->playerShips[i].length < shipMin) {
			shipMin = ptrState->playerShips[i].length;
		}
	}

	// Evaluate every place in field
	for (unsigned int x = 0; x < 10; x++) {
		for (unsigned int y = 0; y < 10; y++) {
			// Check if oportunity has lifted
			if (ptrState->aiPlayField[x][y] == GameAIOportunity::RANDOM_PLACE) {
				// Check if masking by current hit/nohit state
				if (ptrState->playfieldPlayer[x][y] != 0) {
					if (ptrState->playfieldPlayer[x][y] == 2) {
						ptrState->aiPlayField[x][y] = GameAIOportunity::HIT_SHIP_ALIVE;
						ptrState->aiPossibleHits--;
					}
					else {
						ptrState->aiPlayField[x][y] = GameAIOportunity::SHIP_IMPOSSIBLE;
						ptrState->aiPossibleHits--;
					}
				}
				// Check if masking by min ship
				else {
					if (
						((aiPointDistance(ptrState, x, y, +1, 0) + 1 + aiPointDistance(ptrState, x, y, -1, 0)) < shipMin) &&
						((aiPointDistance(ptrState, x, y, 0, +1) + 1 + aiPointDistance(ptrState, x, y, 0, -1)) < shipMin)
					) {
						ptrState->aiPlayField[x][y] = GameAIOportunity::SHIP_IMPOSSIBLE;
						ptrState->aiPossibleHits--;
					}
				}
			}
		}
	}

	// Check dead ships
	for (unsigned int i = 0; i < 5; i++) {
		if (ptrState->playerShips[i].hp == 0) {
			// Mask of Ship
			for (unsigned int o = 0; o < ptrState->playerShips[i].length; o++) {
				// Y-Increment
				if (ptrState->playerShips[i].isVertical) {
					if (ptrState->aiPlayField[ptrState->playerShips[i].posx][ptrState->playerShips[i].posy + o] == GameAIOportunity::RANDOM_PLACE) {
						ptrState->aiPossibleHits--;
					}
					ptrState->aiPlayField[ptrState->playerShips[i].posx][ptrState->playerShips[i].posy + o] = GameAIOportunity::SHIP_IMPOSSIBLE;
				}
				// X-Incrment
				else {
					if (ptrState->aiPlayField[ptrState->playerShips[i].posx + o][ptrState->playerShips[i].posy] == GameAIOportunity::RANDOM_PLACE) {
						ptrState->aiPossibleHits--;
					}
					ptrState->aiPlayField[ptrState->playerShips[i].posx + o][ptrState->playerShips[i].posy] = GameAIOportunity::SHIP_IMPOSSIBLE;
				}
			}
		}
		// Remask ship if it has helth
		else {
			// Remask ship
			for (unsigned int o = 0; o < ptrState->playerShips[i].length; o++) {
				// Y-Increment
				if (ptrState->playerShips[i].isVertical) {
					if (ptrState->aiPlayField[ptrState->playerShips[i].posx][ptrState->playerShips[i].posy + o] == GameAIOportunity::SHIP_IMPOSSIBLE &&
						ptrState->playfieldPlayer[ptrState->playerShips[i].posx][ptrState->playerShips[i].posy + o] == 0
					) {
						ptrState->aiPossibleHits++;
						ptrState->aiPlayField[ptrState->playerShips[i].posx][ptrState->playerShips[i].posy + o] = GameAIOportunity::RANDOM_PLACE;
					}
				}
				// X-Incrment
				else {
					if (ptrState->aiPlayField[ptrState->playerShips[i].posx + o][ptrState->playerShips[i].posy] == GameAIOportunity::SHIP_IMPOSSIBLE &&
						ptrState->playfieldPlayer[ptrState->playerShips[i].posx + o][ptrState->playerShips[i].posy] == 0
					) {
						ptrState->aiPossibleHits++;
						ptrState->aiPlayField[ptrState->playerShips[i].posx + o][ptrState->playerShips[i].posy] = GameAIOportunity::RANDOM_PLACE;
					}
				}
			}
		}
	}
}

unsigned int GameAI::aiPointDistance(GlobalState* ptrState, INT x, INT y, INT ix, INT iy) {
	int distance = -1;

	// Find random distance
	while (ptrState->aiPlayField[x][y] == GameAIOportunity::RANDOM_PLACE && x < 10 && x >= 0 && y < 10 && y >= 0) {
		x += ix;
		y += iy;	
		distance++;
	}

	// Return distance
	return distance;
}
