#include <stdio.h>
#include <stdlib.h>
#include "state.h"

/*
Author: Amoiensis
Email: yxp189@portonmail.com
LINKS: https://github.com/Amoiensis/JD_Logistics_Scheduling
*/

int main(int argc,char** argv)
{
	struct Cost* cost = (struct Cost*)malloc(sizeof(struct Cost));
	int State = Check(cost);
	if (State == 0){
		double Total_cost = Print_Cost (cost);
		printf ("Total_cost = %.2lf",Total_cost);
	}else{
		printf ("\n����ϸ��鲢������Ӧ����\n") ;
	}
	printf ("\n******JD ·�߲���******\n");
	printf ("ע���ɱ�������ܿ��ܴ���һ��ƫ�Լ5%%�����������ƣ�\n");
	printf ("    Ŀǰ�������ڲ���·���Ƿ���С�");
	printf ("\nʹ���������⣬���ͼ��������276003961@qq.com\n") ;
	printf ("****** ��лʹ�ã�*******\n") ;
	system ("pause"); 
	return 0;
}
