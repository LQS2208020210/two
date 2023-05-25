#ifndef __TDOA_H
#define	__TDOA_H
#include <math.h>

typedef struct
{
	double x;
	double y;
	double r21;
	double r31;
	double r41;
	double r12;
	double r13;
	double r14;
	double r32;
	double r23;
	double r42;
	double r24;
	double r43;
	double r34;
	double a1;
	double a2;
	double L;
	
	double c1;
	double c2;
	double b1;
	double b2;
	double dx;
	double dy;
	double q;
	double p;

	

}_TDOA;


extern _TDOA TDOA_S;
void TDOA_Init(_TDOA *TDOA_S);
void TDOA_Get_distanceANDangle(_TDOA *TDOA_S);

#endif
