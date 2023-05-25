#include "TDOA.h"
#include "exti.h"
#include "usart.h"
#include "stdio.h"
#include "math.h"
_TDOA TDOA_S;
float x1,y1;
void TDOA_Init(_TDOA *TDOA_S)
{
	TDOA_S->L=90;
	TDOA_S->r31=0;
	TDOA_S->r42=0;
	TDOA_S->r13=0;
	TDOA_S->r24=0;
	TDOA_S->r32=0;
	TDOA_S->r23=0;
	TDOA_S->r14=0;
	TDOA_S->r41=0;
	TDOA_S->r12=0;
	TDOA_S->r21=0;
	TDOA_S->r34=0;
	TDOA_S->r43=0;
	TDOA_S->c1=0;
	TDOA_S->c2=0;
	TDOA_S-> b1=0;
	TDOA_S-> b2=0;
	TDOA_S->a1=0;
	TDOA_S->a2=0;
	
}

//PA1,PA2,PA3,PA4分别为第一，二，三，四象限

void TDOA_Get_distanceANDangle(_TDOA *TDOA_S)
{
	TDOA_S->r21=(MIT2_S.mit_count-MIT1_S.mit_count)*0.34;//2和1的时间差
	TDOA_S->r31=(MIT3_S.mit_count-MIT1_S.mit_count)*0.34;//3和1的时间差
	TDOA_S->r41=(MIT4_S.mit_count-MIT1_S.mit_count)*0.34;//4和1的时间差
	TDOA_S->r42=(MIT4_S.mit_count-MIT2_S.mit_count)*0.34;//4和2的时间差
	TDOA_S->r12=(MIT1_S.mit_count-MIT2_S.mit_count)*0.34;//1和2的时间差
	TDOA_S->r13=(MIT1_S.mit_count-MIT3_S.mit_count)*0.34;//1和3的时间差
	TDOA_S->r14=(MIT1_S.mit_count-MIT4_S.mit_count)*0.34;//1和4的时间差
	TDOA_S->r24=(MIT2_S.mit_count-MIT4_S.mit_count)*0.34;//2和4的时间差
	TDOA_S->r23=(MIT2_S.mit_count-MIT3_S.mit_count)*0.34;//2和3的时间差
	TDOA_S->r32=(MIT3_S.mit_count-MIT2_S.mit_count)*0.34;//3和2的时间差
	TDOA_S->r34=(MIT3_S.mit_count-MIT4_S.mit_count)*0.34;//3和4的时间差
	TDOA_S->r43=(MIT4_S.mit_count-MIT3_S.mit_count)*0.34;//4和3的时间差

	
	TDOA_S->c1 = 90 * pow(2.0, 0.5);//对角线的长度
	TDOA_S->c2 = 90 * pow(2.0, 0.5);//对角线的长度

	TDOA_S->a1=fabs((MIT3_S.mit_count-MIT1_S.mit_count)*0.34/2);//双曲线a1的长度
	TDOA_S->a2=fabs((MIT4_S.mit_count-MIT2_S.mit_count)*0.34/2);//双曲线a2的长度
	
	TDOA_S->b1 = pow(pow(TDOA_S->c1, 2.0) - pow(TDOA_S->a1, 2.0), 0.5);//双曲线b1的长度
	TDOA_S->b2 = pow(pow(TDOA_S->c2, 2.0) - pow(TDOA_S->a2, 2.0), 0.5);//双曲线b2的长度
	
	//求得x的平方与y的平方
	TDOA_S->dx = pow(TDOA_S->a1, 2.0) * pow(TDOA_S->b2, 2.0) * (pow(TDOA_S->a2, 2.0) + pow(TDOA_S->b1, 2.0)) / 
	((pow(TDOA_S->b1, 2.0) * pow(TDOA_S->b2, 2.0) - pow(TDOA_S->a1, 2.0) * pow(TDOA_S->a2, 2.0)));
	
	TDOA_S->dy = pow(TDOA_S->a2, 2.0) * pow(TDOA_S->b1, 2.0) * (pow(TDOA_S->a1, 2.0) + pow(TDOA_S->b2, 2.0)) /
	((pow(TDOA_S->b1, 2.0) * pow(TDOA_S->b2, 2.0) - pow(TDOA_S->a1, 2.0) * pow(TDOA_S->a2, 2.0)));

	
	/*根据时间差对x，y进行正负判断*/
	
	//第一象限
    if(TDOA_S->r31>0&&TDOA_S->r42>0)
{
	TDOA_S->dx=pow(TDOA_S->dx,0.5);
	TDOA_S->dy=pow(TDOA_S->dy,0.5);
	
}
	//第二象限
	else if(TDOA_S->r31<0&&TDOA_S->r42>0)
{
	TDOA_S->dx=-pow(TDOA_S->dx,0.5);
	TDOA_S->dy=pow(TDOA_S->dy,0.5);
	
}
	//第三象限
	else if(TDOA_S->r31>0&&TDOA_S->r42<0)
{
	TDOA_S->dx=pow(TDOA_S->dx,0.5);
	TDOA_S->dy=-pow(TDOA_S->dy,0.5);
	
}
	//第四象限
	else if(TDOA_S->r31<0&&TDOA_S->r42<0)
{
	TDOA_S->dy=-pow(TDOA_S->dy,0.5);
	TDOA_S->dx=-pow(TDOA_S->dx,0.5);
}
	//转换坐标
    
	if(TDOA_S->b1*TDOA_S->b2>TDOA_S->a1*TDOA_S->a2)//该条件下有解
{
		TDOA_S->y = pow(2, 0.5)/2*(TDOA_S->dx+TDOA_S->dy);
		TDOA_S->x = pow(2, 0.5)/2*(TDOA_S->dx-TDOA_S->dy);
}
		
	//旋转后的坐标系
	if(TDOA_S->r12<0&&TDOA_S->r13<0&&TDOA_S->r14<0)//一象限
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_5,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_5,GPIO_PIN_RESET);
	}
	else if(TDOA_S->r21<0&&TDOA_S->r23<0&&TDOA_S->r24<0)//二象限
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_RESET);
	}
	else if(TDOA_S->r31<0&&TDOA_S->r32<0&&TDOA_S->r34<0)//三象限
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_3,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_3,GPIO_PIN_RESET);
	}
	else if(TDOA_S->r41<0&&TDOA_S->r42<0&&TDOA_S->r43<0)//四象限
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_2,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_2,GPIO_PIN_RESET);
	}
	else if(fabs(TDOA_S->r31)<=10&&fabs(TDOA_S->r24)<=10)//原点
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_PIN_RESET);
	}
	
	/*printf("r21:%lf\r\n",TDOA_S->r21);
	
	printf("r31:%lf\r\n",TDOA_S->r31);
	printf("r41:%lf\r\n",TDOA_S->r41);
	printf("r42:%lf\r\n",TDOA_S->r42);*/
	//printf("a1:%lf\r\n",TDOA_S->a1);
	printf("x:%.2lf\r\n",TDOA_S->x);
	printf("y:%.2lf\r\n",TDOA_S->y);
	//printf("x1:%lf\r\n",x1);
	//printf("y1:%lf\r\n",y1);
	//printf("c1:%lf\r\n",TDOA_S->c1);
	//printf("a1:%lf\r\n",TDOA_S->a1);
	//printf("a1:%lf\r\n",TDOA_S->a2);*/

	//printf("b1:%lf\r\n",TDOA_S->b1);
	//printf("b2:%lf\r\n",TDOA_S->b2);
	printf("t42:%d\r\n",MIT4_S.mit_count-MIT2_S.mit_count);
	printf("t31:%d\r\n",MIT3_S.mit_count-MIT1_S.mit_count);
	printf("t41:%d\r\n",MIT4_S.mit_count-MIT1_S.mit_count);
	printf("t32:%d\r\n",MIT3_S.mit_count-MIT2_S.mit_count);
	printf("t4:%d\r\n\n",MIT4_S.mit_count);
	printf("t3:%d\r\n",MIT3_S.mit_count);
	printf("t1:%d\r\n",MIT1_S.mit_count);
	printf("t2:%d\r\n\n",MIT2_S.mit_count);
	printf("t:%d  %d  %d  %d\r\n",MIT4_S.mit_count,MIT3_S.mit_count,MIT2_S.mit_count,MIT1_S.mit_count);
}















