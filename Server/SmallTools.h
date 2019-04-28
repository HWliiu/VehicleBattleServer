#pragma once
#include <string>
#include <windows.h>
#include "include/mysqlx/devapi/common.h"

namespace GameServer
{
	namespace Common
	{
		//这个函数专门用来将utf-8编码的char*字符串转换成ANSI编码以便在控制台中显示(直接改控制台的编码方式也行)
		inline std::string U2G(const char* utf8)
		{
			int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
			wchar_t* wstr = new wchar_t[len + 1];
			memset(wstr, 0, len + 1);
			MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);	//通过UTF-16来转换
			len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
			char* str = new char[len + 1];
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
			std::string ansi = std::move(str);
			if (wstr) delete[] wstr;
			if (str) delete[] str;
			return ansi;
		}
		//获取当前时间
		inline std::string GetCurTime()
		{
			char tmp[64] = { 0 };
			time_t timep;
			time(&timep);
			strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
			return std::move(tmp);
		}
		//c++的xdevapi直接读数据库的中文数据时编码不正确会抛异常，这个函数的作用是将从Row里获取的字节数组拷贝到string里(字节编码是mysql默认的utf8)
		inline std::string MysqlxBytes2String(mysqlx::bytes mysqlxBytes)
		{
			//char* str = new char[mysqlxBytes.length()];
			char str[256] = { 0 };
			memcpy(str, mysqlxBytes.begin(), mysqlxBytes.length());
			str[mysqlxBytes.length()] = 0;
			std::string utf8 = std::move(str);
			//if (str) delete[] str;
			return utf8;
		}

		inline std::string GenerateGuid()
		{
			GUID guid;
			CoCreateGuid(&guid);
			char cBuffer[64] = { 0 };
			sprintf_s(cBuffer, sizeof(cBuffer),
				"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				guid.Data1, guid.Data2,
				guid.Data3, guid.Data4[0],
				guid.Data4[1], guid.Data4[2],
				guid.Data4[3], guid.Data4[4],
				guid.Data4[5], guid.Data4[6],
				guid.Data4[7]);
			return std::string(cBuffer);
		}
	}
}