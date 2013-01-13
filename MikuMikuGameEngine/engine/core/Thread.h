#pragma once

class CriticalSection
{
public:
	CRITICAL_SECTION m_cs;

public:
	CriticalSection();
	virtual ~CriticalSection();

public:
	void Enter();
	void Leave();
};

class Locker
{
private:
	CriticalSection* m_cs;
public:
	Locker( CriticalSection* cs );
	virtual ~Locker();
};

class IThread
{
private:
	static unsigned _stdcall ThreadCall(void* p);

private:
	HANDLE m_handle;
	DWORD m_dwID;
	HANDLE m_beginEvent;

public:
	IThread();
	virtual ~IThread();

public:
	bool Begin();
	void WaitEnd();

protected:
	void BeginEvent();

public:
	bool IsRun();

public:
	virtual unsigned int Run() = 0;
};
