#ifndef __ANIMATION_H__
#define __ANIMATION_H__
#include"hulib.h"
#include"codec.h"
//extern void FPSWaitTimer(int ms);
class Animation{
public:
	/*******
	 * 帧率
	 */
	HUMap m_mp;
	int duration;
	/***********
	 * 类型
	 * 0：no
	 * 1:alpha
	 * 2:translation
	 * 3:scale
	 * 4:rotate
	 */
	int type;
	int steps;
	Animation(){
		duration=40;
		type=0;
		steps=0;
	}

	virtual void doFlushConfig(){
	}
	void FlushConfig(){
		doFlushConfig();
	}
	 //(pClass->*pfun)(key);
//	template <typename  T>
//	virtual void renderAnim(T *pClass,void (T::*pfun)(image * ,int ,int ,int ,int ,int ,int )){};
	virtual ~Animation(){
	}

};
//template <typename  T>
class AlphaAnim:public Animation{

public:
	float start_alpha;
	float end_alpha;
	AlphaAnim(){
		type=1;
		start_alpha=0;
		end_alpha=1;
	}
	 void doFlushConfig(){
		duration=m_mp["duration"]->getvalue_int();
		steps = m_mp["steps"]->getvalue_int();
		start_alpha = m_mp["start_alpha"]->getvalue_int()/100;
		end_alpha = m_mp["end_alpha"]->getvalue_int()/100;
	}


	template <typename  T>
	void renderAnim(T *dst,image *src,void (T::*pfun)(image * ,int ,int ,int ,int ,int ,int )){
		int old_transp=src->transp;
		for(int i=0;i<steps;i++){
			src->transp=i*end_alpha/steps+start_alpha;
			(dst->*pfun)(src,0,0,src->u32Width,src->u32Height,0,0);
			 FPSWaitTimer(duration);
		}
		src->transp=old_transp;
	}
};
	class TranslationAnim:public Animation{

	public:
		int start_x;
		int start_y;
		int end_x;
		int end_y;
		TranslationAnim(){
			type=2;
			start_x=0;
			start_y=0;
			end_x=0;
			end_y=0;
		}
		 void doFlushConfig(){
			duration=m_mp["duration"]->getvalue_int();
			steps = m_mp["steps"]->getvalue_int();
			start_x = m_mp["start_x"]->getvalue_int();
			start_y = m_mp["start_y"]->getvalue_int();
			end_x = m_mp["end_x"]->getvalue_int();
			end_y = m_mp["end_y"]->getvalue_int();

		}

		template <typename  T>
		void renderAnim(T *dst,image *src,void (T::*pfun)(image * ,int ,int ,int ,int ,int ,int )){
			int x,y;
			for(int i=0;i<steps;i++){
				x=start_x+i*(end_x-start_x)/steps;
				y=start_y+i*(end_y-start_y)/steps;
				(dst->*pfun)(src,0,0,src->u32Width,src->u32Height,x,y);
				 FPSWaitTimer(duration);
			}
		}
	};


//	void RenderToFramebufferAnim(framebuffer * fb,Animation *anim)
//	{
//		//fb->RenderImageToFrameBuffer(this);
//		int steps=40;
//		for(int i=steps;i>=0;i--){
//		int src_x=u32Width/steps*i;
//		int src_y=u32Height/steps*i;
//		fb->RenderImageToFrameBuffer_part(this,src_x,src_y,u32Width-src_x,u32Height-src_y,0,0);
//		FPSWaitFPS(anim->fps);
//		}
//	}



#endif //__ALPHA_H__
