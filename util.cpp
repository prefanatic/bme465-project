#include "image_processor.hpp"
#include <cmath>

int clamp(int val, int min, int max) {
	return std::max(min, std::min(val, max));
}