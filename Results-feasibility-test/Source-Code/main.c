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
		printf ("\n请仔细检查并修正相应错误！\n") ;
	}
	printf ("\n******JD 路线测试******\n");
	printf ("注：成本计算可能可能存在一定偏差（约5%%），正在完善！\n");
	printf ("    目前可以用于测试路线是否可行。");
	printf ("\n使用遇到问题，请截图发送至：Amoiensis@outlook.com\n") ;
	printf ("****** 感谢使用！*******\n") ;
	system ("pause"); 
	return 0;
}
