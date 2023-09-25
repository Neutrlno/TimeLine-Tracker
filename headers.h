#pragma once

// SFML headers
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// ImGUI headers
#include <imgui.h>
#include <imguidatechooser.h>

// ImGUI-SFML link header
#include <imgui-SFML.h>
#define IMGUI_USER_CONFIG <imconfig-SFML.h>

// ImGUI-FreeType link header
#define IMGUI_ENABLE_FREETYPE
#include <imgui_freetype.h>

// Standard includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <time.h>
#include <chrono>
#include <ctime>
#include <regex>
#include <cmath>
#include <stdexcept>
#include <string>
#include <array>

using namespace std;
using namespace chrono;

// Resources include(to compile exe with the icon and the font)
#include "resource.h"
#include "robotom-arial"
#include "ImGui_Style.h"
