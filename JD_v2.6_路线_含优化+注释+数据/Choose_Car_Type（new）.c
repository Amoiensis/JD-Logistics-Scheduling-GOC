#include<stdio.h>
#include<stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"

#define SAFETY_DISTANCE 0 /*注：因为考虑到最后派送车辆时间 不会超过时间限制(24:00)，
								从而可以去除 对于安全剩余里程 的设置*/ 
#define START_TIME 480
#define Test_Route_NUM 2


/************/
int Route_Receiver_IVECO = 0;
int All_Dis_CC_IVECO = 0;
int All_Waittime_CC_IVECO = 0;
int All_Charger_Times_CC_IVECO = 0;
int All_Go_warehouse_Times_CC_IVECO = 0;
int Times_CC_IVECO = 0;
double All_Rweight_CC_IVECO = 0;
double All_Sweight_CC_IVECO = 0;
double All_Rvolume_CC_IVECO = 0;
double All_Svolume_CC_IVECO = 0;
/************/
int Route_Receiver_TRUCK = 0;
int All_Dis_CC_TRUCK = 0;
int All_Waittime_CC_TRUCK = 0;
int All_Charger_Times_CC_TRUCK = 0;
int All_Go_warehouse_Times_CC_TRUCK = 0;
int Times_CC_TRUCK = 0;
double All_Rweight_CC_TRUCK = 0;
double All_Sweight_CC_TRUCK = 0;
double All_Rvolume_CC_TRUCK = 0;
double All_Svolume_CC_TRUCK = 0;
/************/


int choose_car_type(struct Data_str* Data_struct, struct Array_str* Array_Struct)
{
    Clean_Global_Storage_CC ();/*还原所有全局变量 至初始值*/ 
	int Car_type = TRUCK;
	struct Car_route* Test_Car_Route[Test_Route_NUM];
//	struct Time_Node* Test_Node[Test_Route_NUM];
	int Iveco_num = 0, Truck_num = 0;
	double All_cost[Test_Route_NUM] = { 0 };
	double TELL_ONE[Test_Route_NUM] = { 0 };
	Test_Car_Route[IVECO - 1] = Add_New_Car(0, IVECO, START_TIME, Array_Struct);
	Test_Car_Route[TRUCK - 1] = Add_New_Car(0, TRUCK, START_TIME,  Array_Struct);

	Iveco_num = Find_Car_route_CC(Test_Car_Route[IVECO - 1], Data_struct, Array_Struct);
	Free_Test_Route (Test_Car_Route[IVECO - 1]);
	Truck_num = Find_Car_route_CC(Test_Car_Route[TRUCK - 1], Data_struct, Array_Struct);
	Free_Test_Route (Test_Car_Route[TRUCK - 1]);

//	Test_Node[IVECO - 1] = Test_Car_Route[IVECO - 1]->HEAD_Time_Node_point;
//	Test_Node[TRUCK - 1] = Test_Car_Route[TRUCK - 1]->HEAD_Time_Node_point;

	if (Iveco_num != 0 && Truck_num != 0) {
		All_cost[IVECO - 1] = All_Dis_CC_IVECO * 12 / 1000.0 + (All_Waittime_CC_IVECO + All_Charger_Times_CC_IVECO * 30 +
			All_Go_warehouse_Times_CC_IVECO * 60) * 24 / 60.0 + All_Charger_Times_CC_IVECO * 50 + Iveco_num * 200;
		All_cost[TRUCK - 1] = All_Dis_CC_TRUCK * 14 / 1000.0 + (All_Waittime_CC_TRUCK + All_Charger_Times_CC_TRUCK * 30 +
			All_Go_warehouse_Times_CC_TRUCK * 60) * 24 / 60.0 + All_Charger_Times_CC_TRUCK * 50 + Truck_num * 200;
			
		/**********//*该部分 使用特定模型 将不同车型的不同路线 归一化 ， 用于判断选择何种车型*/ 
		TELL_ONE[IVECO - 1] = 10000 * All_cost[IVECO - 1] / /*(All_Dis_CC_IVECO / 10000.0)*/(All_Rweight_CC_IVECO + All_Sweight_CC_IVECO)/(All_Rvolume_CC_IVECO + All_Svolume_CC_IVECO)/ /*(All_Waittime_CC_IVECO / 10.0)*/Route_Receiver_IVECO;
		TELL_ONE[TRUCK - 1] = 10000 * All_cost[TRUCK - 1] / /*(All_Dis_CC_TRUCK / 10000.0)*/(All_Rweight_CC_TRUCK + All_Sweight_CC_TRUCK)/(All_Rvolume_CC_TRUCK+All_Svolume_CC_TRUCK) / /*(All_Waittime_CC_TRUCK / 10.0)*/Route_Receiver_TRUCK;
		/**********/
		 
		if (TELL_ONE[IVECO - 1] < TELL_ONE[TRUCK - 1]) {
			Car_type = IVECO;
		}

	}
	else {
		if (Iveco_num == 0) {
			Car_type = TRUCK;
		}
		else {
			if (Truck_num == 0) {
				Car_type = IVECO;
			}
			else {
				printf("选车函数出现bug,存在无法派车的情况！\n");
				system("pause");
			}
		}
	}

	return Car_type;
}



