#include<stdio.h>
#include<stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"



struct Data_str* Create_Data_str(int Data_str_num)/*����δ�����Ըĳɴ�txt�ĵ��ж�ȡ��ģʽ�����ӱ�ݺͷֿ���ȷ/* 
												  /*��txt�ĵ���׼�� ����Ϊ �磺"inputdistancetime_0_1101.txt" �� "inputnode_0_1101"*/
{
	struct Data_str* Data_struct = (struct Data_str*)malloc(sizeof(struct Data_str));
	Data_struct->Data_ID = 5;/*����������*/ 
	Data_struct->Receiver_num = 900;/*��������е� �ջ��ߵ���Ŀ*/ 
	Data_struct->Sender_num = 200;/*��������е� �����ߵ���Ŀ*/ 
	Data_struct->Charger_num = 100;/*��������е� ���׮����Ŀ*/ 
	return Data_struct;
}



struct Array_str* Create_Array_str(struct Data_str* Data_struct)/*�������� ����������Ϣ�Ľṹ�塣   1.ÿһ�������е�����
																									2.�㵽�����롢ʱ��
																									3.���õ���·�߽��*/ 
{
	int All_Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	int Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	struct Array_str* Array_Str = (struct Array_str*)malloc(sizeof(struct Array_str));
	Array_Str->Node = (struct str_node**)malloc(All_Node_num * sizeof(struct str_node*));/*���� ������� ���ڴ�ռ�*/ 
	int ** Array = (int**)malloc(All_Node_num * sizeof(int*));
	int i;
	for (i = 0; i<All_Node_num; i++) {
		Array[i] = (int*)malloc(All_Node_num * sizeof(int));
	}
	Array_Str->Distance = Array;/*���� ������Ϣ ���ڴ�ռ�*/ 
	Array = (int**)malloc(All_Node_num * sizeof(int*));
	for (i = 0; i<All_Node_num; i++) {
		Array[i] = (int*)malloc(All_Node_num * sizeof(int));
	}
	Array_Str->Time = Array;/*���� ʱ����Ϣ ���ڴ�ռ�*/ 
	Array_Str->Route_List = (struct Car_route**)malloc(Receiver_Sender_num * sizeof(struct Car_route*));/*���� ·�߽�� ���ڴ�ռ�*/ 
	return Array_Str;
}



void Solve(struct Data_str* Data_struct)/*���ｫ�����Ľ�����̶�����Ϊһ������ģ�飬ʹ���û����ýӴ�������ڲ�����*/
{
	int Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num;
	int All_car_num = 0;
	struct Array_str* Array_struct = Create_Array_str(Data_struct);/*Ϊ�ⲿ���ݶ��� �� ���ս��д�� �����ڴ�ռ䣨Ϊ Array_struct ����ڴ�ռ䣩*/ 
	fpRead(Data_struct, Array_struct);/*���ⲿ�ļ� ������������*/ 
	fpRead_Dis_tm(Data_struct, Array_struct);/*���ⲿ�ļ����� ����ʱ�������Ϣ*/ 
	All_car_num = Find_MapRoute(Data_struct, Array_struct);/*���� Find_MapRoute ����ʼ���·�� ���������д���� Array_Str->Route_List ��*/ 
	
	printf("@@�������Ϊ��ʼ�⣻���潫��ʼ�Ż���@@"); 
	system("pause");
 
/**************************/ /*�ָ��������� ������� ��ʼ������Ż���Ŀǰʹ�ý�һЩ·�߽�� �����ķ����Ż���*/ 
	struct Car_route** Route_List =  Array_struct->Route_List; 
	int j;
	for (j = 0;j<10;j++){
		
	int Car_num_Route_List = 0;
	while (Route_List[Car_num_Route_List]) {
		Car_num_Route_List++;
	}
	
	int i,B[2] ={0};
	int tell_parity = Car_num_Route_List%2;/*�ж���ż�ԣ�Ϊ0Ϊż����Ϊ1Ϊ����*/
	int half = All_car_num/2;
	if (tell_parity == 1){
		for (i = 1;i <= half;i++){/*���м����һ��·������*/
			B[0] = (All_car_num - i);
			B[1] = (half - i + 1);/*������·�ߵ���ţ���0��ʼ��All_car_num-1��*/
			int DoneorNot = Optimization (Data_struct, Array_struct,B);/*0��ʾû���Ż���1��ʾ�Ѿ��Ż�*/	
				if (DoneorNot){
					printf("�Ѿ��Ż���\n\n");
				}else{
					printf ("û���Ż���\n\n");
				}
		}		
	} else{
		for (i = 1;i<=half;i++){
			B[0] = (All_car_num-i);
			B[1] = (half - i);/*������·�ߵ���ţ���0��ʼ��All_car_num-1��*/
			int DoneorNot = Optimization (Data_struct, Array_struct,B);/*0��ʾû���Ż���1��ʾ�Ѿ��Ż�*/
				if (DoneorNot){
					printf("�Ѿ��Ż���\n\n");
				}else{
					printf ("û���Ż���\n\n");
				}
		}
	}
	
}
/**************************/ 
	
	final_check(Data_struct, Array_struct);/*�������·�ߵĿ�����*/ 
	
	Clean_Node_str(Data_struct, Array_struct); /*Clean_*** �ĺ����������ͷ���Ӧ �ڴ�ռ�*/ 
	Clean_Array_str_Time_Dis(Data_struct, Array_struct);/*ʱ�� ������Ϣ*/ 
	Clean_MapRoute(All_car_num, Array_struct);/*·�߽��*/ 
	solution_write(Data_struct);/*��·�߽����׼����ӡ����*/ 
	Clean_Check_Area();/*�����������ʱ ���ٵĿռ�*/ 
}



void Clean_Array_str_Time_Dis(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*���������ڵ��·�������������
																						   ����find_route�����е�Clean����ȥ����ֻҪ������صĽṹ��*/
{
	int All_Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	int Reciver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	int ** Array_Dis = Array_Struct->Distance, **Array_Time = Array_Struct->Time;
	int i;
	for (i = 0; i<All_Node_num; i++) {
		free(Array_Dis[i]);
		free(Array_Time[i]);
	}
}
