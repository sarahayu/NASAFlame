#include "IcosphereGenerator.h"
//#include "IcosphereGenerator.h"
//

inline glm::vec3 findMidpoint(glm::vec3 vertex1, glm::vec3 vertex2) {
	return glm::normalize(glm::vec3((vertex1.x + vertex2.x) / 2.0f, (vertex1.y + vertex2.y) / 2.0f, (vertex1.z + vertex2.z) / 2.0f));
}
//
//void generateIcosphereMesh(size_t lod, std::vector<uint32_t>& indices, std::vector<glm::vec3>& positions) {
//	std::vector<uint32_t> newIndices;
//	newIndices.reserve(256);
//
//	std::unordered_map<glm::vec3, uint32_t, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;
//
//	indices.resize(NUM_ICOSOHEDRON_INDICES);
//	for (uint32_t i = 0; i < NUM_ICOSOHEDRON_INDICES; i++) {
//		indices[i] = ICOSOHEDRON_INDICES[i];
//	}
//	positions.resize(NUM_ICOSOHEDRON_VERTICES);
//	for (uint32_t i = 0; i < NUM_ICOSOHEDRON_VERTICES; i++) {
//		positions[i] = glm::normalize(ICOSOHEDRON_VERTICES[i]);
//		vertexLookup[glm::normalize(ICOSOHEDRON_VERTICES[i])] = i;
//	}
//
//	for (size_t i = 0; i < (size_t)lod; i++) {
//		for (size_t j = 0; j < indices.size(); j += 3) {
//			/*
//			j
//			mp12   mp13
//			j+1    mp23   j+2
//			*/
//			// Defined in counter clockwise order
//			glm::vec3 vertex1 = positions[indices[j + 0]];
//			glm::vec3 vertex2 = positions[indices[j + 1]];
//			glm::vec3 vertex3 = positions[indices[j + 2]];
//
//			glm::vec3 midPoint12 = findMidpoint(vertex1, vertex2);
//			glm::vec3 midPoint23 = findMidpoint(vertex2, vertex3);
//			glm::vec3 midPoint13 = findMidpoint(vertex1, vertex3);
//
//			uint32_t mp12Index;
//			uint32_t mp23Index;
//			uint32_t mp13Index;
//
//			auto iter = vertexLookup.find(midPoint12);
//			if (iter != vertexLookup.end()) { // It is in the map
//				mp12Index = iter->second;
//			}
//			else { // Not in the map
//				mp12Index = (uint32_t)positions.size();
//				positions.push_back(midPoint12);
//				vertexLookup[midPoint12] = mp12Index;
//			}
//
//			iter = vertexLookup.find(midPoint23);
//			if (iter != vertexLookup.end()) { // It is in the map
//				mp23Index = iter->second;
//			}
//			else { // Not in the map
//				mp23Index = (uint32_t)positions.size();
//				positions.push_back(midPoint23);
//				vertexLookup[midPoint23] = mp23Index;
//			}
//
//			iter = vertexLookup.find(midPoint13);
//			if (iter != vertexLookup.end()) { // It is in the map
//				mp13Index = iter->second;
//			}
//			else { // Not in the map
//				mp13Index = (uint32_t)positions.size();
//				positions.push_back(midPoint13);
//				vertexLookup[midPoint13] = mp13Index;
//			}
//
//			newIndices.push_back(indices[j]);
//			newIndices.push_back(mp12Index);
//			newIndices.push_back(mp13Index);
//
//			newIndices.push_back(mp12Index);
//			newIndices.push_back(indices[j + 1]);
//			newIndices.push_back(mp23Index);
//
//			newIndices.push_back(mp13Index);
//			newIndices.push_back(mp23Index);
//			newIndices.push_back(indices[j + 2]);
//
//			newIndices.push_back(mp12Index);
//			newIndices.push_back(mp23Index);
//			newIndices.push_back(mp13Index);
//		}
//		indices.swap(newIndices);
//		newIndices.clear();
//	}
//}
//
size_t Vec3KeyFuncs::operator()(const glm::vec3 & k) const {
	return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
}

