#ifndef __ALPHA_H__
#define __ALPHA_H__

class image;


void Render_img_to_img(image * dst, image * src, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y);
//void Copy_img_to_img(image * dst, image * src, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y);
void Draw_rectangle(image * dst, int width, int height, int dst_x, int dst_y,unsigned int color,bool is_blend);
#endif //__ALPHA_H__
