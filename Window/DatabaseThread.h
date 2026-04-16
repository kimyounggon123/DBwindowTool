#ifndef _DATABASETHREAD_H
#define _DATABASETHREAD_H


#include "DatabaseAccount.h"
#include <Windows.h>
#include "..\Utils\ThreadSafeQueue.h"
#include "..\Utils\Timer.h"
#include "resource.h"

struct CommPacket
{
	HWND hRequestWnd; 
	std::wstring query;
	std::vector<ColumnData> columns; // 컬럼명 저장
	std::vector<std::vector<CellData>> tableData; // 실제 데이터 저장

	my_ulonglong errNo;
	std::wstring errMsg;

	double ms;

	CommPacket(HWND wnd = NULL, const std::wstring query = L"") : hRequestWnd(wnd),
		query(query), errNo(0), errMsg(L""), ms(0.0)
	{}

	~CommPacket()
	{
		columns.clear();
		for (std::vector<CellData> row : tableData)
		{
			row.clear();
		}
		tableData.clear();
	}

	BOOL Post(UINT msg)
	{
		return PostMessageW(hRequestWnd, msg, 0, 0);
	}
};

using CommPacketPTR = std::unique_ptr<CommPacket>;

class DatabaseThread
{
	Timer timer;
	DatabaseAccount* account;

	ThreadSafeQueue<CommPacketPTR> AccountToWindow; // db thread -> 사용자
	ThreadSafeQueue<CommPacketPTR> WindowToAccount; // 사용자 -> db thread

	void Close();

	HANDLE hThread;
	unsigned int dwThreadID;
	bool runFlag;
	static unsigned int WINAPI Work(LPVOID lparam);

	HANDLE hMutex;
	bool WorkQueryProcess(CommPacketPTR& pk);
public:

	DatabaseThread();
	~DatabaseThread();

	bool Initialize();


	bool Enqueue(CommPacketPTR&& input)
	{
		return WindowToAccount.enqueue(std::move(input));
	}
	bool Dequeue(CommPacketPTR& output)
	{
		return AccountToWindow.dequeue(output);
	}

	void Quit() { runFlag = false; }


	bool IsLoggedIn() { return account->IsConnected(); }
	bool LogIn(const std::string id, const std::string pw);
	void LogOut();

	std::wstring GetDatabaseName() { return account->GetDatabaseName(); }


};

#endif