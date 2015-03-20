#include "xmlproc.h"
#include "thread_touch.h"
#include "thread_timer.h"
#include "thread_msg.h"

DebugTimer fps;
pXmlproc g_cur_xml;
map<hustr, pXmlproc> g_xml_proc;

ProcTimer g_exec;

int HuExec::doStart()
{
	if (have)
	{
		if (!run.empty())
		{
			system(run);
		}
		if (!cs.empty())
		{
			log_i("exec xml=%s cs=%s\r\n", g_cur_xml->filename.c_str(), cs.c_str());
			g_cur_xml->PostCS(cs);
		}
	}

}

int xmlproc::init()
{
	elemgr = this;
	isDraw = 0; //默认无绘制图像
	fore = 0; //默认为后台进程
	done = 0; //默认非完成状态
	if (out.isNULL())
	{
		if (out.SetBuffer(fb.u32Width, fb.u32Height))
		{
			huErrExit("$$$HU$$$ blt_set_dest_buff error\r\n");
		}
	}
	m_exit = 1;
	create();
}

/********************************
 * 执行完这个函数，页面就前台了，可以考虑在后头添加过场动画
 */

void xmlproc::ForeProc()
{

	g_th_touch.SwitchProc(this);
	g_th_timer.SwitchProc(this);
	g_exec.ChangeContainer(this);
	fore = 1;
	isDraw++;//切换前台后自动刷新一次屏幕
	if(windCtl!=NULL){
		windCtl->onComing();
	}
	postSem();
}
/********************************
 * 执行完这个函数，页面就后台了，可以考虑在前头添加过场动画
 */
void xmlproc::UnForeProc()
{
	fore = 0;
	g_th_touch.SwitchProc(NULL);
	g_th_timer.SwitchProc(NULL);
	g_exec.ChangeContainer(NULL);
	if(windCtl!=NULL){
		windCtl->onLeaving();
	}
}

void xmlproc::DoneProc()
{
	done = 1;
	if(windCtl!=NULL){
		windCtl->onResume();
	}
}
void xmlproc::UnDoneProc()
{
	done = 0;
	if(windCtl!=NULL){
		if(windCtl->isRun())
			windCtl->onPause();
	}
}

