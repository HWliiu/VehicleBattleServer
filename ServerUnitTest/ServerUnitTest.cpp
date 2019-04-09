#include "pch.h"
#include "CppUnitTest.h"
#include "../Server/DbUtil.h"	//²¢¸½¼ÓÒÀÀµÏî"../Server/x64/Debug/*.obj"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace GameServer;

namespace ServerUnitTest
{
	TEST_CLASS(ServerUnitTest)
	{
	public:
		
		TEST_METHOD(GetSeessionTestMethod)
		{
			auto session = Util::DbUtil::GetInstance()->GetSession();
			auto name = session.getDefaultSchema().getName();
			Assert::AreEqual(name.c_str(), L"vehicleBattle");
		}
	};
}
