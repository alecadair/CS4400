/*******************************************
 * Solutions for the CS:APP Performance Lab
 ********************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following student struct 
 */
student_t student = {
  "Alec S. Adair",     /* Full name */
  "alec.adair@utah.edu",  /* Email address */

};
/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/
/*
 * less_naive_pinwheel - The less naive baseline version of pinwheel
 */


char less_naive_pinwheel_descr[] = "less_naive_pinwheel: Less Naive baseline implementation";
void less_naive_pinwheel(int dim, pixel *src, pixel *dest)
{
	int i, j;
	int lim = dim/2;
	for (i = 0; i < lim; i+=16){
		for (j = 0; j < lim; j++){
			dest[RIDX(lim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
			dest[RIDX(lim - 1 - j, (i+1), dim)] = src[RIDX((i+1), j, dim)];
			dest[RIDX(lim - 1 - j, (i+2), dim)] = src[RIDX((i+2), j, dim)];
			dest[RIDX(lim - 1 - j, (i+3), dim)] = src[RIDX((i+3), j, dim)];
			dest[RIDX(lim - 1 - j, (i+4), dim)] = src[RIDX((i+4), j, dim)];
			dest[RIDX(lim - 1 - j, (i+5), dim)] = src[RIDX((i+5), j, dim)];
			dest[RIDX(lim - 1 - j, (i+6), dim)] = src[RIDX((i+6), j, dim)];
			dest[RIDX(lim - 1 - j, (i+7), dim)] = src[RIDX((i+7), j, dim)];
			dest[RIDX(lim - 1 - j, (i+8), dim)] = src[RIDX((i+8), j, dim)];
			dest[RIDX(lim - 1 - j, (i+9), dim)] = src[RIDX((i+9), j, dim)];
			dest[RIDX(lim - 1 - j, (i+10), dim)] = src[RIDX((i+10), j, dim)];
			dest[RIDX(lim - 1 - j, (i+11), dim)] = src[RIDX((i+11), j, dim)];
			dest[RIDX(lim - 1 - j, (i+12), dim)] = src[RIDX((i+12), j, dim)];
			dest[RIDX(lim - 1 - j, (i+13), dim)] = src[RIDX((i+13), j, dim)];
			dest[RIDX(lim - 1 - j, (i+14), dim)] = src[RIDX((i+14), j, dim)];
			dest[RIDX(lim - 1 - j, (i+15), dim)] = src[RIDX((i+15), j, dim)];
		}
	}

	for (i = 0; i < lim; i+=16){
		for (j = 0; j < lim; j++){
			dest[RIDX(lim - 1 - j, lim + i, dim)] = src[RIDX(i, lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+1), dim)] = src[RIDX((i+1), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+2), dim)] = src[RIDX((i+2), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+3), dim)] = src[RIDX((i+3), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+4), dim)] = src[RIDX((i+4), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+5), dim)] = src[RIDX((i+5), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+6), dim)] = src[RIDX((i+6), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+7), dim)] = src[RIDX((i+7), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+8), dim)] = src[RIDX((i+8), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+9), dim)] = src[RIDX((i+9), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+10), dim)] = src[RIDX((i+10), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+11), dim)] = src[RIDX((i+11), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+12), dim)] = src[RIDX((i+12), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+13), dim)] = src[RIDX((i+13), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+14), dim)] = src[RIDX((i+14), lim + j, dim)];
			dest[RIDX(lim - 1 - j, lim + (i+15), dim)] = src[RIDX((i+15), lim + j, dim)];
		}
	}


	for (i = 0; i < lim; i+=16){
		for (j = 0; j < lim; j++){
			dest[RIDX(dim - 1 - j, i, dim)] = src[RIDX(lim + i, j, dim)];
			dest[RIDX(dim - 1 - j, (i+1), dim)] = src[RIDX(lim + (i+1), j, dim)];
			dest[RIDX(dim - 1 - j, (i+2), dim)] = src[RIDX(lim + (i+2), j, dim)];
			dest[RIDX(dim - 1 - j, (i+3), dim)] = src[RIDX(lim + (i+3), j, dim)];
			dest[RIDX(dim - 1 - j, (i+4), dim)] = src[RIDX(lim + (i+4), j, dim)];
			dest[RIDX(dim - 1 - j, (i+5), dim)] = src[RIDX(lim + (i+5), j, dim)];
			dest[RIDX(dim - 1 - j, (i+6), dim)] = src[RIDX(lim + (i+6), j, dim)];
			dest[RIDX(dim - 1 - j, (i+7), dim)] = src[RIDX(lim + (i+7), j, dim)];
			dest[RIDX(dim - 1 - j, (i+8), dim)] = src[RIDX(lim + (i+8), j, dim)];
			dest[RIDX(dim - 1 - j, (i+9), dim)] = src[RIDX(lim + (i+9), j, dim)];
			dest[RIDX(dim - 1 - j, (i+10), dim)] = src[RIDX(lim + (i+10), j, dim)];
			dest[RIDX(dim - 1 - j, (i+11), dim)] = src[RIDX(lim + (i+11), j, dim)];
			dest[RIDX(dim - 1 - j, (i+12), dim)] = src[RIDX(lim + (i+12), j, dim)];
			dest[RIDX(dim - 1 - j, (i+13), dim)] = src[RIDX(lim + (i+13), j, dim)];
			dest[RIDX(dim - 1 - j, (i+14), dim)] = src[RIDX(lim + (i+14), j, dim)];
			dest[RIDX(dim - 1 - j, (i+15), dim)] = src[RIDX(lim + (i+15), j, dim)];

		}
	}

	for (i = 0; i < lim; i+=16){
		for (j = 0; j < lim; j++){
			dest[RIDX(dim - 1 - j, lim + (i), dim)] = src[RIDX(lim + (i), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+1), dim)] = src[RIDX(lim + (i+1), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+2), dim)] = src[RIDX(lim + (i+2), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+3), dim)] = src[RIDX(lim + (i+3), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+4), dim)] = src[RIDX(lim + (i+4), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+5), dim)] = src[RIDX(lim + (i+5), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+6), dim)] = src[RIDX(lim + (i+6), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+7), dim)] = src[RIDX(lim + (i+7), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+8), dim)] = src[RIDX(lim + (i+8), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+9), dim)] = src[RIDX(lim + (i+9), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+10), dim)] = src[RIDX(lim + (i+10), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+11), dim)] = src[RIDX(lim + (i+11), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+12), dim)] = src[RIDX(lim + (i+12), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+13), dim)] = src[RIDX(lim + (i+13), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+14), dim)] = src[RIDX(lim + (i+14), lim + j, dim)];
			dest[RIDX(dim - 1 - j, lim + (i+15), dim)] = src[RIDX(lim + (i+15), lim + j, dim)];

		}
	}

}
/* 
 * naive_pinwheel - The naive baseline version of pinwheel 
 */
