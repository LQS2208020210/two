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

//PA1,PA2,PA3,PA4�ֱ�Ϊ��һ����������������

void TDOA_Get_distanceANDangle(_TDOA *TDOA_S)
{
	TDOA_S->r21=(MIT2_S.mit_count-MIT1_S.mit_count)*0.34;//2��1��ʱ���
	TDOA_S->r31=(MIT3_S.mit_count-MIT1_S.mit_count)*0.34;//3��1��ʱ���
	TDOA_S->r41=(MIT4_S.mit_count-MIT1_S.mit_count)*0.34;//4��1��ʱ���
	TDOA_S->r42=(MIT4_S.mit_count-MIT2_S.mit_count)*0.34;//4��2��ʱ���
	TDOA_S->r12=(MIT1_S.mit_count-MIT2_S.mit_count)*0.34;//1��2��ʱ���
	TDOA_S->r13=(MIT1_S.mit_count-MIT3_S.mit_count)*0.34;//1��3��ʱ���
	TDOA_S->r14=(MIT1_S.mit_count-MIT4_S.mit_count)*0.34;//1��4��ʱ���
	TDOA_S->r24=(MIT2_S.mit_count-MIT4_S.mit_count)*0.34;//2��4��ʱ���
	TDOA_S->r23=(MIT2_S.mit_count-MIT3_S.mit_count)*0.34;//2��3��ʱ���
	TDOA_S->r32=(MIT3_S.mit_count-MIT2_S.mit_count)*0.34;//3��2��ʱ���
	TDOA_S->r34=(MIT3_S.mit_count-MIT4_S.mit_count)*0.34;//3��4��ʱ���
	TDOA_S->r43=(MIT4_S.mit_count-MIT3_S.mit_count)*0.34;//4��3��ʱ���

	
	TDOA_S->c1 = 90 * pow(2.0, 0.5);//�Խ��ߵĳ���
	TDOA_S->c2 = 90 * pow(2.0, 0.5);//�Խ��ߵĳ���

	TDOA_S->a1=fabs((MIT3_S.mit_count-MIT1_S.mit_count)*0.34/2);//˫����a1�ĳ���
	TDOA_S->a2=fabs((MIT4_S.mit_count-MIT2_S.mit_count)*0.34/2);//˫����a2�ĳ���
	
	TDOA_S->b1 = pow(pow(TDOA_S->c1, 2.0) - pow(TDOA_S->a1, 2.0), 0.5);//˫����b1�ĳ���
	TDOA_S->b2 = pow(pow(TDOA_S->c2, 2.0) - pow(TDOA_S->a2, 2.0), 0.5);//˫����b2�ĳ���
	
	//���x��ƽ����y��ƽ��
	TDOA_S->dx = pow(TDOA_S->a1, 2.0) * pow(TDOA_S->b2, 2.0) * (pow(TDOA_S->a2, 2.0) + pow(TDOA_S->b1, 2.0)) / 
	((pow(TDOA_S->b1, 2.0) * pow(TDOA_S->b2, 2.0) - pow(TDOA_S->a1, 2.0) * pow(TDOA_S->a2, 2.0)));
	
	TDOA_S->dy = pow(TDOA_S->a2, 2.0) * pow(TDOA_S->b1, 2.0) * (pow(TDOA_S->a1, 2.0) + pow(TDOA_S->b2, 2.0)) /
	((pow(TDOA_S->b1, 2.0) * pow(TDOA_S->b2, 2.0) - pow(TDOA_S->a1, 2.0) * pow(TDOA_S->a2, 2.0)));

	
	/*����ʱ����x��y���������ж�*/
	
	//��һ����
    if(TDOA_S->r31>0&&TDOA_S->r42>0)
{
	TDOA_S->dx=pow(TDOA_S->dx,0.5);
	TDOA_S->dy=pow(TDOA_S->dy,0.5);
	
}
	//�ڶ�����
	else if(TDOA_S->r31<0&&TDOA_S->r42>0)
{
	TDOA_S->dx=-pow(TDOA_S->dx,0.5);
	TDOA_S->dy=pow(TDOA_S->dy,0.5);
	
}
	//��������
	else if(TDOA_S->r31>0&&TDOA_S->r42<0)
{
	TDOA_S->dx=pow(TDOA_S->dx,0.5);
	TDOA_S->dy=-pow(TDOA_S->dy,0.5);
	
}
	//��������
	else if(TDOA_S->r31<0&&TDOA_S->r42<0)
{
	TDOA_S->dy=-pow(TDOA_S->dy,0.5);
	TDOA_S->dx=-pow(TDOA_S->dx,0.5);
}
	//ת������
    
	if(TDOA_S->b1*TDOA_S->b2>TDOA_S->a1*TDOA_S->a2)//���������н�
{
		TDOA_S->y = pow(2, 0.5)/2*(TDOA_S->dx+TDOA_S->dy);
		TDOA_S->x = pow(2, 0.5)/2*(TDOA_S->dx-TDOA_S->dy);
}
		
	//��ת�������ϵ
	if(TDOA_S->r12<0&&TDOA_S->r13<0&&TDOA_S->r14<0)//һ����
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_5,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_5,GPIO_PIN_RESET);
	}
	else if(TDOA_S->r21<0&&TDOA_S->r23<0&&TDOA_S->r24<0)//������
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_RESET);
	}
	else if(TDOA_S->r31<0&&TDOA_S->r32<0&&TDOA_S->r34<0)//������
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_3,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_3,GPIO_PIN_RESET);
	}
	else if(TDOA_S->r41<0&&TDOA_S->r42<0&&TDOA_S->r43<0)//������
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_2,GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_2,GPIO_PIN_RESET);
	}
	else if(fabs(TDOA_S->r31)<=10&&fabs(TDOA_S->r24)<=10)//ԭ��
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















