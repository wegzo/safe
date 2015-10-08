#pragma once
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _WTL_USE_CSTRING

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include "resource.h"
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atlmisc.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlpath.h>