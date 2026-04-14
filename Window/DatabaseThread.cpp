#include "DatabaseThread.h"
DatabaseThread::DatabaseThread() : account(nullptr),
hThread(NULL), dwThreadID(0), hMutex(NULL),
runFlag(true)
{
}
DatabaseThread::~DatabaseThread()
{
	Close();
}

void DatabaseThread::Close()
{
	CloseHandle(hThread);
	CloseHandle(hMutex);
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

		hMutex = CreateMutexW(NULL, FALSE, NULL);
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

	ThreadSafeQueue<CommPacketPTR>& WtA = This->WindowToAccount;
	ThreadSafeQueue<CommPacketPTR>& AtW = This->AccountToWindow;


	CommPacketPTR output;
	while (This->runFlag)
	{
		if (WtA.dequeue(output)) continue;
		This->WorkQueryProcess(output); // 작업 진행
		AtW.enqueue(std::move(output)); // 다시 사용자 window로
	}

	return 0;
}


bool DatabaseThread::WorkQueryProcess(CommPacketPTR& pk)
{
	CommPacket* pkPtr = pk.get();
	
	//// 여기서 락 거는 게 맞음
	WaitForSingleObject(hMutex, INFINITE);

	timer.Start();
	my_ulonglong queryResult = account->ExecuteQuery(pkPtr->query);
	if (queryResult == -1)
	{
		pkPtr->errMsg = account->GetLastErrorW();
		pkPtr->errNo = queryResult;
		return false;
	}

	// column 정보 저장
	MYSQL_FIELD* fields = account->getFieldName();
	unsigned int num_fields = account->getFieldNum();
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
	pkPtr->errNo = queryResult;

	return true;
}