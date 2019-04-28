#pragma once
#include <string>
#include <windows.h>
#include "include/mysqlx/devapi/common.h"

namespace GameServer
{
	namespace Common
	{
		//�������ר��������utf-8�����char*�ַ���ת����ANSI�����Ա��ڿ���̨����ʾ(ֱ�ӸĿ���̨�ı��뷽ʽҲ��)
		inline std::string U2G(const char* utf8)
		{
			int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
			wchar_t* wstr = new wchar_t[len + 1];
			memset(wstr, 0, len + 1);
			MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);	//ͨ��UTF-16��ת��
			len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
			char* str = new char[len + 1];
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
			std::string ansi = std::move(str);
			if (wstr) delete[] wstr;
			if (str) delete[] str;
			return ansi;
		}
		//��ȡ��ǰʱ��
		inline std::string GetCurTime()
		{
			char tmp[64] = { 0 };
			time_t timep;
			time(&timep);
			strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
			return std::move(tmp);
		}
		//c++��xdevapiֱ�Ӷ����ݿ����������ʱ���벻��ȷ�����쳣����������������ǽ���Row���ȡ���ֽ����鿽����string��(�ֽڱ�����mysqlĬ�ϵ�utf8)
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