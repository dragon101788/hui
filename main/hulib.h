#ifndef __HULIB_HU__
#define __HULIB_HU__
#include <unistd.h>
#include <stdarg.h>
#include <string>
#include <pthread.h>
#include <map>
#include <vector>
#include <algorithm>
#include <list>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/prctl.h>
#include <semaphore.h>
#include <deque>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
using namespace std;

unsigned long GetTickCount();
void debug_timer(const char * str = NULL);
void FPSWaitTimer(int ms);
#define FPSWaitFPS(fps) FPSWaitTimer(1000/fps)

//static inline void errexit(const char * str)
//{
//	printf("**************error exit*****************\r\n");
//	printf("%s\r\n",str);
//	printf("*****************************************\r\n");
//	exit(-1);
//}
void errexit(const char * str);
#define errexitf(str,...) errexit(hustr(str, ##__VA_ARGS__ ))

void debug_usage();
void dumpstack();
int system_noblk(const char *cmdstring);
//#define debug(a,...)	printf(a,##__VA_ARGS__)
//#define debug(a,...)

class hustr: public string
{
public:
	hustr()
	{

	}

//	hustr(const char * str)
//	{
//		assign(str);
//	}
	void Replace(const char * rst, const char * dst)
	{
		replace(find(rst), strlen(rst), dst, strlen(dst));
	}

	hustr str_key(const char * str = "=")
	{
		int len = length() + 1;
		char * tmp = (char *) alloca(len);
		memset(tmp, 0, len);
		strcpy(tmp, c_str());
		char * p = strstr(tmp, str);
		*p = 0;
		char * key = tmp;
		char * value = p + 1;
		return key;
	}
	hustr str_value(const char * str = "=")
	{
		int len = length() + 1;
		char * tmp = (char *) alloca(len);
		memset(tmp, 0, len);
		strcpy(tmp, c_str());
		char * p = strstr(tmp, str);
		*p = 0;
		char * key = tmp;
		char * value = p + 1;
		return value;
	}

	hustr(const char * fmt, ...)
	{
		char buffer[256];
		if (fmt == NULL)
		{
			return;
		}
		va_list argptr;
		int cnt;

		va_start(argptr, fmt);
		cnt = vsprintf(buffer, fmt, argptr);
		va_end(argptr);

		assign(buffer);

	}

	int format(const char * fmt, ...)
	{
		char buffer[256];
		if (fmt == NULL)
		{
			errexitf("hustr format error\r\n");
		}
		va_list argptr;
		int cnt;
		va_start(argptr, fmt);
		cnt = vsprintf(buffer, fmt, argptr);
		va_end(argptr);

		assign(buffer);

		return (cnt);
	}

	const char * nstr()
	{
		if (empty())
		{
			return NULL;
		}
		else
			return c_str();
	}
	operator const char *()
	{
		return c_str();
	}

};

template<typename T>
class SmartPtr //����ָ��
{
private:
	//T *ptr;
	unsigned int *pUse;
public:
	T *ptr;
	SmartPtr(T *p = NULL) :
			ptr(p), pUse(new unsigned int(1)) //���캯��
	{
	}
	SmartPtr(const SmartPtr& src) :
			ptr(src.ptr), pUse(src.pUse) //�������캯��
	{

		++*pUse;
	}
	SmartPtr& operator=(const SmartPtr& rhs) //��ֵ
	{
		// self-assigning is also right
		++*rhs.pUse;
		DestroyUse(); //����Լ���ԭ��ָ��
		ptr = rhs.ptr;
		pUse = rhs.pUse;

		return *this;
	}
	T *operator->()
	{
		if (!ptr)
		{
			errexitf("SmartPtr access NULL ptr\r\n");
		}
		return ptr;
	}

//	T* operator&()
//	{
//		if (!ptr)
//		{
//			errexit("SmartPtr access NULL ptr\r\n");
//		}
//		return ptr;
//	}
//	const T* operator&() const
//	{
//		if (!ptr)
//		{
//			errexit("SmartPtr access NULL ptr\r\n");
//		}
//		return ptr;
//	}
	const T *operator->() const
	{
		if (!ptr)
		{
			errexitf("SmartPtr access NULL ptr\r\n");
		}
		return ptr;
	}
	T &operator*()
	{
		if (!ptr)
		{
			errexitf("SmartPtr access NULL ptr\r\n");
		}
		return *ptr;
	}
	const T &operator*() const
	{
		if (!ptr)
		{
			errexitf("SmartPtr access NULL ptr\r\n");
		}
		return *ptr;
	}
	operator T()
	{
		if (!ptr)
		{
			errexitf("SmartPtr access NULL ptr\r\n");
		}
		return *ptr;
	}
	operator T*()
	{
		if (!ptr)
		{
			errexitf("SmartPtr access NULL ptr\r\n");
		}
		return ptr;
	}
	operator const T*() const
	{
		if (!ptr)
		{
			errexitf("SmartPtr access NULL ptr\r\n");
		}
		return ptr;
	}

