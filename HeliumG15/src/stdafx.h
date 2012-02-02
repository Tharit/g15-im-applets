#ifndef _STDAFX_H_
#define _STDAFX_H_

/* Common header files */
#include <assert.h>

#define _WIN32_WINNT 0x0500			// Needed for waitable timers
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <time.h>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <math.h>
using namespace std;


//#define IDF_ISIDLE		0x1 // idle has become active (if not set, inactive)
#define IDF_SHORT		0x2 // short idle mode
#define IDF_LONG		0x4 // long idle mode
//#define IDF_PRIVACY		0x8 // if set, the information provided shouldn't be given to third parties.
#define IDF_ONFORCE	   0x10

#define ASSERT assert

#include "LCDFramework.h"

#include "api.h"
#include "CTimeSys.h"

#include "../resource.h"

#endif