#ifndef _DATABASETHREAD_H
#define _DATABASETHREAD_H


#include "DatabaseAccount.h"
#include <Windows.h>
#include "..\Utils\ThreadSafeQueue.h"

struct CommPacket
{
	std::string data;
};
using CommPacketPTR = std::unique_ptr<CommPacket>;
class DatabaseThread
{
	DatabaseAccount* account;

	ThreadSafeQueue<CommPacketPTR> AccountToWindow;
	ThreadSafeQueue<CommPacketPTR> WindowToAccount;

	void Close()
	{
		SAFE_FREE(account);
	}

	static unsigned int WINAPI Work(LPVOID lparam);
public:
	DatabaseThread(): account(nullptr)
	{}
	~DatabaseThread()
	{
		Close();
	}

	bool Initialize();


	bool Enqueue(CommPacketPTR&& input)
	{
		return WindowToAccount.enqueue(std::move(input));
	}
	bool Dequeue(CommPacketPTR& output)
	{
		return AccountToWindow.dequeue(output);
	}
};

#endif