	bool isNULL()
	{
		if (ptr == NULL)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename rT>
	rT Switch()
	{
		return (rT) ptr;
	}

	~SmartPtr()
	{
		DestroyUse();
		//printf("$$$HU$$$ Auto Ptr ~SmartPtr\n");

	}
	void Release()
	{
		DestroyUse();
		ptr = NULL;
		pUse = new unsigned int(1);
	}

private:
	void DestroyUse()
	{
		if (--*pUse == 0)
		{
			if (ptr != NULL)
				log_i("$$$HU$$$ Auto Ptr DestroyUse Release content\n");
			delete ptr;
			delete pUse;
			ptr = NULL;
			pUse = NULL;

		}
	}

};

class Mutex
{
public:
	pthread_mutex_t mutex;
	void lock()
	{
		pthread_mutex_lock(&mutex);
	}
	int timed_lock(int usec)
	{
		while (try_lock() != 0)
		{
			usleep(1000);
			usec -= 1000;
			//printf("timed_lock %d\r\n",usec);
			if (usec < 0)
			{
				return -1;
			}
		}
		return 0;
	}
	int try_lock()
	{
		pthread_mutex_trylock(&mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(&mutex);
	}

	Mutex(int flag = PTHREAD_MUTEX_RECURSIVE_NP)
	{
		pthread_mutexattr_init(&mattr);
		pthread_mutexattr_settype(&mattr, flag);
		pthread_mutex_init(&mutex, &mattr);
	}
	virtual ~Mutex()
	{
		//printf("~Mutex\r\n");
		pthread_mutex_destroy(&mutex);
		pthread_mutexattr_destroy(&mattr);
	}
private:

	pthread_mutexattr_t mattr;
};

class pMutex
{
public:

	pMutex()
	{
		mutex = NULL;
	}
	void SetMutex(Mutex * mut)
	{
		mutex = mut;
	}
	void lock()
	{
		if (mutex != NULL)
		{
			mutex->lock();
		}
	}
	int try_lock()
	{
		if (mutex != NULL)
		{
			return mutex->try_lock();
		}
	}
	void unlock()
	{
		if (mutex != NULL)
		{
			mutex->unlock();
		}
	}
	Mutex * mutex;
};

class thread
{
public:
	virtual int run()
	{
		log_i("call base thread run\r\n");
	}

	virtual int destroy()
	{
		log_i("call base thread destroy\r\n");
	}
	static void cleanup(void * arg)
	{
		thread * is = (thread *) arg;
		log_i("[%s] thread asynchronous interrupt\r\n", is->name.c_str());
		is->destroy();
	}
	static void * create_thread(void *arg)
	{
		thread * is = (thread *) arg;

		prctl(PR_SET_NAME, is->name.c_str());
		pthread_cleanup_push(cleanup,is);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		is->run();
		pthread_cleanup_pop(0);
		pthread_detach(pthread_self());
		pthread_exit(NULL);
	}
	void create()
	{

		int err = pthread_create(&tid, NULL, create_thread, this);
		if (err != 0)
		{
			errexitf("can't create thread: %s\n", strerror(err));
		}

	}


	void wait()
	{
		if (tid != -1)
		{
			pthread_join(tid, NULL);
		}

	}
	void cancel()
	{
		if (tid != -1)
		{
			if (!pthread_cancel(tid))
			{
				log_i("pthread_cancel %s OK\n ", name.c_str());
				wait();
				tid = -1;
			}
			else
			{
				log_e("pthread_cancel %s faild: %s\n ", name.c_str(), strerror(errno));
			}
		}
	}
	void kill(int sig)
	{
		pthread_kill(tid, sig);
	}
	thread()
	{
		tid = -1;
	}
	virtual ~thread()
	{
		cancel();
	}
	pthread_t tid;
	bool isRuning(){
		return tid==-1?false:true;
	}
	hustr name;
};

class thread_debug: public thread
{
public:
	int timer;
	int m_max;
	hustr m_name;
	thread * m_thd;
	int run()
	{
		while (1)
		{
			if (GetTickCount() > timer + m_max)
			{
				log_w("%s thread Timer Out\r\n", m_name.nstr());
				m_thd->kill(11);
				return -1;
			}
			usleep(1000);
		}
		return 0;
	}
	int start(const char * name, thread * thd, int max)
	{
		timer = GetTickCount();
		m_max = max;
		m_name = name;
		m_thd = thd;
		create();
	}
	int end()
	{
		cancel();
	}
};
class huvar
{
public:

