#include "pch.h"
#include "CppUnitTest.h"
#include "../Server/DbUtil.h"	//²¢¸½¼ÓÒÀÀµÏî"../Server/x64/Debug/*.obj"
#include "../Server/LoginHandle.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace GameServer::Common;
using namespace GameServer::Handle;

namespace ServerUnitTest
{
	TEST_CLASS(ServerUnitTest)
	{
	public:
		TEST_METHOD(GetSeessionTestMethod)
		{
			auto session = DBUtil::GetInstance()->GetSession();
			auto name = session.getDefaultSchema().getName();
			Assert::AreEqual(name.c_str(), L"vehicleBattle");
		}
		TEST_METHOD(RegisterTestMethod)
		{
			AccountHandle loginHandle;
			loginHandle.Register("test", "123123", 0, [](std::string s) {});
		}
	};
}
