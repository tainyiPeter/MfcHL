#ifndef DISPLAY_INFO_FROM_DX_H
#define DISPLAY_INFO_FROM_DX_H 
#include <atlstr.h>
#include <string>

void ForEachGraphicCard(std::function<bool(const wchar_t * displayName, const wchar_t * cardInfo, const wchar_t * LowPart, const wchar_t * HighPart) > func);

#endif
