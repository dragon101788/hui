#ifndef __THREAD_EXEC_H__
#define __THREAD_EXEC_H__

#include "hulib.h"


template<typename T>
class ExecThread: public thread, public Mutex
{
	int t;
public:
	typedef int (*timer_fun_ft)(int tm, T data);
	class timer_node
	{
	public:
		int m_tm;
		timer_fun_ft m_fun;
		T m_data;
	};
	class Container: virtual public Mutex
	{
	public:
		typedef typename map<hustr, timer_node>::iterator iterator;
		map<hustr, timer_node> mp;
		void UpdateContainer(int cur)
		{
			lock();
			iterator it;
			for (it = mp.begin(); it != mp.end();)
			{
				if (cur > it->second.m_tm)
				{
					timer_fun_ft fun = it->second.m_fun;
					T data = it->second.m_data;
					mp.erase(it++);

					fun(cur, data);
					continue;
				}
				++it;
			}
			unlock();

		}
		void ExecAdd(const char * name, int tm, timer_fun_ft fun, T data)
		{
			lock();
			mp[name].m_data = data;
			mp[name].m_fun = fun;
			mp[name].m_tm = tm;
			unlock();
		}
	};

	ExecThread()
	{
		name = "ExecThread";
		t = GetTickCount();
		create();
	}
	unsigned long GetTickCount()
	{
		struct timespec ts;

		clock_gettime(CLOCK_MONOTONIC, &ts);

		return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
	}
	inline int GetUpTimer()
	{
		return GetTickCount() - t;
	}
	int run()
	{
		while (1)
		{
			lock();
			int cur = GetUpTimer();
			if (m_Container != NULL)
			{
				m_Container->UpdateContainer(cur);
			}
			unlock();

			usleep(1000);
		}
	}
	void ChangeContainer(Container * c)
	{
		lock();
		m_Container = c;
		unlock();
	}
//	void add(const char * name, int tm, timer_fun_ft fun, T data)
//	{
//		lock();
//		mp[name].m_tm = tm;
//		mp[name].m_fun = fun;
//		mp[name].m_data = data;
//		unlock();
//	}

	Container * m_Container;
};



template<typename T>
class DirectExecThread: public thread, public Mutex
{
public:
	typedef int (*fun_ft)(T data);
	class ExeNode
	{
	public:
		fun_ft m_fun;
		T m_data;
	};

	typedef typename map<hustr, ExeNode>::iterator iterator;
	map<hustr, ExeNode> mp;

	void execAdd(const char * name, fun_ft fun, T data)
	{
		lock();
		mp[name].m_data = data;
		mp[name].m_fun = fun;
		unlock();
	}


	DirectExecThread()
	{
		name = "DirectExecThread";
		//t = GetTickCount();
		create();
	}
	void update(){
	lock();
	iterator it;
	for (it = mp.begin(); it != mp.end();)
	{
		fun_ft fun = it->second.m_fun;
		T data = it->second.m_data;
		mp.erase(it++);
		fun(data);
	}
	unlock();
}
	int run()
	{
		while (1)
		{
			update();
			usleep(5000);
		}
	}

};

#endif //__THREAD_TOUCH_H__