bool Vec3KeyFuncs::operator()(const glm::vec3 & a, const glm::vec3 & b) const {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

ISphere generateIcosphereMesh(size_t lod)
{
	const static float GOLDEN_RATIO = 1.61803398875f;

	const static int NUM_ICOSOHEDRON_VERTICES = 12;
	const static glm::vec3 ICOSOHEDRON_VERTICES[12] = {
		glm::vec3(-1.0f, GOLDEN_RATIO, 0.0f),
		glm::vec3(1.0f, GOLDEN_RATIO, 0.0f),
		glm::vec3(-1.0f, -GOLDEN_RATIO, 0.0f),
		glm::vec3(1.0f, -GOLDEN_RATIO, 0.0f),

		glm::vec3(0.0f, -1.0f, GOLDEN_RATIO),
		glm::vec3(0.0f, 1.0f, GOLDEN_RATIO),
		glm::vec3(0.0f, -1.0, -GOLDEN_RATIO),
		glm::vec3(0.0f, 1.0f, -GOLDEN_RATIO),

		glm::vec3(GOLDEN_RATIO, 0.0f, -1.0f),
		glm::vec3(GOLDEN_RATIO, 0.0f, 1.0f),
		glm::vec3(-GOLDEN_RATIO, 0.0f, -1.0f),
		glm::vec3(-GOLDEN_RATIO, 0.0, 1.0f)
	};

	const static int NUM_ICOSOHEDRON_INDICES = 60;
	const static uint32_t ICOSOHEDRON_INDICES[60] = {
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};

	ISphere::IndArray indices, newIndices;
	ISphere::PosArray positions;

	newIndices.reserve(256);

	std::unordered_map<glm::vec3, uint32_t, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;

	indices.resize(NUM_ICOSOHEDRON_INDICES);
	for (uint32_t i = 0; i < NUM_ICOSOHEDRON_INDICES; i++) {
		indices[i] = ICOSOHEDRON_INDICES[i];
	}
	positions.resize(NUM_ICOSOHEDRON_VERTICES);
	for (uint32_t i = 0; i < NUM_ICOSOHEDRON_VERTICES; i++) {
		positions[i] = glm::normalize(ICOSOHEDRON_VERTICES[i]);
		vertexLookup[glm::normalize(ICOSOHEDRON_VERTICES[i])] = i;
	}

	for (size_t i = 0; i < (size_t)lod; i++) {
		for (size_t j = 0; j < indices.size(); j += 3) {
			/*
			j
			mp12   mp13
			j+1    mp23   j+2
			*/
			// Defined in counter clockwise order
			glm::vec3 vertex1 = positions[indices[j + 0]];
			glm::vec3 vertex2 = positions[indices[j + 1]];
			glm::vec3 vertex3 = positions[indices[j + 2]];

			glm::vec3 midPoint12 = findMidpoint(vertex1, vertex2);
			glm::vec3 midPoint23 = findMidpoint(vertex2, vertex3);
			glm::vec3 midPoint13 = findMidpoint(vertex1, vertex3);

			uint32_t mp12Index;
			uint32_t mp23Index;
			uint32_t mp13Index;

			auto iter = vertexLookup.find(midPoint12);
			if (iter != vertexLookup.end()) { // It is in the map
				mp12Index = iter->second;
			}
			else { // Not in the map
				mp12Index = (uint32_t)positions.size();
				positions.push_back(midPoint12);
				vertexLookup[midPoint12] = mp12Index;
			}

			iter = vertexLookup.find(midPoint23);
			if (iter != vertexLookup.end()) { // It is in the map
				mp23Index = iter->second;
			}
			else { // Not in the map
				mp23Index = (uint32_t)positions.size();
				positions.push_back(midPoint23);
				vertexLookup[midPoint23] = mp23Index;
			}

			iter = vertexLookup.find(midPoint13);
			if (iter != vertexLookup.end()) { // It is in the map
				mp13Index = iter->second;
			}
			else { // Not in the map
				mp13Index = (uint32_t)positions.size();
				positions.push_back(midPoint13);
				vertexLookup[midPoint13] = mp13Index;
			}

			newIndices.push_back(indices[j]);
			newIndices.push_back(mp12Index);
			newIndices.push_back(mp13Index);

			newIndices.push_back(mp12Index);
			newIndices.push_back(indices[j + 1]);
			newIndices.push_back(mp23Index);

			newIndices.push_back(mp13Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(indices[j + 2]);

			newIndices.push_back(mp12Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(mp13Index);
		}
		indices.swap(newIndices);
		newIndices.clear();
	}

	return{ indices, positions };
}
