#include "DatabaseThread.h"
DatabaseThread::DatabaseThread() : account(nullptr),
hThread(NULL), dwThreadID(0), hEvent(NULL),
runFlag(true),
AccountToWindow(0), WindowToAccount(0)
{}

DatabaseThread::~DatabaseThread()
{
	Quit();
	Close();
}

void DatabaseThread::Close()
{
	CloseHandle(hThread);
	CloseHandle(hEvent);
	SAFE_FREE(account);
}


bool DatabaseThread::Initialize()
{
	bool result = true;
	try
	{
		account = new DatabaseAccount();
		if (account == nullptr)	throw false;

		hThread = (HANDLE)_beginthreadex(
			NULL, 0,
			Work, this,
			0, &dwThreadID
		);
		if (hThread == NULL) throw false;

		hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	}
	catch (bool ret)
	{
		result = ret;
	}
	return result;
}

unsigned int WINAPI DatabaseThread::Work(LPVOID lparam)
{
	DatabaseThread* This = reinterpret_cast<DatabaseThread*>(lparam);

	ThreadSafeQueue<ConnPacketPTR>& WtA = This->WindowToAccount;
	ThreadSafeQueue<ConnPacketPTR>& AtW = This->AccountToWindow;


	ConnPacketPTR output;
	while (This->runFlag)
	{
		WaitForSingleObject(This->hEvent, INFINITE);

		if (!WtA.dequeue(output))
		{
			ResetEvent(This->hEvent); continue;
		}

		This->WorkQueryProcess(output); // 작업 진행

		HWND& handle = output->hRequestWnd;
		AtW.enqueue(std::move(output)); // 다시 사용자 window로
		PostMessageW(handle, WM_POST_QUERY_RESULT, 0, 0);
		if (AtW.isEmpty()) ResetEvent(This->hEvent);
	}

	return 0;
}

bool DatabaseThread::WorkQueryProcess(ConnPacketPTR& pk)
{
	ConnPacket* pkPtr = pk.get();
	bool ret = true;
	try
	{
		timer.Start();

		my_ulonglong queryResult = account->ExecuteQuery(pkPtr->query);
		if (queryResult == -1) // 쿼리 실패 시
		{
			pkPtr->errMsg = account->GetLastErrorW();
			pkPtr->numOfRows = queryResult;
			throw false;
		}
		MYSQL_FIELD* fields = account->getFieldName();
		unsigned int num_fields = account->getFieldNum();
		
		// column 정보 저장
		for (unsigned int i = 0; i < num_fields; ++i) 
		{
			ColumnType type = ColumnType::Normal;
			if (fields[i].flags & PRI_KEY_FLAG) type = ColumnType::PK;
			else if (fields[i].flags & MULTIPLE_KEY_FLAG) type = ColumnType::FK;
			pkPtr->columns.push_back({ type, UTF8ToWString(fields[i].name) });
		}


		// row 정보 저장
		MYSQL_ROW row = nullptr;
		int count = 0;
		while ((row = account->fetchRow()) && count < account->GetMaxRow())
		{
			std::vector<CellData> oneRow;

			// 각 필드는 문자열(char*) 형태로 반환됨
			for (unsigned int i = 0; i < num_fields; ++i)  // 각 row의 field를 출력
			{
				bool isRealNULL = row[i] ? false : true;
				std::wstring value = isRealNULL ? L"NULL" : UTF8ToWString(row[i]);

				CellData cell{ fields[i].type, value, isRealNULL };
				oneRow.push_back(cell);
			}
			pkPtr->tableData.push_back(oneRow);
			count++;
		}

		timer.End();
		double ms = timer.GetDuration();

		// 정산
		pkPtr->ms = ms;
		pkPtr->numOfRows = queryResult;
	}
	catch (bool result)
	{
		ret = result;
	}

	return ret;
}


bool DatabaseThread::LogIn(const std::string id, const std::string pw)
{
	if (account == nullptr) return false;

	bool result = account->Connect("localhost", id.c_str(), pw.c_str(), nullptr);
	if (!result)
	{
		account->Close();
		return false;
	}

	return true;
}
void DatabaseThread::LogOut()
{
	if (!account->IsConnected()) return;
	account->Close();
}