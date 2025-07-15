#pragma once

#include<iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <span>
#include <memory>
#include <functional>
#include <concepts>
#include <format>
#include <chrono>
#include <numeric>
#include <numbers>


#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include<glm.hpp>
#include<gtc/matrix_transform.hpp>

#include"stb_image.h"

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#pragma comment (lib,"vulkan-1.lib")
#endif
#include <vulkan/vulkan.h>