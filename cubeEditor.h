#pragma once

#include "cube.h"
#include "imgui.h"
#include "transform.h"
#include "arrow.h"
#include <vector>
#include "math.h"

class CubeEditor {
public:
	CubeEditor();
	void setupOutline();
	void update();
	void render();

	Transform transform;
	Arrow rightArrow, upArrow, forwardArrow;
	Arrow leftArrow, downArrow, backArrow;
	// Cube* cube;
	Cube* cubeClickedOn;
	// Cube* prevFrameCubeSelection;
	Arrow* arrows[6];
	float disableSelectionTime;

	std::vector<Cube*> selectedCubes;

private:
	void addCube(Cube* cubePtr);
	float arrowPosOffset = 0.5f;
	void moveCubes(glm::vec3 offset);
};