int Find_Car_route_CC(struct Car_route * Car, struct Data_str* Data_struct, struct Array_str* Array_Struct) /*该函数 类似Find_Car_route函数， 参见'find_route.c'文件*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int ** Time = Array_Struct->Time;
	int Arrived_Time = START_TIME;
	int Next_node_ID = 1, Last_Next_node_ID = 0;
	int Receiver_num = 0;
	int k = 0;
	struct Time_Node* Next_TN = NULL;
	do {
		if (k != 0) {
			Last_Next_node_ID = Next_node_ID;
		}
		Next_node_ID = Find_nextnode(Car, Data_struct, Array_Struct);
		if (k == 0 && Next_node_ID != 0) {
			if ((Node[Next_node_ID]->first_receive_tm - Time[WAREHOUSE][Node[Next_node_ID]->ID]) >= START_TIME) {
				Car->car_now_time = Node[Next_node_ID]->first_receive_tm - Time[WAREHOUSE][Node[Next_node_ID]->ID];
				Car->HEAD_Time_Node_point->Arrived_time = Car->car_now_time;
			}
			else {
				Car->HEAD_Time_Node_point->Arrived_time = START_TIME;
			}
		}
		Next_TN = NtoTN_Transform(Next_node_ID, Array_Struct);
		if (Tell_ID_type(Next_node_ID, Data_struct) == RECEIVER_TYPE) {
			Receiver_num++;
		}
		if (k == 0 && Next_node_ID != WAREHOUSE) {
			Arrive_Node_CC(Car, Next_TN, Data_struct, Array_Struct);
			Next_TN->Array_ID = Next_node_ID;
			Car->now_positon = Next_node_ID;
			k = 1;
		}
		else {
			if (Next_node_ID != WAREHOUSE || Last_Next_node_ID != WAREHOUSE) {
				Car->Time_Node_point->Next_Time_Node = Next_TN;
				Car->Time_Node_point = Next_TN;
				Arrive_Node_CC(Car, Next_TN, Data_struct, Array_Struct);
				Next_TN->Array_ID = Next_node_ID;
				Car->now_positon = Next_node_ID; 
			}
		}
	} while (Next_node_ID != WAREHOUSE || Last_Next_node_ID != WAREHOUSE);
	Car->Time_Node_point->Next_Time_Node = NULL;

	return Receiver_num;
}



void Clean_Global_Storage_CC (void)/*还原所有全局变量 至初始值*/ 
{
	Route_Receiver_IVECO = 0;
     All_Dis_CC_IVECO = 0;
     All_Waittime_CC_IVECO = 0;
     All_Charger_Times_CC_IVECO = 0;
     All_Go_warehouse_Times_CC_IVECO = 0;
     Times_CC_IVECO = 0;
	 All_Rweight_CC_IVECO = 0;
	 All_Sweight_CC_IVECO = 0;
	 All_Rvolume_CC_IVECO = 0;
	 All_Svolume_CC_IVECO = 0;
    /************/
    Route_Receiver_TRUCK = 0;
     All_Dis_CC_TRUCK = 0;
     All_Waittime_CC_TRUCK = 0;
     All_Charger_Times_CC_TRUCK = 0;
     All_Go_warehouse_Times_CC_TRUCK = 0;
     Times_CC_TRUCK = 0;
	 All_Rweight_CC_TRUCK = 0;
	 All_Sweight_CC_TRUCK = 0;
	 All_Rvolume_CC_TRUCK = 0;
	 All_Svolume_CC_TRUCK = 0;
    /************/
}



void Free_Test_Route (struct Car_route * Car)/*消除车型判断时对原始数据（点的经过情况）的影响，并释放路线所占用的内存空间*/ 
{ 
	struct Time_Node* free_point1 = Car->HEAD_Time_Node_point, *free_point2 = NULL;
		while (free_point1) {
			free_point1->Now_node->Tell_pass = 0;
			free_point2 = free_point1->Next_Time_Node;
			free(free_point1);
			free_point1 = free_point2;
		}
		free(Car);
} 



