#ifndef _STDAFX_H_
#define _STDAFX_H_

#define APP_GUID "892fe160-9d0f-4e33-92e3-7ba4e7aaeecf"

#ifdef _UNICODE
	#define APP_NAME "TrillianG15 (unicode)"
	#define APP_SHORTNAME "TrillianG15"
#else
	#define APP_NAME	"TrillianG15"
	#define APP_SHORTNAME "TrillianG15"
#endif

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

using namespace std;

#define ASSERT assert

#include "LCDFramework.h"
#include "CEvent.h"

#include "plugin.h"
#include "Trillian.h"

#include "../resource.h"

// HID Includes
#include <setupapi.h>
extern "C"
{
#include "../../LCDFramework/hid/hidsdi.h"
}

#endif