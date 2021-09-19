/// Seed of Andromeda Icosphere Generator
/// Written by Frank McCoy
/// Use it for whatever, but remember where you got it from.

// modified by Sarah Yuniar

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
// GLM is awesome ^ ^ ^

// Hash functions for the unordered map
class Vec3KeyFuncs 
{
public:
	size_t operator()(const glm::vec3& k)const;

	bool operator()(const glm::vec3& a, const glm::vec3& b)const;
};

inline glm::vec3 findMidpoint(glm::vec3 vertex1, glm::vec3 vertex2);

struct ISphere
{
	typedef std::vector<uint32_t> IndArray;
	typedef std::vector<glm::vec3> PosArray;

	IndArray indices;
	PosArray positions;
};

/// Generates an icosphere with radius 1.0f.
/// @param lod: Number of subdivisions
/// @return: ISphere struct with indices and positions
ISphere generateIcosphereMesh(size_t lod);