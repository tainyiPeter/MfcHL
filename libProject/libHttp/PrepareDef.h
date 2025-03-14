#pragma once

#include "pch.h"
#include "framework.h"

#include <string>
#include <vector>
#include <cassert>
#include <memory>

#ifndef ASSERT
#define ASSERT	assert
#endif

#ifdef HTTPLIB_PROJECT
#define EXTERN_CLASS _declspec(dllexport)
#else
#define EXTERN_CLASS _declspec(dllimport)
#endif
