#include "utilityFunctions.h"
#include <Colour.h>
#include <ModelTriangle.h>
#include <Utils.h>
#include <DrawingWindow.h>

#include <map>
#include <vector>
#include <glm/glm.hpp>

#include "Renderer.h"


std::map<std::string, Colour> utilityFunctions::readMatFile(std::string filename, std::map<std::string,Colour> &colourMap) {
	std::ifstream file(filename); // read file
	std::string currentLine; // string to store each line
	std::string nameOfColour; // key Colour

	bool isLineRGB = false;

	while (getline (file, currentLine)) {

		std::vector<std::string> tokens = split(currentLine, ' '); // gets the first character after space of each line.

		if (isLineRGB) {
			Colour colour;
			// --> first number red , second number green, third number blue
			colour.red = std::round(std::stof(tokens[1]) *255.0f); // indexing starts at 1.
			colour.green = std::round(std::stof(tokens[2])*255.0f); // indexing starts at 2.
			colour.blue = std::round(std::stof(tokens[3])*255.0f); // indexing starts at 3.

			isLineRGB = false;
			colourMap.emplace(nameOfColour,colour);
		}
		// is a new colour being declared?
		if (tokens[0] == "newmtl") {
			// create new colour
			nameOfColour = tokens[1];
			isLineRGB = true;
		}
	}
	return colourMap;
}

std::vector<ModelTriangle> utilityFunctions::readObjFile(std::string filename, std::vector<ModelTriangle>& savedTriangle, float scaleFactor, std::map<std::string,Colour>& colourMap) {

	std::ifstream file(filename); // read file
	std::string currentLine; // string to store each line
	std::vector<glm::vec3> verticesList; // stores verticies
	std::vector<float> vertexList;
	Colour currentColour = Colour(255,255,255);
	std::map<std::vector<int>, Colour> faces;

	while (getline (file, currentLine)) {

		std::vector<std::string> tokens = split(currentLine, ' '); // gets the first character after space of each line
		if (tokens[0] == "usemtl") {
			// set the current colour being processed from the colour table
			currentColour = colourMap.find(tokens[1])->second;
		}

		if (currentLine[0] == 'v') {
			glm::vec3 vertices;

			std::vector<std::string> lineTokens = split(currentLine, ' ');

			for (int i = 1; i <= lineTokens.size(); i++) { // should always be 3.
				if (lineTokens[i] == "v") continue;  // avoid first letter
				// stof --> string to float.
				// scale factor --> avoids bunnyzilla

				if (i == 1) vertices.x = std::stof(lineTokens[i]) * scaleFactor;
				if (i == 2) vertices.y = std::stof(lineTokens[i]) * scaleFactor;
				if (i == 3) vertices.z = std::stof(lineTokens[i]) * scaleFactor;
			}


			verticesList.push_back(vertices);
		} else if (currentLine[0] == 'f') {
			std::vector<int> newFace;
			std::vector<std::string> lineTokens = split(currentLine, ' ');

			// tokens are formatted like '2/'; make sure we only get the number
			for (const auto& token : lineTokens) {
				if (token == "f") continue;  // avoid first letter
				// must split tokens again to get rid of //
				std::vector<std::string> splitToken = split(token, '/');
				if (!splitToken.empty() && !splitToken[0].empty()) {
					int vertexNumber = std::stoi(splitToken[0]);
					//vertices in OBJ are indexed from 1 --> hence when fetching the vertices from vertex list, subtract one from the index stored in the face list.
					newFace.push_back(vertexNumber-1); // push the index of the side to the vectorList
				}
			}
			faces.insert({newFace, currentColour});
		}
	}
	// go through all faces and create model triangles based on vertex information -- AFTER reading thru file
	for (const auto& [face, colour] : faces) {

		ModelTriangle modelTriangle;
		int counter = 0; // adds vertices to model triangle
		// iterate through all vertices in a face and find in vertex list
		for (const auto& vertexIndex : face) {
			modelTriangle.vertices[counter] = verticesList[vertexIndex];
			counter++;
		}
		modelTriangle.colour = colour;

		savedTriangle.push_back(modelTriangle);
	}


	return savedTriangle;
}

void utilityFunctions::loadModel(std::string objFile, std::string matFile, std::vector<ModelTriangle> &triangles, std::map<std::string,Colour> &colourMap, float scaleFactor) {
	colourMap = readMatFile(matFile, colourMap);
	triangles = readObjFile(objFile, triangles, scaleFactor, colourMap);
}


std::vector<float> utilityFunctions::interpolateSingleFloats(float from, float to, int numberOfValues) {

	float difference = to-from; // find the difference between start and end/
	int intervals = numberOfValues -1;  // find the number of intervals
	float cDifference = difference/float(intervals); // find common difference between vector values.

	if (intervals == 0 ) {cDifference = 1.0;}

	std::vector <float> result;
	for (int i = 0; i < numberOfValues; i++) {
		result.push_back(from + cDifference*float(i)); // start + c.difference gives colour at different pixel
	}

	return result;
}

std::vector<glm::vec3> utilityFunctions::interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {

	glm::vec3 difference = to-from;
	glm::vec3 intervals((numberOfValues-1.0),(numberOfValues-1.0),(numberOfValues-1.0));
	glm::vec3 cDifference = difference/intervals;

	std::vector <glm::vec3> result;

	for (int i = 0; i < numberOfValues; i++) {
		glm::vec3 p = glm::vec3(i,i,i); // can only multiply by a vec3, hence make a vector p
		result.push_back(from + cDifference*p);
	}
	return result;
}

