

#ifndef _ROTATE_H_
#define _ROTATE_H_

#include"codec.h"
#include <math.h>

//class image_rotate{


//	int abs(float number)
//	{
//	        return (number >= 0 ? number : -number);
//	}


	/**
	 * 水平方向错切变换
	 *
	 * @param input - 输入像素数据
	 * @param shear - 错切角度
	 * @param width - 图像像素数据宽度
	 * @param height - 图像像素数据高度
	 * @return
	 */
	void  xshear(image *src,image *dst, float shear) {
		int width= src->GetWidth();
		int height=src->GetHeight();
	//	int outw = (int)(abs(shear) * height +width );
		int outw = (int)(tan(shear)* height +width );
		int outh = height;
		dst->SetBuffer(outw,outh);
		unsigned int * input=(unsigned int *)src->SrcGPUAddr();
		unsigned int * output=(unsigned int *)dst->SrcGPUAddr();
		// initialization - 初始化计算变量
		float skew = 0.0f;
		float skewi = 0.0f;
		float skewf = 0.0f;
		int index = 0;
		int outdex = 0;
		float leftred = 0.0f, leftgreen = 0.0f, leftblue = 0.0f;
		float oleftred = 0.0f, oleftgreen = 0.0f, oleftblue = 0.0f;
		int ta = 0, tr=0, tg = 0, tb = 0;

		// 执行对每个像素的错切变换
		for(int row=0; row<height; row++) {
			// skew = shear * (height-1-row + 0.5f); big issue!! very difficulty to find it
			skew = shear * (row + 0.5f);
			skewi = (float)floor(skew);
			skewf = skew - skewi;
			for(int col=0; col<width; col++) {
				index = row * width + col;
        		ta = (input[index] >> 24) & 0xff;
                tr = (input[index] >> 16) & 0xff;
                tg = (input[index] >> 8) & 0xff;
                tb = input[index] & 0xff;
                if(tr == tg && tg == tb && tb == 0) {
                	continue;
                }
                // calculate interpolation pixel value
				leftred = (skewf * tr);
				leftgreen = (skewf * tg);
				leftblue = (skewf * tb);
				// calculate the new pixel RGB value
				tr = (int)(tr - leftred + oleftred);
				tg = (int)(tg - leftgreen + oleftgreen);
				tb = (int)(tb - leftblue + oleftblue);

				// fix issue, need to check boundary
				// computation the new pixel postion here!!
				outdex = (int)(row * outw + col + skewi);
				output[outdex] = (ta << 24) | (tr << 16) | (tg << 8) | tb;

				// ready for next pixel.
				oleftred = leftred;
				oleftgreen = leftgreen;
				oleftblue = leftblue;
			}
		}
		//return output;
	}


	void  yshear(image *src,image *dst, float shear) {
		int width= src->GetWidth();
		int height=src->GetHeight();
		//int outh = (int)(shear * width + height);
		int outh = (int)(tan(shear) * width + height);
		int outw = width;
		dst->SetBuffer(outw,outh);
		unsigned int * input=(unsigned int *)src->SrcGPUAddr();
		unsigned int * output=(unsigned int *)dst->SrcGPUAddr();

		// initialization - 初始化计算变量
		float skew = 0.0f;
		float skewi = 0.0f;
		float skewf = 0.0f;
		int index = 0;
		int outdex = 0;
		float leftred = 0.0f, leftgreen = 0.0f, leftblue = 0.0f;
		float oleftred = 0.0f, oleftgreen = 0.0f, oleftblue = 0.0f;
		int ta = 0, tr=0, tg = 0, tb = 0;

		for(int col = 0; col < width; col++) {
			// the trick is here!!, you can control the
			// anti-clockwise or clockwise
			skew = shear * (width-1-col + 0.5f);
			// skew = shear * (col + 0.5f);
			skewi = (float)floor(skew);
			skewf = skew - skewi;
			for(int row = 0; row < height; row++) {
				index = row * width + col;
        		ta = (input[index] >> 24) & 0xff;
                tr = (input[index] >> 16) & 0xff;
                tg = (input[index] >> 8) & 0xff;
                tb = input[index] & 0xff;

                // calculate interpolation pixel value
				leftred = (skewf * tr);
				leftgreen = (skewf * tg);
				leftblue = (skewf * tb);
				// calculate the new pixel RGB value
				tr = (int)(tr - leftred + oleftred);
				tg = (int)(tg - leftgreen + oleftgreen);
				tb = (int)(tb - leftblue + oleftblue);

				// computation the new pixel postion here!!
				// outdex = (int)((height-row + skewi) * outw + col);
				outdex = (int)((row + skewi) * outw + col);
				output[outdex] = (ta << 24) | (tr << 16) | (tg << 8) | tb;

				// ready for next pixel.
				oleftred = leftred;
				oleftgreen = leftgreen;
				oleftblue = leftblue;
			}
		}
		//return output;
	}

//	public:
//	image_rotate(){
//
//	}
//
//	~image_rotate(){
//
//	}
#define PI  3.1415926
	static void rotate(image *src,image *dst,int angle){
		image temp;
		float shear =angle*PI /180;
		if(abs(angle)<15||abs(90-angle)<15){
			xshear(src,&temp,shear);
			yshear(&temp,dst, shear);
		}else{
			xshear(src,dst,shear);
			yshear(dst,&temp, shear);
			xshear(&temp,dst,shear);
		}
	}
//};


#endif