	void init_arg(int argc, char * argv[])
	{
		for (int i = 0; i < argc; i++)
		{
			char * p = strchr(argv[i], '=');
			if (p != NULL)
			{
				*p = 0;
				mp[argv[i]] = p + 1;
			}
			else
			{
				mp[argv[i]] = "1";
			}
		}
	}

	const char * getvar(const char * str)
	{

		map<hustr, hustr>::iterator it;
		it = mp.find(str);
		if (it != mp.end())
		{
			return it->second.c_str();
		}
		else
		{
			return NULL;
		}

	}
	int getvar_int(const char * str)
	{
		const char * p = getvar(str);
		if (p != NULL)
		{
			return strtol(p, NULL, 10);
		}
		return 0;
	}

	map<hustr, hustr> mp;
};

extern huvar g_var;

//class HuDebug
//{
//public:
//	int checkout(const char * str)
//	{
//		if (str[0] == '<')
//		{
//			char tmp[32] =
//			{ 0 };
//			sscanf(str, "<%[^>]", tmp);
//			if (mp[tmp] == 0)
//			{
//				return 1;
//			}
//			else
//			{
//				return 0;
//			}
//		}
//		return 1;
//	}
//	void output(const char * fmt, ...)
//	{
//		if (checkout(fmt))
//			return;
//
//		if (fmt == NULL)
//		{
//			return;
//		}
//		va_list argptr;
//		int cnt;
//
//		va_start(argptr, fmt);
//		cnt = vprintf(fmt, argptr);
//		va_end(argptr);
//	}
//	void fps(const char * str)
//	{
//		if (checkout(str))
//			return;
//		debug_timer(str);
//	}
//	int & operator[](const char * str)
//	{
//		return mp[str];
//	}
//	map<hustr, int> mp;
//};
//extern HuDebug debug;

class Sem
{
public:
	Sem()
	{
		int ret=sem_init(&m_sem, 0, 0);
		if ( 0 != ret )
		{
	        log_e("semaphore intitialization failed\n");
	        exit(EXIT_FAILURE);
		}
	}
	Sem(int init_value)
	{
		int ret = sem_init(&m_sem, 0, init_value);
		if ( 0 != ret )
		{
	        log_e("semaphore intitialization failed\n");
	        exit(EXIT_FAILURE);
		}
	}
	virtual ~Sem()
	{
		sem_destroy(&m_sem);
	}
	int postSem()
	{
		sem_post(&m_sem);
	}
	int waitSem()
	{
		sem_wait(&m_sem);
	}
	int tryWait()
	{
		return sem_trywait(&m_sem);
	}
//	int wait_timerout(int msecs)
//	{
//		struct timespec ts;
//		int tm = GetTickCount() + msecs * 1000;
//		ts.tv_sec = tm / (1000 * 1000 * 1000);
//		ts.tv_nsec = tm % (1000 * 1000 * 1000);
//		//clock_gettime(CLOCK_MONOTONIC, &ts);
//
//		//ts.tv_nsec += msecs*1000;
//
//		return sem_timedwait(&m_sem, &ts);
//		//ETIMEOUT 超时
//	}
	bool waitSem(long milliseconds){
		if ( 0 == milliseconds )
		{
			sem_trywait(&m_sem);
		}
		else
		{
			int roopMax = milliseconds/10;
			do
			{
				usleep(10*1000);
				int ret = sem_trywait(&m_sem);
				if ( 0 == ret )
				{
					break;
				}
				roopMax--;
			} while( roopMax > 0 );
		}

		return true;
	}


