#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#endif
