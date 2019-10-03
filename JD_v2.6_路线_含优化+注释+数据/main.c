#include<stdio.h>
#include<stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"


int main(int argc, char *argv[])
{
	int Data_str_num;
	Data_str_num = 1;/*本次运行程序待处理的数据组数；
					（示例中，表示本次程序默认只处理 1组 数据）*/
	struct Data_str* Data_struct = NULL;/*这里的 Data_struct 是指建立的一个包含所有数据的结构体*/ 
	Data_struct = Create_Data_str(Data_str_num);
	Solve (Data_struct);/*调用Solve函数 完成问题的解决*/ 
	system("pause");
	return 0;
}