uint32_t utilityFunctions::convertColourToUint32(const Colour& colour) {
	uint32_t uIntColour = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue; // UNSIGNED INTEGER (THEREFORE, NEGATIVE INTEGER)
	return uIntColour;
}

glm::vec3 utilityFunctions::translateCameraPosition(glm::vec3 cameraPosition, glm::vec3 translationVector) {
    if (glm::normalize(translationVector) == glm::vec3(0.0,0.0,0.0)) return cameraPosition;

	cameraPosition = cameraPosition + translationVector;
	return cameraPosition;
}

// get's cos theta between camera and a scene vector.
float utilityFunctions::getCosAngle(glm::vec3 cameraPosition, glm::vec3 sceneVector) {
    float dot = glm::dot(cameraPosition, sceneVector);
	float magCamera = length(cameraPosition);
	float magScene = length(sceneVector);


	float angle = dot/(magCamera*magScene);
	return angle;
}

float utilityFunctions::getSinAngle(glm::vec3 cameraPosition, glm::vec3 sceneVector) {
	glm::vec3 crossVec = glm::cross(cameraPosition, sceneVector);

	float cross = glm::length(crossVec);

	float magCamera = length(cameraPosition);
	float magScene = length(sceneVector);

	float angle = cross/(magCamera*magScene);


	return angle;
}

glm::mat3 utilityFunctions::tiltedMatrix(float cos, float sin, glm::mat3 cameraOrientation) {
	glm::vec3 col1(1.0, 0.0, 0.0);
	glm::vec3 col2(0.0, cos, sin);
	glm::vec3 col3(0.0, -sin, cos);
	glm::mat3 rotationMatrix = glm::mat3(col1, col2, col3);

	glm::mat3 newCameraOrientation = rotationMatrix*cameraOrientation;
	return newCameraOrientation;
}

glm::mat3 utilityFunctions::pannedMatrix(float cos, float sin, glm::mat3 cameraOrientation) {
	glm::vec3 col1(cos, 0.0, -sin);
	glm::vec3 col2(0.0, 1.0, 0.0);
	glm::vec3 col3(sin, 0.0, cos);
	glm::mat3 rotationMatrix = glm::mat3(col1, col2, col3);

	glm::mat3 newCameraOrientation = rotationMatrix*cameraOrientation;
	return newCameraOrientation;
}

glm::mat3 utilityFunctions::rolledMatrix(float cos, float sin, glm::mat3 cameraOrientation) {
	glm::vec3 col1(cos, sin, 0.0);
	glm::vec3 col2(-sin, cos, 0.0);
	glm::vec3 col3(0.0, 0.0, 1.0);
	glm::mat3 rotationMatrix = glm::mat3(col1, col2, col3);
	return rotationMatrix;
}

glm::mat3 utilityFunctions::tiltMatrix(float cos, float sin) {
	glm::vec3 col1(1.0, 0.0, 0.0);
	glm::vec3 col2(0.0, cos, sin);
	glm::vec3 col3(0.0, -sin, cos);
	glm::mat3 rotationMatrix = glm::mat3(col1, col2, col3);
	return rotationMatrix;
}

	glm::mat3 newCameraOrientation = rotationMatrix*cameraOrientation;
	return newCameraOrientation;
}

glm::vec3 utilityFunctions::orbit(glm::vec3 cameraPosition, glm::mat3 rotationMatrix){
	cameraPosition = cameraPosition*rotationMatrix;
	return cameraPosition;
}

glm::mat3 utilityFunctions::lookAt(glm::vec3 cameraPosition,glm::vec3 centreOfScene, glm::mat3 cameraOrientation) {

	glm::vec3 forwardVector = centreOfScene - cameraPosition;
		// centre (0,0,0) , camera (0,0,4) , direction (0,0,-4)
	forwardVector = glm::normalize(forwardVector);
	glm::vec3 vertical(0.0,1.0,0.0);
	glm::vec3 rightVector =  glm::cross(forwardVector,vertical); // slides have these the other way
	glm::vec3 upVector = glm::cross(rightVector,forwardVector); // slide had this the other way.

	cameraOrientation = glm::mat3(rightVector, upVector, forwardVector);
	return cameraOrientation;
}

bool utilityFunctions::triangleOffScreen(CanvasPoint p1, CanvasPoint p2, CanvasPoint p3) {
	bool triangleOffScreen = false;
	if (p1.x < 0 && p2.x < 0 && p3.x < 0) triangleOffScreen = true;
	if (p1.x > WIDTH && p2.x >WIDTH && p3.x >WIDTH) triangleOffScreen = true;
	if (p1.y < 0 && p2.y < 0 && p3.y < 0) triangleOffScreen = true;
	if (p1.y >HEIGHT && p2.y >HEIGHT&& p3.y >HEIGHT) triangleOffScreen = true;
	return triangleOffScreen;
}

std::vector<float> utilityFunctions::clearDepthBuffer(std::vector<float> depthBuffer) {
	for (float & i : depthBuffer) {
		i = 0.0;
	}
	return depthBuffer;
}