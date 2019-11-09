// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

// Windows Header Files
#include <windows.h>
#include <CommCtrl.h>

// C RunTime Header Files
#include <algorithm>
#include <codecvt>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <malloc.h>
#include <tchar.h>

#include <Strsafe.h> 
