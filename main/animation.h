#ifndef __ANIMATION_H__
#define __ANIMATION_H__
#include"hulib.h"
#include"codec.h"
#include"image_transform.h"
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
	//	type=0;
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
	int start_alpha;
	int end_alpha;
	AlphaAnim(){
		type=1;
		start_alpha=0;
		end_alpha=100;
	}
	 void doFlushConfig(){
		duration=m_mp["duration"]->getvalue_int();
		steps = m_mp["steps"]->getvalue_int();
		start_alpha = m_mp["start_alpha"]->getvalue_int();
		end_alpha = m_mp["end_alpha"]->getvalue_int();
	}


	template <typename  T>
	void renderAnim(T *dst,image *src,void (T::*pfun)(image * )){
		int old_transp=src->transp;
		image temp;
		for(int i=0;i<steps;i++){
			src->transp=i*(end_alpha-start_alpha)/steps+start_alpha;
			temp.SetBuffer(src->u32Width,src->u32Height);
			temp.Render(src,0,0);
			(dst->*pfun)(&temp);
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
				usleep(100);
				x=start_x+i*(end_x-start_x)/steps;
				y=start_y+i*(end_y-start_y)/steps;
				(dst->*pfun)(src,0,0,src->u32Width,src->u32Height,x,y);
				 FPSWaitTimer(duration);
			}
		}
	};

	class RotateAnim:public Animation{

	public:
		int start_angle;
		int end_angle;
		//int center;
		float start_scale;
		RotateAnim(){
			type=3;
			start_angle=0;
			start_angle=0;
			start_scale=0.2;
			//center=0;
		}
		 void doFlushConfig(){
			duration=m_mp["duration"]->getvalue_int();
			steps = m_mp["steps"]->getvalue_int();
			start_angle = m_mp["start_angle"]->getvalue_int();
			end_angle = m_mp["end_angle"]->getvalue_int();
			//center= m_mp["center"]->getvalue_int();
			if(m_mp.exist("start_scale"))
				start_scale=(float)m_mp["start_scale"]->getvalue_int()/100;
		}

		template <typename  T>
		void renderAnim(T *dst,image *src,void (T::*pfun)(image *)){
			int  angle;
			image temp,temp2;
			temp2.SetBuffer(src->u32Width,src->u32Height);
			for(int i=0;i<steps;i++){
				angle=start_angle+i*(end_angle-start_angle)/steps;
				float scale=start_scale+i*(1-start_scale)/steps;
//				if(center){
//					ImageTransform::rotete_center(temp,*src,angle,scale,scale);
//				}else
					ImageTransform::rotate_no_bilinear(temp,*src,angle,scale,scale,0,0);
				temp2.AreaCopy(&temp,0,0);
				(dst->*pfun)(&temp2);
				 FPSWaitTimer(duration);
			}
		}
	};



	class ScaleAnim:public Animation{

	public:
		int start_angle;
		int end_angle;
		int start_x;
		int end_x;
		int start_y;
		int end_y;
		//int center;
		float start_scale;
		float end_scale;
		ScaleAnim(){
			type=4;
			start_scale=0.2;
			end_scale=1;
			start_x=0;
			start_y=0;
			end_x=0;
			end_y=0;
			//center=0;
		}
		 void doFlushConfig(){
			duration=m_mp["duration"]->getvalue_int();
			steps = m_mp["steps"]->getvalue_int();
			//center= m_mp["center"]->getvalue_int();
			if(m_mp.exist("start_scale"))
				start_scale=(float)m_mp["start_scale"]->getvalue_int()/100;
			if(m_mp.exist("end_scale"))
				end_scale=(float)m_mp["end_scale"]->getvalue_int()/100;
		}

		template <typename  T>
		void renderAnim(T *dst,image *src,void (T::*pfun)(image * ,int ,int ,int ,int ,int ,int )){

			image temp;
			int x,y;
			for(int i=0;i<steps;i++){
				float scale=start_scale+i*(end_scale-start_scale)/steps;
				x=start_x+i*(end_x-start_x)/steps;
				y=start_y+i*(end_y-start_y)/steps;
				temp.SetBuffer(src->u32Width*scale,src->u32Height*scale);
				ImageTransform::zoom_no_bilinear(temp,*src);
				//(dst->*pfun)(&temp);
				(dst->*pfun)(&temp,0,0,temp.u32Width,temp.u32Height,x,y);
				 FPSWaitTimer(duration);
			}
		}
	};





#endif //__ALPHA_H__