	/*
	 将当前事件对象设置为无信号状态
	*/
	void resetSem(){
		int sval = 0;
		do
		{
			sem_trywait(&m_sem);
			sem_getvalue(&m_sem, &sval);
		} while(sval > 0);
	}

private:
	sem_t m_sem;
};




template<typename T>
class queue: public Mutex, public Sem
{
public:
	queue()
	{
	}
	void clean()
	{
		lock();
		m_list.clear();
		unlock();
	}
	void addele(T ele)
	{
		lock();
		m_list.push_back(ele);
		postSem();
		unlock();
	}
	T getele()
	{
		waitSem();
		lock();
		T ret = m_list.front();
		m_list.pop_front();
		unlock();
		return ret;
	}
	T getele_nowait()
	{
		if (tryWait() != 0)
		{
			if (errno == EAGAIN)
			{
				return NULL;
			}
			else
			{
				errexitf("getele_nowait %d\r\n", errno);
			}
		}
		lock();
		T ret = m_list.front();
		m_list.pop_front();
		unlock();
		return ret;
	}
	T getele_timerout(int msecs)
	{
		if (waitSem(msecs) != 0)
		{
			if (errno == ETIMEDOUT)
			{
				return NULL;
			}
			else
			{
				errexitf("getele_timerout %d\r\n", errno);
			}
		}
		lock();
		T ret = m_list.front();
		m_list.pop_front();
		unlock();
		return ret;
	}
	void delele(T ele)
	{
		lock();
		m_list.remove(ele);
		unlock();
	}
	int empty()
	{
		int ret;
		lock();
		m_list.empty();
		unlock();
		return ret;
	}
	list<T> m_list;
};

class DebugTimer
{
	int oldtime;
public:
	DebugTimer()
	{
		oldtime = GetTickCount();

	}
	void debug_timer(const char * str)
	{
		int usetime = GetTickCount() - oldtime;
		if (str != NULL)
			log_s("debug_timer [%s] %dfps/S fps/%dms\r\n", str,
					usetime ? 1000 / usetime : 1000, usetime);
		oldtime = GetTickCount();
	}
};



template<typename T>
class humap: public multimap<hustr, humap<T> >
{
public:
	humap(const char * str = NULL, humap * f = NULL, int o = 0)
	{
		m_order = o;
		m_father = f;
		m_key = str;
	}
	typedef typename multimap<hustr, humap>::iterator iterator_base;
	hustr m_key;
	T m_val;
	humap * m_father;
	int m_order;

	class iterator: public iterator_base
	{
	public:
		//iterator m_it;
		humap * m_mgr;
		iterator()
		{
			//(*this) = (*it);
			m_mgr = NULL;
			//m_it = it;
		}
		iterator(iterator_base it) :
				iterator_base(it)
		{
			//(*this) = (*it);
			m_mgr = NULL;
			//m_it = it;
		}
		iterator(iterator_base it, humap * mgr) :
				iterator_base(it)
		{
			//(*this) = (*it);
			m_mgr = mgr;
			//m_it = it;
		}
		hustr & operator=(const char * key)
		{
			(**this).second.m_val = key;
			//return m_val = key;
		}
		iterator operator[](const char * key)
		{
			return (**this).second.Get(key);
		}
		iterator operator[](int id)
		{
			return m_mgr->Get((**this).first.c_str(), id);
		}
		const hustr & key()
		{
			return (**this).first;
		}
		T & value()
		{
			return (**this).second.m_val;
		}
		operator humap &()
		{
			return (**this).second;
		}
		operator humap *()
		{
			return &(**this).second;
		}
		T * operator->()
		{
			return &(**this).second.m_val;
		}
	};

