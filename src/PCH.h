#pragma once

#include "SFSE/SFSE.h"
#include "RE/Starfield.h"

#include <xbyak/xbyak.h>

#ifdef NDEBUG
#include <spdlog/sinks/basic_file_sink.h>
#else
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace logger = SFSE::log;
namespace stl = SFSE::stl;
using namespace std::literals;

#define DLLEXPORT extern "C" [[maybe_unused]] __declspec(dllexport)

#include "Version.h"
