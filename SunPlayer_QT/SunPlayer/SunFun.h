#ifndef SUNFUN_H
#define SUNFUN_H
#include <Windows.h>
#include <string>

std::string GetAppPathA();

std::wstring GetAppPathW();

std::wstring CHAR2wstring(LPCSTR lpcszSrc);

std::string WCHAR2string(LPCWSTR lpwszSrc);

std::wstring UTF8ToWString(const char *lpcszString);

std::string WStringToUTF8(const wchar_t *lpwcszWString);

std::string GBKToUTF8(const std::string &strGBK);

std::string UTF8ToGBK(const std::string &strUTF8);

std::string FormatString(const char *lpcszFormat, ...);

std::wstring FormatWstring(const wchar_t *lpcwszFormat, ...);

std::string &replace(std::string &str, const std::string &old_value, const std::string &new_value);

#endif