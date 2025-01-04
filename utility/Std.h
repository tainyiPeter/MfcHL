#ifndef __std_H__
#define __std_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>
#include <deque>
#include <vector>
#include <string>
#include <set>
#include <functional>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <thread>

using sys_clock = std::chrono::system_clock;
using time_point_t = sys_clock::time_point;

using StringList = std::vector<std::string>;

typedef std::string     astring;
typedef std::wstring    wstring;

#ifdef _UNICODE
//typedef wstring         tstring;
#else
typedef astring         tstring;
#endif // _UNICODE

#endif /*__std_H__*/