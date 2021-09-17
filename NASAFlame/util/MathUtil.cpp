#include "MathUtil.h"

glm::vec3 MathUtil::getCameraPos(const glm::mat4 & viewModel)
{
	return glm::vec3(glm::inverse(viewModel) * glm::vec4(0.f, 0.f, 0.f, 1.f));
}