char naive_pinwheel_descr[] = "naive_pinwheel: Naive baseline implementation";
void naive_pinwheel(int dim, pixel *src, pixel *dest)
{
  int i, j;

  for (i = 0; i < dim/2; i++)
    for (j = 0; j < dim/2; j++)
      dest[RIDX(dim/2 - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
    
  for (i = 0; i < dim/2; i++)
    for (j = 0; j < dim/2; j++)
      dest[RIDX(dim/2 - 1 - j, dim/2 + i, dim)] = src[RIDX(i, dim/2 + j, dim)];

  for (i = 0; i < dim/2; i++)
    for (j = 0; j < dim/2; j++)
      dest[RIDX(dim - 1 - j, i, dim)] = src[RIDX(dim/2 + i, j, dim)];

  for (i = 0; i < dim/2; i++)
    for (j = 0; j < dim/2; j++)
      dest[RIDX(dim - 1 - j, dim/2 + i, dim)] = src[RIDX(dim/2 + i, dim/2 + j, dim)];
}

/* 
 * rotate - Your current working version of pinwheel
 * IMPORTANT: This is the version you will be graded on
 */
char pinwheel_descr[] = "pinwheel: Current working version";
void pinwheel(int dim, pixel *src, pixel *dest)
{
  less_naive_pinwheel(dim, src, dest);
}

/*********************************************************************
 * register_pinwheel_functions - Register all of your different versions
 *     of the pinwheel kernel with the driver by calling the
 *     add_pinwheel_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_pinwheel_functions() {
  add_pinwheel_function(&pinwheel, pinwheel_descr);
  add_pinwheel_function(&naive_pinwheel, naive_pinwheel_descr);
  add_pinwheel_function(&less_naive_pinwheel, less_naive_pinwheel_descr);
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
  int red;
  int green;
  int blue;
} pixel_sum;

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static inline void initialize_pixel_sum(pixel_sum *sum)
{
  sum->red = sum->green = sum->blue = 0;
  return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static inline void accumulate_weighted_sum(pixel_sum *sum, pixel p, double weight)
{
  sum->red += (int) p.red * weight;
  sum->green += (int) p.green * weight;
  sum->blue += (int) p.blue * weight;
  return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static inline void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
  current_pixel->red = (unsigned short)sum.red;
  current_pixel->green = (unsigned short)sum.green;
  current_pixel->blue = (unsigned short)sum.blue;
  return;
}

/* 
 * weighted_combo - Returns new pixel value at (i,j) 
 */
static inline pixel weighted_combo(int dim, int i, int j, pixel *src)
{
  int ii, jj;
  pixel_sum sum;
  pixel current_pixel;
  double weights[3][3] = { { 0.50, 0.03125, 0.00 },
                           { 0.03125, 0.25, 0.03125 },
                           { 0.00, 0.03125, 0.125 } };

  initialize_pixel_sum(&sum);
  for(ii=0; ii < 3; ii++)
    for(jj=0; jj < 3; jj++) 
      if ((i + ii < dim) && (j + jj < dim))
        accumulate_weighted_sum(&sum,
                                src[RIDX(i+ii,j+jj,dim)],
                                weights[ii][jj]);
    
  assign_sum_to_pixel(&current_pixel, sum);
 
  return current_pixel;
}

/******************************************************
 * Your different versions of the motion kernel go here
 ******************************************************/

char even_less_naive_motion_descr[] = "even_less_naive_motion: An Even less Naive baseline implementation";
void even_less_naive_motion(int dim, pixel *src, pixel *dst){
	//double weights[3][3] = { { 0.50, 0.03125, 0.00 },
	//                           { 0.03125, 0.25, 0.03125 },
	//                           { 0.00, 0.03125, 0.125 } };
	int i, j;
	for(i = 0; i < dim; i++){
		for(j = 0; j < dim; j++){
			int ii, jj;
			pixel_sum inst;
			pixel_sum* sum = &inst;
			pixel current_pixel;
			sum->red = 0;
			sum->green = 0;
			sum->blue = 0;
			ii = 0;
			jj = 0;
			int iplusii = i + ii;
			if ((iplusii < dim) && (j + jj < dim)){
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red >>1;
				  sum->green += (int) p.green >>1;
				  sum->blue += (int) p.blue >>1;
			}
			jj++;
			//(0,1)
			if ((iplusii < dim) && (j + jj < dim)){
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red>>5;
				  sum->green += (int) p.green >>5;
				  sum->blue += (int) p.blue >>5;
			}
			ii++;
			iplusii++;
			jj = 0;
			//(1,0)
			if ((iplusii < dim) && (j + jj < dim)){
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red >>5;
				  sum->green += (int) p.green >>5;
				  sum->blue += (int) p.blue >>5;
			}
			jj++;
			//(1,1)
			if ((iplusii < dim) && (j + jj < dim)){
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red >>2;
				  sum->green += (int) p.green >>2;
				  sum->blue += (int) p.blue  >>2;
			}
			jj++;
			//(1,2)
			if ((iplusii < dim) && (j + jj < dim)){
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red >>5;
				  sum->green += (int) p.green >>5;
				  sum->blue += (int) p.blue >>5;
			}
			ii++;
			iplusii++;
			jj = 01;
			//(2,0) don't need to do anything.

			//(2,1)
			if ((iplusii < dim) && (j + jj < dim)){
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red >> 5;
				  sum->green += (int) p.green >>5;
				  sum->blue += (int) p.blue >>5;
			}
			jj++;
			//(2,2)
			if ((iplusii < dim) && (j + jj < dim)){
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red >>3;
				  sum->green += (int) p.green >> 3;
				  sum->blue += (int) p.blue >> 3;
			}

			current_pixel.red = (unsigned short)sum->red;
		    current_pixel.green = (unsigned short)sum->green;
	        current_pixel.blue = (unsigned short)sum->blue;
			dst[RIDX(i, j, dim)] = current_pixel;

		}
	}
}















char less_naive_motion_descr[] = "less_naive_motion: Less Naive baseline implementation";
void less_naive_motion(int dim, pixel *src, pixel *dst){
	double weights[3][3] = { { 0.50, 0.03125, 0.00 },
	                           { 0.03125, 0.25, 0.03125 },
	                           { 0.00, 0.03125, 0.125 } };
	int i, j;
	for(i = 0; i < dim; i++){
		for(j = 0; j < dim; j++){
			int ii, jj;
			pixel_sum suminst;
			pixel_sum* sum = &suminst;
			pixel current_pixel;
			sum->red = 0;
			sum->green = 0;
			sum->blue = 0;
			//initialize_pixel_sum(&sum);
			ii = 0;
			jj = 0;
			int iplusii = i + ii;
			//int jplusjj = j + jj;
			//pixel * p = src[RIDX(i + ii, j+jj,dim)];
			if ((iplusii < dim) && (j + jj < dim)){
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
			}
			jj++;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}
			jj++;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}
			ii++;
			iplusii++;
			jj = 0;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}
			jj++;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}
			jj++;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}
			ii++;
			iplusii++;
			jj = 0;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}
			jj++;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}
			jj++;
			if ((iplusii < dim) && (j + jj < dim)){
				  double weight = weights[ii][jj];
				  pixel p = src[RIDX(i+ii,j+jj,dim)];
				  sum->red += (int) p.red * weight;
				  sum->green += (int) p.green * weight;
				  sum->blue += (int) p.blue * weight;
				//accumulate_weighted_sum(&sum, src[RIDX(i+ii,j+jj,dim)], weights[ii][jj]);
			}

			current_pixel.red = (unsigned short)sum->red;
		    current_pixel.green = (unsigned short)sum->green;
	        current_pixel.blue = (unsigned short)sum->blue;
			//assign_sum_to_pixel(&current_pixel, sum);
			dst[RIDX(i, j, dim)] = current_pixel;
		}
	}
}
/*
 * naive_motion - The naive baseline version of motion 
 */
char naive_motion_descr[] = "naive_motion: Naive baseline implementation";
void naive_motion(int dim, pixel *src, pixel *dst) 
{
  int i, j;
    
  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(i, j, dim)] = weighted_combo(dim, i, j, src);
}


/*
 * motion - Your current working version of motion. 
 * IMPORTANT: This is the version you will be graded on
 */
char motion_descr[] = "motion: Current working version";
void motion(int dim, pixel *src, pixel *dst) 
{
  even_less_naive_motion(dim, src, dst);
}

/********************************************************************* 
 * register_motion_functions - Register all of your different versions
 *     of the motion kernel with the driver by calling the
 *     add_motion_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_motion_functions() {
  add_motion_function(&motion, motion_descr);
  add_motion_function(&naive_motion, naive_motion_descr);
  add_motion_function(&less_naive_motion,less_naive_motion_descr);
  add_motion_function(&even_less_naive_motion,even_less_naive_motion_descr);
}