void Arrive_Node_CC(struct Car_route *  Car, struct Time_Node * T_Node, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*用于更改点的属性信息，以表示经过了该点*/ 
{/*注：本函数 类似于'find_route.c'中Arrive_Node函数。
		以后可以考虑 将两个函数合并使用（如：添加一个变量tell：车型判断时取值为1；路线求解时值为0）*/ 
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int Car_type = Car->type;
	if (Car_type == IVECO) {
		Times_CC_IVECO++;
	}
	else {
		Times_CC_TRUCK++;
	}
	Car->Time_Node_point->Next_Time_Node = T_Node;
	Car->Time_Node_point = T_Node;
	int Node_ID = T_Node->Array_ID;
	int Node_Type = 0;
	int Arrived_Time = 0;
	Node_Type = Tell_ID_type(Node_ID, Data_struct);
	if (Car_type == IVECO) {
		All_Dis_CC_IVECO += (Distance[Node[Car->now_positon]->ID][Node[Node_ID]->ID]);
	}
	else {
		All_Dis_CC_TRUCK += (Distance[Node[Car->now_positon]->ID][Node[Node_ID]->ID]);
	}

	switch (Node_Type) {

	case WAREHOUSE_TYPE: {

		if (Car_type == IVECO) {
			All_Go_warehouse_Times_CC_IVECO++;
			Car->pass_warehouse_time++;
			Car->blank_data = (struct Blank_WV_str *)malloc(sizeof(struct Blank_WV_str));
			Car->blank_data->Blank_weight = Car->Blank_weight;
			Car->blank_data->Blank_volume = Car->Blank_volume;
			Car->blank_data->Next_Blank_struct = NULL;
			Car->Blank_weight = I_max_weight;
			Car->Blank_volume = I_max_volume;
			Car->Offset_weight = 0;
			Car->Offset_volume = 0;
			Car->left_mileage = I_driving_range - SAFETY_DISTANCE;
			Arrived_Time = Car->car_now_time + Time[Node[Car->now_positon]->ID][Node[Node_ID]->ID];
			T_Node->Arrived_time = Arrived_Time;
			T_Node->Now_node->Tell_pass = 0;
			Car->car_now_time = Arrived_Time + RESET_TIME;
			Car->now_positon = Node_ID;
		}
		else {
			if (Car_type == TRUCK) {
				All_Go_warehouse_Times_CC_TRUCK++;
				Car->Blank_weight = T_max_weight;
				Car->Blank_volume = T_max_volume;
				Car->left_mileage = T_driving_range - SAFETY_DISTANCE;
				Arrived_Time = Car->car_now_time + Time[Node[Car->now_positon]->ID][Node[Node_ID]->ID];
				T_Node->Arrived_time = Arrived_Time;
				T_Node->Now_node->Tell_pass = 0;
				Car->car_now_time = Arrived_Time + RESET_TIME;
				Car->now_positon = Node_ID;
			}
			else {
				printf("输入Car_type有误！Car_ID:%d\t (Arrive_Node)\n", Car->ID);
				system("pause");
			}
		}
		break;
	}

	case RECEIVER_TYPE: {
		if (Car_type == IVECO) {
			Route_Receiver_IVECO ++;
			All_Rweight_CC_IVECO += Node[Node_ID]->pack_total_weight;
			All_Rvolume_CC_IVECO += Node[Node_ID]->pack_total_volume;
		}
		else {
			Route_Receiver_TRUCK ++;			
			All_Rweight_CC_TRUCK += Node[Node_ID]->pack_total_weight;
			All_Rvolume_CC_TRUCK += Node[Node_ID]->pack_total_volume;
		}
		Car->Blank_weight -= Node[Node_ID]->pack_total_weight;
		Car->Blank_volume -= Node[Node_ID]->pack_total_volume;
		Car->Offset_weight += Node[Node_ID]->pack_total_weight;
		Car->Offset_volume += Node[Node_ID]->pack_total_volume;
		Car->left_mileage -= Distance[Node[Car->now_positon]->ID][Node[Node_ID]->ID];
		Arrived_Time = Car->car_now_time + Time[Node[Car->now_positon]->ID][Node[Node_ID]->ID];
		T_Node->Arrived_time = Arrived_Time;
		T_Node->Now_node->Tell_pass = 1;/*变成2对前面的其他程序不影响，但是IVECO影响TRUCK的选择*/
		if (Arrived_Time < T_Node->Now_node->first_receive_tm) {
			if (Car_type == IVECO) {
				All_Waittime_CC_IVECO += (T_Node->Now_node->first_receive_tm - Arrived_Time);
			}
			else {
				All_Waittime_CC_TRUCK += (T_Node->Now_node->first_receive_tm - Arrived_Time);
			}
			Car->car_now_time = UNLOAD_TIME + Node[Node_ID]->first_receive_tm;
		}
		else {
			if (Arrived_Time > T_Node->Now_node->last_receive_tm) {
				printf("车辆迟到，请检查！");
			}
			else {
				Car->car_now_time = UNLOAD_TIME + Arrived_Time;
			}
		}
		Car->now_positon = Node_ID;
		break;
	}

	case SENDER_TYPE: {
		if (Car_type == IVECO) {
			Route_Receiver_IVECO++;
			All_Sweight_CC_IVECO += Node[Node_ID]->pack_total_weight;
			All_Svolume_CC_IVECO += Node[Node_ID]->pack_total_volume;
		}
		else {
			if (Car_type == TRUCK) {
			Route_Receiver_TRUCK++;
			All_Sweight_CC_TRUCK += Node[Node_ID]->pack_total_weight;
			All_Svolume_CC_TRUCK += Node[Node_ID]->pack_total_volume;
			}
			else
			{
				printf("Car_type=%d出错！请检查Arrive_Node_CC！！！", Car_type);
			}

		}
		if (Node[Node_ID]->pack_total_weight <= Car->Offset_weight && Node[Node_ID]->pack_total_volume <= Car->Offset_volume) {
			Car->Offset_weight -= Node[Node_ID]->pack_total_weight;
			Car->Offset_volume -= Node[Node_ID]->pack_total_volume;
		}
		else {
			if (Node[Node_ID]->pack_total_weight > Car->Offset_weight) {
				Car->Blank_weight += (Node[Node_ID]->pack_total_weight - Car->Offset_weight);
				Car->Offset_weight = 0;
			}
			else {
				Car->Offset_weight -= Node[Node_ID]->pack_total_weight;
			}
			if (Node[Node_ID]->pack_total_volume > Car->Offset_volume) {
				Car->Blank_volume += (Node[Node_ID]->pack_total_volume - Car->Offset_volume);
				Car->Offset_volume = 0;
			}
			else {
				Car->Offset_volume -= Node[Node_ID]->pack_total_volume;
			}
		}
		Car->left_mileage -= Distance[Node[Car->now_positon]->ID][Node[Node_ID]->ID];
		Arrived_Time = Car->car_now_time + Time[Node[Car->now_positon]->ID][Node[Node_ID]->ID];
		T_Node->Arrived_time = Arrived_Time;
		T_Node->Now_node->Tell_pass = 1;/*变成2对前面的其他程序不影响，但是IVECO影响TRUCK的选择*/
		if (Arrived_Time < T_Node->Now_node->first_receive_tm) {
			if (Car_type == IVECO) {
				All_Waittime_CC_IVECO += (T_Node->Now_node->first_receive_tm - Arrived_Time);
			}
			else {
				All_Waittime_CC_TRUCK += (T_Node->Now_node->first_receive_tm - Arrived_Time);
			}
			Car->car_now_time = UNLOAD_TIME + Node[Node_ID]->first_receive_tm;
		}
		else {
			if (Arrived_Time > T_Node->Now_node->last_receive_tm) {
				printf("车辆迟到，请检查！");
			}
			else {
				Car->car_now_time = UNLOAD_TIME + Arrived_Time;
			}
		}
		Car->now_positon = Node_ID;
		break;
	}

	case CHARGER_TYPE: {
		if (Car_type == IVECO) {
			All_Charger_Times_CC_IVECO++;
		}
		else {
			All_Charger_Times_CC_TRUCK++;
		}
		Arrived_Time = Car->car_now_time + Time[Node[Car->now_positon]->ID][Node[Node_ID]->ID];
		T_Node->Arrived_time = Arrived_Time;
		T_Node->Now_node->Tell_pass = 0;
		Car->car_now_time = Arrived_Time + CHARGE_TIME;
		if (Car->type == IVECO) {
			Car->left_mileage = I_driving_range - SAFETY_DISTANCE;
		}
		else {
			if (Car->type == TRUCK) {
				Car->left_mileage = T_driving_range - SAFETY_DISTANCE;
			}
			else {
				printf("输入Car_type有误！Car_ID:%d\t (Arrive_Node)\n", Car->ID);
				system("pause");
			}
		}
		Car->now_positon = Node_ID;
		break;
	}
	default: {
		printf("(Arrive_Node) 出错！当前Car ID：%d\n当前 前往位置Node-ID:%d\n", Car->ID, Node_ID);
		system("pause");
		break;
	}
	}
}
