#include<stdio.h>
#include<stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"


int main(int argc, char *argv[])
{
	int Data_str_num;
	Data_str_num = 1;/*�������г�������������������
					��ʾ���У���ʾ���γ���Ĭ��ֻ���� 1�� ���ݣ�*/
	struct Data_str* Data_struct = NULL;/*����� Data_struct ��ָ������һ�������������ݵĽṹ��*/ 
	Data_struct = Create_Data_str(Data_str_num);
	Solve (Data_struct);/*����Solve���� �������Ľ��*/ 
	system("pause");
	return 0;
}
