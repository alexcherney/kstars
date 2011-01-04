/*
 * Convert an image from yuv colourspace to rgb 
 *
 * Code by Tony Hague (C) 2001.
 */

#include "ccvt.h"
#include "ccvt_types.h"

/* by suitable definition of PIXTYPE, can do yuv to rgb or bgr, with or
without word tqalignment */

/* This doesn't exactly earn a prize in a programming beauty contest. */

#define WHOLE_FUNC2RGB(type) 			\
	const unsigned char *y1, *y2, *u, *v; 	\
	PIXTYPE_##type *l1, *l2;		\
	int r, g, b, cr, cg, cb, yp, j, i;	\
						\
	if ((width & 1) || (height & 1))	\
		return;				\
						\
	l1 = (PIXTYPE_##type *)dst;		\
	l2 = l1 + width;			\
	y1 = (unsigned char *)src;		\
	y2 = y1 + width;			\
	u = (unsigned char *)src + width * height;		\
	v = u + (width * height) / 4;		\
	j = height / 2;				\
	while (j--) {				\
		i = width / 2;			\
		while (i--) {			\
			/* Since U & V are valid for 4 pixels, repeat code 4 	\
			   times for different Y */				\
			cb = ((*u-128) * 454)>>8;				\
			cr = ((*v-128) * 359)>>8;				\
			cg = ((*v-128) * 183 + (*u-128) * 88)>>8;		\
						\
			yp = *(y1++);		\
			r = yp + cr; 		\
			b = yp + cb;		\
			g = yp - cg;            \
			SAT(r);                 \
			SAT(g);                 \
			SAT(b);                 \
			l1->b = b;		\
			l1->g = g;              \
			l1->r = r;              \
			l1++;                   \
                                                \
			yp = *(y1++);           \
			r = yp + cr;            \
			b = yp + cb;            \
			g = yp - cg;            \
			SAT(r);                 \
			SAT(g);                 \
			SAT(b);                 \
			l1->b = b;		\
			l1->g = g;		\
			l1->r = r;		\
			l1++;			\
						\
			yp = *(y2++);		\
			r = yp + cr; 		\
			b = yp + cb;		\
			g = yp - cg;		\
			SAT(r);			\
			SAT(g);			\
			SAT(b);			\
			l2->b = b;		\
			l2->g = g;		\
			l2->r = r;		\
			l2++;			\
						\
			yp = *(y2++);		\
			r = yp + cr; 		\
			b = yp + cb;		\
			g = yp - cg;		\
			SAT(r);			\
			SAT(g);			\
			SAT(b);			\
			l2->b = b;		\
			l2->g = g;		\
			l2->r = r;		\
			l2++;			\
						\
			u++;			\
			v++;			\
		}				\
		y1 = y2;			\
		y2 += width;			\
		l1 = l2;			\
		l2 += width;			\
	}




void ccvt_420p_bgr32(int width, int height, const void *src, void *dst)
{
	WHOLE_FUNC2RGB(bgr32)
}

void ccvt_420p_bgr24(int width, int height, const void *src, void *dst)
{
	WHOLE_FUNC2RGB(bgr24)
}

void ccvt_420p_rgb32(int width, int height, const void *src, void *dst)
{
	WHOLE_FUNC2RGB(rgb32)
}

void ccvt_420p_rgb24(int width, int height, const void *src, void *dst)
{
	WHOLE_FUNC2RGB(rgb24)
}

