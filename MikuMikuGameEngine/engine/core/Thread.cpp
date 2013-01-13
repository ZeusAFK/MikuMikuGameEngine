#include "stdafx.h"

#include "Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CriticalSection::CriticalSection()
{
	InitializeCriticalSection( &m_cs );
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection( &m_cs );
}

void CriticalSection::Enter()
{
	EnterCriticalSection( &m_cs );
}

void CriticalSection::Leave()
{
	LeaveCriticalSection( &m_cs );
}

Locker::Locker( CriticalSection* cs )
	: m_cs(cs)
{
	if( m_cs )
	{
		m_cs->Enter();
	}
}

Locker::~Locker()
{
	if( m_cs )
	{
		m_cs->Leave();
	}
}

unsigned IThread::ThreadCall(void* p)
{
	IThread* thread = (IThread*)p;

	thread->BeginEvent();

	unsigned int ret=thread->Run();

	_endthreadex( ret );
	return ret;
}

IThread::IThread()
	: m_handle(NULL)
	, m_dwID(0)
	, m_beginEvent(NULL)
{
}

IThread::~IThread()
{
	WaitEnd();
}

bool IThread::Begin()
{
	m_beginEvent = ::CreateEvent( NULL,TRUE,FALSE,NULL );
	if( m_beginEvent==NULL )
	{
		return false;
	}

	m_handle = (HANDLE)_beginthreadex( NULL,0,IThread::ThreadCall,this,0,(unsigned int*)&m_dwID );
	if( m_handle==0 )
	{
		return false;
	}

	::WaitForSingleObject( m_beginEvent,INFINITE );
	::CloseHandle( m_beginEvent );
	m_beginEvent = NULL;

	return true;
}

void IThread::BeginEvent()
{
	::SetEvent( m_beginEvent );
}

void IThread::WaitEnd()
{
	if( m_handle && GetCurrentThreadId()!=m_dwID )
	{
		::WaitForSingleObject( m_handle, INFINITE );
		::CloseHandle( m_handle );
		m_handle = NULL;
		m_dwID = 0;
	}
}