	class OrderList:public vector<humap>
	{
	public:
		typedef typename vector<humap>::iterator iterator;
		static int cmp(const humap & a, const humap & b)
		{
			return a.m_order < b.m_order;
		}
		void display()
		{
			for (iterator it = this->begin(); it != this->end(); it++)
			{
				//printf("%s %s\r\n", (*it).m_key.c_str(), (*it).m_val.c_str());
				(*it).display();
			}
		}
		void accept(humap & mp)
		{
			for (typename humap::iterator it = mp.begin(); it != mp.end(); it++)
			{
				this->push_back((*it).second);
			}
			sort(this->begin(), this->end(), cmp);
		}
		OrderList()
		{

		}
	};
	int exist(const char * key)
	{
		if (this->find(key) != this->end())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	void fetch(humap & mp)
	{
		*this = mp;
	}
	T * operator->()
	{
		return &m_val;
	}
	iterator operator[](const char * key)
	{
		return Get(key);
	}
	iterator CreateMultiLast(const char * key)
	{
		//printf("CreateMultiLast [%s]\r\n", key);
		return iterator(this->insert(pair<hustr, humap>(key, humap(key, this, this->size()))), this);
	}
	iterator Get(const char * key)
	{
		iterator it = this->find(key);
		if (it != this->end())
		{
			return iterator(it, this);
		}
		else
		{
			return iterator(this->insert(pair<hustr, humap>(key, humap(key, this, this->size()))), this);
		}
	}
	iterator Get(const char * key, int id)
	{

		iterator it = this->find(key);
		if (it != this->end())
		{
			int count = this->count(key);
			//printf("count=%d id=%d\r\n", count, id);
			int i;
			for (i = 0; i < count; ++i)
			{
				if (i == id)
				{
					//printf("find %s %d\r\n", key, i);
					return iterator(it, this);
				}
				++it;
			}
			if (i >= count)
			{
				log_i("key %s id %d not find\r\n", key, i);
				exit(-1);
			}
		}
		else
		{
			return iterator(this->insert(pair<hustr, humap>(key, humap(key, this, this->size()))), this);
		}
	}
	void display()
	{
		displayspace(retrospect_tree());
		log_i("[%s]=[%s]  <%d>\r\n", m_key.c_str(), m_val.c_str(),m_order);
		iterator it;
		for (it = this->begin(); it != this->end(); ++it)	//遍历
		{
			(*it).second.display();
		}
	}

	int retrospect_tree()
	{
		int i = 0;
		humap * p = this;
		while (p->m_father != NULL)
		{
			i++;
			p = p->m_father;
		}
		return i;
	}

	void displayspace(int n, const char * str = "     ")
	{
		for (int i = 0; i < n; i++)
			log_i("%s", str);
	}
	void setfather(humap * father)
	{
		m_father = father;
		iterator it;
		for (it = this->begin(); it != this->end(); ++it)
		{
			it->second.setfather(this);
		}

	}
};
class mapv: public hustr
{
public:
	mapv()
	{

	}
	mapv(const char * fmt, ...)
	{
		char buffer[256];
		if (fmt == NULL)
		{
			return;
		}
		va_list argptr;
		int cnt;

		va_start(argptr, fmt);
		cnt = vsprintf(buffer, fmt, argptr);
		va_end(argptr);

		assign(buffer);

	}

	const char * getvalue()
	{
		const char * tmp = c_str();
		if (tmp[0] == '$')
		{
			const char * name = tmp + 1;
			if (g_var.getvar(name) != NULL)
			{
				tmp = g_var.getvar(name);
			}
			else
			{
				tmp = getenv(name);
			}

			if (tmp == NULL)
			{
				errexitf("$$$HU$$$$ Xml file Variable parameter [%s] undefined!!!\r\n", name);
			}
		}
		return tmp;
	}
	 unsigned int  getvalue_hex()
	{
		const char * tmp = c_str();
		return hex2dec(tmp);
	}
	int getvalue_int()
	{
		return strtol(getvalue(), NULL, 10);
	}


private:
	/*
	 * 将字符转换为数值
	 * */
	unsigned int c2i(char ch)
	{
	        // 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2
	        if(isdigit(ch))
	                return ch - 48;
	        // 如果是字母，但不是A~F,a~f则返回
	        if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )
	                return -1;

	        // 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10
	        // 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10
	        if(isalpha(ch))
	                return (unsigned int)(isupper(ch) ? ch - 55 : ch - 87);

	        return -1;
	}

	/*
	 * 功能：将十六进制字符串转换为整型(int)数值
	 * */
	unsigned int hex2dec(const char *hex)
	{
	        int len;
	        unsigned   int num = 0;
	        unsigned   int temp;
	        int bits;
	        int i;

	        // 此例中 hex = "1de" 长度为3, hex是main函数传递的
	        len = strlen(hex);

	        for (i=0, temp=0; i<len; i++, temp=0)
	        {
	                // 第一次：i=0, *(hex + i) = *(hex + 0) = '1', 即temp = 1
	                // 第二次：i=1, *(hex + i) = *(hex + 1) = 'd', 即temp = 13
	                // 第三次：i=2, *(hex + i) = *(hex + 2) = 'd', 即temp = 14
	                temp = c2i( *(hex + i) );
	                // 总共3位，一个16进制位用 4 bit保存
	                // 第一次：'1'为最高位，所以temp左移 (len - i -1) * 4 = 2 * 4 = 8 位
	                // 第二次：'d'为次高位，所以temp左移 (len - i -1) * 4 = 1 * 4 = 4 位
	                // 第三次：'e'为最低位，所以temp左移 (len - i -1) * 4 = 0 * 4 = 0 位
	                bits = (len - i - 1) * 4;
	                temp = temp << bits;

	                // 此处也可以用 num += temp;进行累加
	                num = num | temp;
	        }

	        // 返回结果
	        return num;
	}

};
typedef humap<mapv> HUMap;











#endif //__HULIB_HU__
