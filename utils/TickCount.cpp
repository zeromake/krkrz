//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// safe 64bit System Tick Count
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "tjsUtils.h"
#include "TickCount.h"
#include "SysInitIntf.h"
#include "ThreadIntf.h"

#ifdef TVP_COMPILING_KRKRSDL2
#include <SDL.h>
#else
#ifdef _WIN32
#include <mmsystem.h>
#else
#include <time.h>
#endif
#endif

#if 0
// システムに依存しない実装ではあるが、乱数の偏り等懸念される
// 環境ごとにシステム軌道からのtickを取得することにする
#include <chrono>
//---------------------------------------------------------------------------
class tTVPTickCounter {
	std::chrono::time_point<std::chrono::system_clock> start_;
public:
	tTVPTickCounter() : start_(std::chrono::system_clock::now()) {}
	tjs_uint32 Count() const {
		auto current = std::chrono::system_clock::now();
		auto duration = current - start_;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		return static_cast<tjs_uint32>(msec);
	}
} static TVPTickCounter;
//---------------------------------------------------------------------------
#endif

#ifndef TVP_COMPILING_KRKRSDL2
//---------------------------------------------------------------------------
// 64bit may enough to hold usual time count.
// ( 32bit is clearly insufficient )
//---------------------------------------------------------------------------
static tjs_uint64 TVPTickCountBias = 0;
static tjs_uint TVPWatchLastTick;
static tTJSCriticalSection TVPTickWatchCS;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
// TVPGetRoughTickCount
// 32bit値のtickカウントを得る
//---------------------------------------------------------------------------
tjs_uint32 TVPGetRoughTickCount32()
{
#ifdef TVP_COMPILING_KRKRSDL2
	return SDL_GetTicks();
#else
#ifdef _WIN32
	return timeGetTime();	// win32 mmsystem.h
#else
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC, &now );	// Android の SystemClock.uptimeMillis() では SYSTEM_TIME_MONOTONIC(CLOCK_MONOTONIC) が使われている
	//clock_gettime( CLOCK_BOOTTIME, &now );
	return static_cast<tjs_uint32>( now.tv_sec * 1000LL + now.tv_nsec / 1000000LL );
#endif
#endif
//	return TVPTickCounter.Count();
}


//---------------------------------------------------------------------------
#ifndef TVP_COMPILING_KRKRSDL2
static tjs_uint TVPCheckTickOverflow()
{
	tjs_uint curtick;
	{	// thread-protected
		tTJSCriticalSectionHolder holder(TVPTickWatchCS);

		curtick = TVPGetRoughTickCount32();
		if(curtick < TVPWatchLastTick)
		{
			// timeGetTime() was overflowed
			TVPTickCountBias += 0x100000000L; // add 1<<32
		}
		TVPWatchLastTick = curtick;
	}	// end-of-thread-protected
	return curtick;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class tTVPWatchThread : public tTVPThread
{
	// thread which watches overflow of 32bit counter of TVPGetRoughTickCount32

	tTVPThreadEvent Event;

public:

	tTVPWatchThread();
	~tTVPWatchThread();

protected:
	void Execute();

} static * TVPWatchThread = NULL;
//---------------------------------------------------------------------------
tTVPWatchThread::tTVPWatchThread()
{
	TVPWatchLastTick = TVPGetRoughTickCount32();
	SetPriority(ttpNormal);
	StartTread();
}
//---------------------------------------------------------------------------
tTVPWatchThread::~tTVPWatchThread()
{
	Terminate();
	Event.Set();
	WaitFor();
}
//---------------------------------------------------------------------------
void tTVPWatchThread::Execute()
{
	while(!GetTerminated())
	{
		TVPCheckTickOverflow();

		Event.WaitFor(0x10000000);
			// 0x10000000 will be enough to watch timeGetTime()'s counter overflow.
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TVPWatchThreadInit()
{
	if(!TVPWatchThread)
	{
		TVPWatchThread = new tTVPWatchThread();
	}
}
//---------------------------------------------------------------------------
static void TVPWatchThreadUninit()
{
	if(TVPWatchThread)
	{
		delete TVPWatchThread;
		TVPWatchThread = NULL;
	}
}
//---------------------------------------------------------------------------
static tTVPAtExit TVPWatchThreadUninitAtExit(TVP_ATEXIT_PRI_SHUTDOWN,
	TVPWatchThreadUninit);
//---------------------------------------------------------------------------
#endif



//---------------------------------------------------------------------------
// TVPGetTickCount
//---------------------------------------------------------------------------
tjs_uint64 TVPGetTickCount()
{
#ifdef TVP_COMPILING_KRKRSDL2
	return TVPGetRoughTickCount32();
#else
	TVPWatchThreadInit();

	tjs_uint curtick = TVPCheckTickOverflow();

	return curtick + TVPTickCountBias;
#endif
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPStartTickCount
//---------------------------------------------------------------------------
void TVPStartTickCount()
{
#ifndef TVP_COMPILING_KRKRSDL2
	TVPWatchThreadInit();
#endif
}
//---------------------------------------------------------------------------



