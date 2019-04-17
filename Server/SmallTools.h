#pragma once
#include <string>
#include <windows.h>

namespace GameServer
{
	namespace Util
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
		//��ȡʱ���
		inline std::string GetCurTime()
		{
			char tmp[64];
			time_t timep;
			time(&timep);
			strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
			return std::move(tmp);
		}
	}
}