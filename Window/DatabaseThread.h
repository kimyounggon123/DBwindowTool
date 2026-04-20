#ifndef _DATABASETHREAD_H
#define _DATABASETHREAD_H


#include "DatabaseAccount.h"
#include <Windows.h>
#include "..\Utils\ThreadSafeQueue.h"
#include "..\Utils\Timer.h"
#include "resource.h"

enum class PacketCode
{
	Normal = 0,
	UserTyping,
	LogIn,
	LogOut,
	RefreshTree,
	Transaction
};

struct ConnPacket
{
	HWND hRequestWnd;
	PacketCode head;
	std::wstring query;
	std::vector<ColumnData> columns; // 컬럼명 저장
	std::vector<std::vector<CellData>> tableData; // 실제 데이터 저장

	double ms;

	my_ulonglong numOfRows;
	std::wstring errMsg;

	ConnPacket(HWND hwnd, const PacketCode& head = PacketCode::Normal, const std::wstring query = L"") : hRequestWnd(hwnd), head(head),
		query(query),  ms(0.0), numOfRows(0), errMsg(L"")
	{}

	~ConnPacket()
	{
		columns.clear();
		tableData.clear();
	}

};
using ConnPacketPTR = std::unique_ptr<ConnPacket>;

class DatabaseThread
{
	Timer timer;
	DatabaseAccount* account;

	ThreadSafeQueue<ConnPacketPTR> AccountToWindow; // db thread -> 사용자
	ThreadSafeQueue<ConnPacketPTR> WindowToAccount; // 사용자 -> db thread

	void Close();

	HANDLE hThread;
	unsigned int dwThreadID;
	bool runFlag;
	static unsigned int WINAPI Work(LPVOID lparam);

	HANDLE hEvent;
	bool WorkQueryProcess(ConnPacketPTR& pk); // 일반 쿼리문 
public:

	DatabaseThread();
	~DatabaseThread();

	bool Initialize();


	bool Enqueue(ConnPacketPTR&& input)
	{
		if (!WindowToAccount.enqueue(std::move(input))) return false;
		SetEvent(hEvent);
		return true;
	}
	bool Dequeue(ConnPacketPTR& output)
	{
		return AccountToWindow.dequeue(output);
	}
	bool HasResultPacket() { return !AccountToWindow.isEmpty(); }

	void Quit() { runFlag = false; }


	bool IsConnected() { return account->IsConnected(); }
	bool LogIn(const std::string id, const std::string pw);
	void LogOut();

	std::wstring GetDatabaseName() { return account->GetDatabaseName(); }



	bool IsDirty() const { return account->IsDirty(); }
	bool StartTransaction() const { return account->StartTransaction(); }
	bool Rollback() const { return account->Rollback(); }
	bool Commit() const { return account->Commit(); }

	std::wstring GetLastErrorW() { return account->GetLastErrorW(); }
};

#endif