#include<stdio.h>
#include<stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"



struct Data_str* Create_Data_str(int Data_str_num)/*这里未来可以改成从txt文档中读取的模式，更加便捷和分块明确/* 
												  /*在txt文档标准化 命名为 如："inputdistancetime_0_1101.txt" 和 "inputnode_0_1101"*/
{
	struct Data_str* Data_struct = (struct Data_str*)malloc(sizeof(struct Data_str));
	Data_struct->Data_ID = 5;/*数据组的序号*/ 
	Data_struct->Receiver_num = 900;/*待处理点中的 收货者的数目*/ 
	Data_struct->Sender_num = 200;/*待处理点中的 发货者的数目*/ 
	Data_struct->Charger_num = 100;/*待处理点中的 充电桩的数目*/ 
	return Data_struct;
}



struct Array_str* Create_Array_str(struct Data_str* Data_struct)/*用于生成 储存以下信息的结构体。   1.每一个点所有的属性
																									2.点到点间距离、时间
																									3.求解得到的路线结果*/ 
{
	int All_Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	int Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	struct Array_str* Array_Str = (struct Array_str*)malloc(sizeof(struct Array_str));
	Array_Str->Node = (struct str_node**)malloc(All_Node_num * sizeof(struct str_node*));/*储存 点的属性 的内存空间*/ 
	int ** Array = (int**)malloc(All_Node_num * sizeof(int*));
	int i;
	for (i = 0; i<All_Node_num; i++) {
		Array[i] = (int*)malloc(All_Node_num * sizeof(int));
	}
	Array_Str->Distance = Array;/*储存 距离信息 的内存空间*/ 
	Array = (int**)malloc(All_Node_num * sizeof(int*));
	for (i = 0; i<All_Node_num; i++) {
		Array[i] = (int*)malloc(All_Node_num * sizeof(int));
	}
	Array_Str->Time = Array;/*储存 时间信息 的内存空间*/ 
	Array_Str->Route_List = (struct Car_route**)malloc(Receiver_Sender_num * sizeof(struct Car_route*));/*储存 路线结果 的内存空间*/ 
	return Array_Str;
}



void Solve(struct Data_str* Data_struct)/*这里将整个的解决过程独立作为一个函数模块，使得用户不用接触具体的内部函数*/
{
	int Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num;
	int All_car_num = 0;
	struct Array_str* Array_struct = Create_Array_str(Data_struct);/*为外部数据读入 和 最终结果写入 开辟内存空间（为 Array_struct 获得内存空间）*/ 
	fpRead(Data_struct, Array_struct);/*从外部文件 读入各点的属性*/ 
	fpRead_Dis_tm(Data_struct, Array_struct);/*从外部文件读入 点间的时间距离信息*/ 
	All_car_num = Find_MapRoute(Data_struct, Array_struct);/*调用 Find_MapRoute 求解初始结果路线 ；并将结果写入了 Array_Str->Route_List 内*/ 
	
	printf("@@上述输出为初始解；下面将开始优化。@@"); 
	system("pause");
 
/**************************/ /*分隔出的内容 用于完成 初始结果的优化（目前使用将一些路线结合 再求解的方法优化）*/ 
	struct Car_route** Route_List =  Array_struct->Route_List; 
	int j;
	for (j = 0;j<10;j++){
		
	int Car_num_Route_List = 0;
	while (Route_List[Car_num_Route_List]) {
		Car_num_Route_List++;
	}
	
	int i,B[2] ={0};
	int tell_parity = Car_num_Route_List%2;/*判断奇偶性，为0为偶数，为1为基数*/
	int half = All_car_num/2;
	if (tell_parity == 1){
		for (i = 1;i <= half;i++){/*最中间的那一条路不处理*/
			B[0] = (All_car_num - i);
			B[1] = (half - i + 1);/*给的是路线的序号（从0开始到All_car_num-1）*/
			int DoneorNot = Optimization (Data_struct, Array_struct,B);/*0表示没有优化，1表示已经优化*/	
				if (DoneorNot){
					printf("已经优化！\n\n");
				}else{
					printf ("没有优化！\n\n");
				}
		}		
	} else{
		for (i = 1;i<=half;i++){
			B[0] = (All_car_num-i);
			B[1] = (half - i);/*给的是路线的序号（从0开始到All_car_num-1）*/
			int DoneorNot = Optimization (Data_struct, Array_struct,B);/*0表示没有优化，1表示已经优化*/
				if (DoneorNot){
					printf("已经优化！\n\n");
				}else{
					printf ("没有优化！\n\n");
				}
		}
	}
	
}
/**************************/ 
	
	final_check(Data_struct, Array_struct);/*检查所得路线的可行性*/ 
	
	Clean_Node_str(Data_struct, Array_struct); /*Clean_*** 的函数均用于释放相应 内存空间*/ 
	Clean_Array_str_Time_Dis(Data_struct, Array_struct);/*时间 距离信息*/ 
	Clean_MapRoute(All_car_num, Array_struct);/*路线结果*/ 
	solution_write(Data_struct);/*将路线结果标准化打印出来*/ 
	Clean_Check_Area();/*检查结果可行性时 开辟的空间*/ 
}



void Clean_Array_str_Time_Dis(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*清理函数对于点和路径的清理可以留
																						   交给find_route函数中的Clean部分去清理，只要传递相关的结构体*/
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
