#include <stdio.h>
#include <stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"

#define SAFETY_DISTANCE 0 /*注：因为考虑到最后派送车辆时间 不会超过时间限制(24:00)，
								从而可以去除 对于安全剩余里程 的设置*/ /*（14000 //8450 //15030 以上为安全的剩余距离的经验值）*/ 
#define SAFETY_LEFT_TIME 117 /*安全的剩余时间，防止车辆最后回仓库晚于时间限制(24:00)；
								注：到目前为之还没有出现车辆配送时间超限的问题*/
#define TtoD_factor 447   /*时间转换到距离的放大倍数，经验值*/
						      /*注：447//350//477 为各次的经验值；
							  		意义：将时间数值转化成为和距离一个数量级使用的放大倍数；
									得到方式：用总的距离的平均值和总的时间的平均值做比*/
#define T_chance 3.9  /*权重判断的经验值；时间参数的系数；*/
						/*3.9//2.4//2.04 为各次经验值； 
						得到方式：权值现在采用遍历搜索得到，期待未来采用数学方法计算得到*/
#define D_chance 0.7  /*权重判断的经验值；距离参数的系数；*/ 
						/*0.7 //1-0.7 为各次经验值；
						得到方式：权重判断的经验值；现在使用的是遍历搜索得到，期待未来采用计算的数学方法得到*/
#define Distance_rate 2800  /*2800 //2000 //1-2800//1600 为各次经验值；*/ 
                            /*这里是将选择和已经跑过的点挂钩，因为越到后面成本越来越高，所以现在使用一个线性关系尝试拟合。
                              现在用来绘图寻找，二者相关性的图标也是用一次线性关系*/


/*不用理会这句话（这里在加油站可以移动之后，应该不放弃这些参数的正确性，要适当的扣除这些已累计的部分）*/
int All_Passed_Receiver = 0;/*现在已经经过的收货者*/
int All_Passed_Sender = 0;/*现在已经经过的发货者*/
int All_Dis_1 = 0;/*IVECO驶过路程*/
int All_Dis_2 = 0;/*TRUCK驶过路程*/
int All_Waittime = 0;/*总的等待时间*/
int All_Charger_Times = 0;/*总的充电站充电次数*/
int All_Go_warehouse_Times = 0;/*总的路程中回家的次数*/
int Times = 0;/*所有经过点(仓库、收货者、充电站)的次数之和*/



int Find_MapRoute(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*找出整个的线路地图（即，各个单条路线组成的路线簇）*/
{
	Clean_Global_Storage();/*还原所有全局变量 至初始值*/
	Clean_Route_List(Data_struct, Array_Struct);/*将链表清空*/
	struct Car_route** Route_List = Array_Struct->Route_List;
	int Reciver_and_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	int Receiver_num = Data_struct->Receiver_num, Sender_num = Data_struct->Sender_num;
	int All_receiver = 0, Route_receiver = 0;/*函数内部记录的经过的接收者的个数、每条路径经过的接受点的个数*/
	int Car_ID = 0, car_type = TRUCK;/*Car_ID是车辆的数量-1 也是车辆的编号*/
	struct Car_route* Add_Car_Route = NULL;/*用来新增加车辆/路线*/
	int APS = 0,k =0,Gap =0;
	while (All_Passed_Receiver != Receiver_num || All_Passed_Sender!= Sender_num/*All_receiver != Reciver_and_Sender_num*/) {
		car_type = choose_car_type(Data_struct, Array_Struct);/*用来选择车型*/
		APS = All_Passed_Sender;
		Add_Car_Route = Add_New_Car(Car_ID, car_type, START_TIME, Array_Struct);/*这个函数的第三个参数其实是多余的，
																  可以考虑在全局范围内删掉第三个参数*/
		Route_List[Car_ID] = Add_Car_Route;/*将车辆送到全局的结果里*/
		Route_receiver = Find_Car_route(Add_Car_Route, Data_struct, Array_Struct);/*每一台车的路径找出来*/
		printf ("\n");/*IMPORT*/
		All_receiver += Route_receiver;/*记录函数内部的经过收获点的数目*/
		Car_ID++;
		
		if (APS == All_Passed_Sender) {/*防止最后产生指定车辆数内无法完成条件从而加的跳出条件*/
			if (k == 20){
							Car_ID--; 
				Data_struct->Receiver_num += Data_struct->Sender_num;
				Data_struct->Sender_num =0;
				Gap = Sender_num - All_Passed_Sender;
				All_Passed_Sender = Sender_num;
				All_Passed_Receiver -= Gap;
			}
			k++;
		}
		if (Car_ID > MAX_CAN_USED_CAR_NUM){
			printf ("车辆数超标！\n"); 
			system("pause");
			goto NO_RE;
		} 
	}

	printf("Car_num= %d\t All_reciver=%d\n", Car_ID, All_receiver);
	double Cost_all;
	double Cost_1 = Print_DATA();
	printf("@@@Cost_2= \t%d\n", Car_ID * 300);
	Cost_all = Cost_1 + Car_ID * 300 + All_Charger_Times * 50;
 NO_RE:;
	if (Car_ID>MAX_CAN_USED_CAR_NUM) {
		Cost_all = -1;
	}
	printf("@@@Cost_All= \t%lf\n", Cost_all);
	printf("（测试）Receiver:\t%d\n", All_Passed_Receiver);
	printf("All Node_num:%d\n", Times);
	
	return Car_ID;
}



double Print_DATA(void)/*用来输出全局变量的值，程序自行计算成本*/
{
	double OTHER_COST = (All_Dis_1 / 1000.0) * 12 + (All_Dis_2 / 1000.0) * 14 + 24 * (All_Waittime + /*All_Charger_Times * 30 +*/ All_Go_warehouse_Times * 60) / 60.0;
	printf("All_Dis_1 = \t%d\nAll_Dis_2 = \t%d\n", All_Dis_1, All_Dis_2);
	printf("All_Waittime = \t%d\n", All_Waittime);
	printf("All_Charger_Times = \t%d\n", All_Charger_Times);
	printf("All_Go_warehouse_Times = \t%d\n", All_Go_warehouse_Times);
	printf("@@@Cost_1 = \t%lf\n", OTHER_COST);
	return OTHER_COST;
}



void Clean_Global_Storage(void)/*还原所有全局变量 至初始值*/
{
	All_Passed_Receiver = 0;/*现在已经经过的收货者*/
	All_Passed_Sender = 0;/*现在已经经过的发货者*/
	All_Dis_1 = 0;/*IVECO驶过路程*/
	All_Dis_2 = 0;/*TRUCK驶过路程*/
	All_Waittime = 0;/*总的等待时间*/
	All_Charger_Times = 0;/*总的充电站充电次数*/
	All_Go_warehouse_Times = 0;/*总的路程中回家的次数*/
	Times = 0;/*所有经过点(仓库、收货者、充电站)的次数之和*/
}



void Clean_MapRoute(int All_car_num, struct Array_str* Array_Struct)/*释放整个路线地地图 占用的内存空间*/ 
{
	int i;
	struct Car_route** Route_List = Array_Struct->Route_List;
	struct Time_Node* free_point1 = NULL, *free_point2 = NULL;
	for (i = 0; i < All_car_num; i++) {
		free_point1 = Route_List[i]->HEAD_Time_Node_point;
		while (free_point1) {
			free_point2 = free_point1->Next_Time_Node;
			free(free_point1);
			free_point1 = free_point2;
		}
		free(Route_List[i]);
	}
}



int Clean_Route_List(struct Data_str* Data_struct, struct Array_str* Array_Str) {/*释放单条路线占用的内存空间（清空全局存储变量的数组）*/
	int i,Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	struct Car_route** Route_List = Array_Str->Route_List;
	for (i = 0; i<Receiver_Sender_num; i++) {
		Route_List[i] = NULL;
	}
	return i;
}


struct Car_route* Add_New_Car(int car_ID, int car_type, int car_time, struct Array_str* Array_Struct)/*新添加一辆车；即，新添加一条路线，并且初始化其参数*/ 
{
	struct Car_route * CarRoute_p;
	CarRoute_p = (struct Car_route*)malloc(sizeof(struct Car_route));
	CarRoute_p->ID = car_ID;
	CarRoute_p->type = car_type;
	if (car_type == IVECO) {
		CarRoute_p->Blank_weight = I_max_weight;
		CarRoute_p->Blank_volume = I_max_volume;
		CarRoute_p->Offset_weight = 0;
		CarRoute_p->Offset_volume = 0;
		CarRoute_p->left_mileage = I_driving_range - SAFETY_DISTANCE;
	}
	else {
		if (car_type == TRUCK) {
			CarRoute_p->Blank_weight = T_max_weight;
			CarRoute_p->Blank_volume = T_max_volume;
			CarRoute_p->Offset_weight = 0;
			CarRoute_p->Offset_volume = 0;
			CarRoute_p->left_mileage = T_driving_range - SAFETY_DISTANCE;
		}		
		else {
			printf("输入Car_type有误！Car_ID:%d", car_ID);
			system("pause");
		}
	}
	CarRoute_p->car_now_time = START_TIME;
	CarRoute_p->now_positon = WAREHOUSE;
	struct Time_Node *TN_1 = (struct Time_Node *)malloc(sizeof(struct Time_Node));
	TN_1->Now_node = (Array_Struct->Node)[0];
	CarRoute_p->HEAD_Time_Node_point = TN_1;
	CarRoute_p->now_positon = 0;
	CarRoute_p->Time_Node_point = TN_1;
	return CarRoute_p;
}



int Tell_ID_type(int node_ID, struct Data_str* Data_struct)/*给定一个点的序号(node_ID),返回该点的类型（仓库、收货者、发货者、充电站）*/ 
{
	int First_receiver = WAREHOUSE_NUM, First_sender = WAREHOUSE_NUM + Data_struct->Receiver_num;
	int First_charger = First_sender + Data_struct->Sender_num, Last_charger = First_charger + Data_struct->Charger_num - WAREHOUSE_NUM;
	int Type = 1;
	if (node_ID <= WAREHOUSE) {
		if (node_ID == WAREHOUSE) {
           Type = WAREHOUSE_TYPE;
		}
		else {
			printf("node_ID：%d为负！请检查！\n", node_ID);
			system("pause");
		}
	}
	else {
		if (/*node_ID >= First_receiver &&*/ node_ID < First_sender) {/*为收货者*/
			Type = RECEIVER_TYPE;
		}
		else {
			if (node_ID < First_charger) {/*为发货者*/
				Type = SENDER_TYPE;
			}
			else {
				if (node_ID <= Last_charger) {
					Type = CHARGER_TYPE;/*未来要改成4 这里是为了用第一次数据测试程序 从而暂时没有考虑 发货者*/
				}
				else {
				printf("node_ID：%d超出范围！请检查！\n", node_ID);
				system("pause");
				}
			}
		}
	}
	return Type;
}



int Find_Car_route(struct Car_route * Car, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*求解得到并储存单条路线的路线*/
{
	int ** Time = Array_Struct->Time;
	struct str_node** Node = Array_Struct->Node;
	int Arrived_Time = START_TIME;
	int Next_node_ID = 1, Last_Next_node_ID = 0;
	int Receiver_Sender_num = 0;
	int k = 0;
	struct Time_Node* Next_TN = NULL;
	do {
		if (k != 0) {
			Last_Next_node_ID = Next_node_ID;
		}
		Next_node_ID = Find_nextnode(Car, Data_struct, Array_Struct);
		if (k == 0 && Next_node_ID != 0) {
			if ((Node[Next_node_ID]->first_receive_tm - Time[WAREHOUSE][Node[Next_node_ID]->ID]) >= 480) {
				Car->HEAD_Time_Node_point->Arrived_time = Node[Next_node_ID]->first_receive_tm - Time[WAREHOUSE][Node[Next_node_ID]->ID ];
				Car->car_now_time = Car->HEAD_Time_Node_point->Arrived_time;
			}
			else {
				Car->HEAD_Time_Node_point->Arrived_time = START_TIME;
				Car->car_now_time = 480;
			}
		}
		Next_TN = NtoTN_Transform(Next_node_ID, Array_Struct);
		if (Tell_ID_type(Next_node_ID, Data_struct) == RECEIVER_TYPE) {
			Receiver_Sender_num++;
			All_Passed_Receiver++;
		}
		if (Tell_ID_type(Next_node_ID, Data_struct) == SENDER_TYPE) {
			Receiver_Sender_num++;
			All_Passed_Sender++;
		}
		if (k == 0 && Next_node_ID != WAREHOUSE) {
			Arrive_Node(Car, Next_TN, Data_struct, Array_Struct);
			Next_TN->Array_ID = Next_node_ID;
			Car->now_positon = Next_node_ID;
			k = 1;
		}
		else {
			if (Next_node_ID != WAREHOUSE || Last_Next_node_ID != WAREHOUSE) {
				Car->Time_Node_point->Next_Time_Node = Next_TN;
				Car->Time_Node_point = Next_TN;
				Arrive_Node(Car, Next_TN, Data_struct, Array_Struct);
				Next_TN->Array_ID = Next_node_ID;
				Car->now_positon = Next_node_ID; 
			}
			
		}
	} while (Next_node_ID != 0 || Last_Next_node_ID != 0);
	Car->Time_Node_point->Next_Time_Node = NULL;
	
	return Receiver_Sender_num;/*这里未来可以使用一个数通过运算传达两个值；
									如：个位数可以用0/1表示是RECEIVER/Sender*/
}



struct Time_Node* NtoTN_Transform(int Node_ID, struct Array_str* Array_Struct)/*将初始的点的属性从Node包装到Time_Node结构;即，N(Node)toTN(Time_Node)（这里名字中的to未来可以改为2）*/
{
	struct str_node** Node = Array_Struct->Node;
	struct Time_Node* Now_Time_Node = (struct Time_Node*) malloc(sizeof(struct Time_Node));
	Now_Time_Node->Now_node = Node[Node_ID];
	Now_Time_Node->Next_Time_Node = NULL;
	Now_Time_Node->Arrived_time = START_TIME;
	Now_Time_Node->Array_ID = Node_ID;
	return Now_Time_Node;
}



void Arrive_Node(struct Car_route *  Car, struct Time_Node * T_Node, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*用于更改点的属性信息，以表示经过了该点*/ 
{/*注：本函数 类似于'Choose_Car_Type.c'中Arrive_Node_CC函数。
		以后可以考虑 将两个函数合并使用（如：添加一个变量tell：车型判断时取值为1；路线求解时值为0）*/ 
	printf ("@%d->",T_Node->Now_node->ID);/*IMPORT!!!*/
	Times++;
	Car->Time_Node_point->Next_Time_Node = T_Node;
	Car->Time_Node_point = T_Node;
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int Node_ID = T_Node->Array_ID;/*T_Node->Now_node->ID;*/
	int Node_Type = 0;
	int Arrived_Time = 0;
	Node_Type = Tell_ID_type(Node_ID, Data_struct);
	if (Car->type == IVECO) {
		All_Dis_1 += (Distance[Node[Car->now_positon]->ID][Node[Node_ID]->ID ]);
	}
	else {
		All_Dis_2 += (Distance[Node[Car->now_positon]->ID][Node[Node_ID]->ID]);
	}

	switch (Node_Type) {

	case WAREHOUSE_TYPE: {

		if (Car->type == IVECO) {
			All_Go_warehouse_Times++;
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
			if (Car->type == TRUCK) {
				Car->Blank_weight = T_max_weight;
				Car->Blank_volume = T_max_volume;
				Car->Offset_weight = 0;
				Car->Offset_volume = 0;
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
		Car->Blank_weight -= Node[Node_ID]->pack_total_weight;
		Car->Blank_volume -= Node[Node_ID]->pack_total_volume;
		Car->Offset_weight += Node[Node_ID]->pack_total_weight;
		Car->Offset_volume += Node[Node_ID]->pack_total_volume;
		Car->left_mileage -= Distance[Node[Car->now_positon]->ID][Node[Node_ID]->ID ];
		Arrived_Time = Car->car_now_time + Time[Node[Car->now_positon]->ID][Node[Node_ID]->ID ];
		T_Node->Arrived_time = Arrived_Time;
		T_Node->Now_node->Tell_pass = 1;
		if (Arrived_Time < T_Node->Now_node->first_receive_tm) {
			All_Waittime += (T_Node->Now_node->first_receive_tm - Arrived_Time);
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

	case SENDER_TYPE :{
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
		T_Node->Now_node->Tell_pass = 1;
		if (Arrived_Time < T_Node->Now_node->first_receive_tm) {
			All_Waittime += (T_Node->Now_node->first_receive_tm - Arrived_Time);
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
		All_Charger_Times++;
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



int Find_nextnode(struct Car_route *  Car, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*在单条路线求解中，寻找路线中下一个点*/
{
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int Now_node_ID = Car->now_positon;
	int Next_node_ID = 0;
	int Near_charger_ID = 0;
	int Near_Receiver_array[MAX_RECEIVER_ARRAY] = { 0 };
	int Tell_go[MAX_RECEIVER_ARRAY] = { 0 };
	Near_charger_ID = Find_nearest_charger(Car->now_positon, Data_struct, Array_Struct);
	Find_nearest_receiver(Car, Near_Receiver_array, All_Passed_Receiver, Data_struct, Array_Struct);
	int i;
	for (i = 0; i<MAX_RECEIVER_ARRAY; i++) {
		if (Near_Receiver_array[i] == 0) {
			Tell_go[i] = -1;
		}
		else {
			Tell_go[i] = Tell_GOorNOT(Car, Near_Receiver_array[i], Data_struct, Array_Struct);
		}
	}
	for (i = 0; i<MAX_RECEIVER_ARRAY; i++) {
		if (Tell_go[i] == 0) {
			Next_node_ID = Near_Receiver_array[i];
			goto Find_node_OK;
		}
	}
	for (i = 0; i<MAX_RECEIVER_ARRAY; i++) {
		if (Tell_go[i] == 1) {
			Next_node_ID = Near_charger_ID;
			goto Find_Charger_OK;
		}
	}
	for (i = 0; i<MAX_RECEIVER_ARRAY; i++) {
		if (Tell_go[i] == 2) {
			Next_node_ID = WAREHOUSE;
			goto Go_warehouse;
		}
	}
	if (Tell_go[0] == -1) {
		if ((Car->left_mileage) < Distance[Node[Now_node_ID]->ID][WAREHOUSE]) {
			Next_node_ID = Near_charger_ID;
		}
		else {
			Next_node_ID = WAREHOUSE;
		}
		goto NO_choice_WHorCH;
	}
	printf("Find_nextnode 函数出现漏洞请检查! 出现数值越界！\tNow_position: %d\n", Car->now_positon);
NO_choice_WHorCH:;
Go_warehouse:;
Find_Charger_OK:;
Find_node_OK:;
	return Next_node_ID;
}



int Tell_GOorNOT(struct Car_route *  Car, int Next_node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*判断备选点 是否可以前往*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int Next_step = 0;
	int Now_Time = Car->car_now_time;
	int Now_positon = Car->now_positon;
	if ((Time[Node[Now_positon]->ID][Node[Next_node_ID]->ID] + Now_Time) > (FINAL_TIME - SAFETY_LEFT_TIME)) {
		if ((Car->left_mileage) < Distance[Node[Now_positon]->ID][WAREHOUSE]) {
			Next_step = 1;/*充电站*/
		}
		else {
			Next_step = 2;/*回仓库*/
		}
	}
	else {
		if (((Car->Blank_weight) < Node[Next_node_ID]->pack_total_weight) || ((Car->Blank_volume) < Node[Next_node_ID]->pack_total_volume)) {
			if ((Car->left_mileage) < Distance[Node[Now_positon]->ID] [WAREHOUSE]) {
				Next_step = 1;//充电站 
			}
			else {
				Next_step = 2;/*回仓库*/ 
			}
		}
		else {
			if (Car->left_mileage < Distance[Node[Now_positon]->ID][Node[Next_node_ID]->ID] + Distance[Node[Next_node_ID]->ID][Node[Find_nearest_charger(Next_node_ID,Data_struct,Array_Struct)]->ID]
			/*(Car->left_mileage) < Distance[Now_positon][Next_node_ID]*/) {
				Next_step = 1;/*去充电站*/ 
			}
			else {
				Next_step = 0;/*可以前往*/
			}
		}
	}
	return Next_step;
}



void Find_nearest_receiver(struct Car_route* Car, int *Near_Receiver_array, int Passed_Receiver_num, struct Data_str* Data_struct, struct Array_str* Array_Struct)
/*找到距离当前路线中 距当前所在点醉经的收货者或者送货者；
	注：这里命名比较早，在其他函数中有调用；更改比较复杂，暂时还未更改；
		正确情况是 Find_nearest_receiver 应改为 Find_nearest_receiver_or_sender*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int now_time = Car->car_now_time;
	int now_node = Car->now_positon;
	int Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;/*目前的程序暂时没有考虑收货者的问题，目前暂时在处理只有收货人的问题*/
	int Near_charger_ID = Find_nearest_charger(now_node, Data_struct, Array_Struct);
	int Next_receiver_ID_1 = 0, Next_receiver_ID_2 = 0, Next_receiver_ID_3 = 0, Next_receiver_ID_4 = 0;
	double factor_1 = 0, factor_2 = 0, factor_3 = 0, factor_4 = 0;
	int min_dis = 0, secondmin_dis = 0, min_time_left = 0, secondmin_time_left = 0;
	int min_dis_order = 0, secondmin_dis_order = 0;
	int min_time_order = 0, secondmin_time_order = 0;
	int Car_type = Car->type;
	int Judge = 1;/*0表示可行，1表示不可行*/
	double Max_weight, Max_volume;
	double temp_Blank_weight = 0, temp_Blank_volume = 0;
	if (Car_type == IVECO) {
		Max_weight = I_max_weight;
		Max_volume = I_max_volume;
	}
	else {
		if (Car_type == TRUCK) {
			Max_weight = T_max_weight;
			Max_volume = T_max_volume;
		}
		else {
			printf("Car_type 出错！请检查！");
			system("pause");
		}
	}
	int i, k = 0;
	for (i = First_Receiver; i <= Receiver_Sender_num; i++) {
		Judge = 1;
		if (Node[i]->Tell_pass != 1) {
			if (Tell_ID_type(i, Data_struct) == SENDER_TYPE) {
				temp_Blank_weight = Car->Blank_weight;
				temp_Blank_volume = Car->Blank_volume;
				if (Node[i]->pack_total_weight > Car->Offset_weight) {
					temp_Blank_weight += (Node[i]->pack_total_weight - Car->Offset_weight);
				}
				else {
					temp_Blank_weight -= Node[i]->pack_total_weight;
				}
				if (Node[i]->pack_total_volume > Car->Offset_volume) {
					temp_Blank_volume += (Node[i]->pack_total_volume - Car->Offset_volume);
				}
				else {
					temp_Blank_volume -= Node[i]->pack_total_volume;
				}
				if ((temp_Blank_volume <= Max_volume) && (temp_Blank_weight <= Max_weight) && (temp_Blank_volume >= 0) && (temp_Blank_weight >= 0)) {
					Judge = 0;
				}
			}
			else {
				if (Tell_ID_type(i, Data_struct) == RECEIVER_TYPE ) {
					if ((Node[i]->pack_total_volume <= Car->Blank_volume) && (Node[i]->pack_total_weight <= Car->Blank_weight)) {
						Judge = 0;
					}

				}
				
				else {
					printf("Car_type出现错误！请检查！（Find_nearest_receiver）");
						system("pause");
				}
			}
			if (Judge == 0){
			if (Tell_GOorNOT(Car, i, Data_struct, Array_Struct) == 0) {
				if (Node[i]->last_receive_tm >= (now_time + Time[Node[now_node]->ID][Node[i]->ID])) {
					if (k == 0) {
						min_time_left = Node[i]->last_receive_tm;
						min_time_order = i;
						min_dis = Distance[Node[now_node]->ID][Node[i]->ID];
						min_dis_order = i;
						k = 1;
					}
					else {
						if (Node[i]->last_receive_tm < min_time_left) {
							secondmin_time_left = min_time_left;
							min_time_left = Node[i]->last_receive_tm;
							secondmin_time_order = min_time_order;
							min_time_order = i;
						}
						else {
							if (Node[i]->last_receive_tm < secondmin_time_left) {
								secondmin_time_left = Node[i]->last_receive_tm;
								secondmin_time_order = i;
							}
						}
						if (Distance[Node[now_node]->ID][Node[i]->ID] < min_dis) {
							secondmin_dis = min_dis;
							min_dis = (Distance[Node[now_node]->ID][Node[i]->ID]);
							secondmin_dis_order = min_dis_order;
							min_dis_order = i;
						}
						else {
							if (Node[i]->last_receive_tm < secondmin_dis) {
								secondmin_dis = Node[i]->last_receive_tm;
								secondmin_dis_order = i;
							}
						}
					}
				  }
				}
			}
		}
	}
	factor_1 = Balence_Time(Car, min_time_order, Data_struct, Array_Struct)*T_chance *TtoD_factor + Distance[Node[now_node]->ID][Node[min_time_order]->ID]*D_chance*(Distance_rate - All_Passed_Receiver) / 1000.0;
	factor_2 = Balence_Time(Car, min_dis_order, Data_struct, Array_Struct)*T_chance *TtoD_factor + Distance[Node[now_node]->ID][Node[min_dis_order]->ID]*D_chance*(Distance_rate - All_Passed_Receiver) / 1000.0;
	factor_3 = Balence_Time(Car, secondmin_time_order, Data_struct, Array_Struct)*T_chance *TtoD_factor + Distance[Node[now_node]->ID][Node[secondmin_time_order]->ID]*D_chance*(Distance_rate - All_Passed_Receiver) / 1000.0;
	factor_4 = Balence_Time(Car, secondmin_dis_order, Data_struct, Array_Struct)*T_chance *TtoD_factor + Distance[Node[now_node]->ID][Node[secondmin_dis_order]->ID]*D_chance*(Distance_rate - All_Passed_Receiver) / 1000.0;
	if (min_dis_order != 0 && min_time_order != 0) {
		if (factor_2 < factor_1) {
			Next_receiver_ID_1 = min_dis_order;
			Next_receiver_ID_2 = min_time_order;
		}
		else {
			Next_receiver_ID_1 = min_time_order;
			Next_receiver_ID_2 = min_dis_order;
		}
		if (secondmin_dis_order != 0 && secondmin_time_order != 0) {
			if (factor_4 < factor_3) {
				Next_receiver_ID_3 = secondmin_dis_order;
				Next_receiver_ID_4 = secondmin_time_order;
			}
			else {
				Next_receiver_ID_3 = secondmin_time_order;
				Next_receiver_ID_4 = secondmin_dis_order;
			}

		}
		else {
			if (secondmin_dis_order == 0) {
				Next_receiver_ID_3 = secondmin_time_order;
				Next_receiver_ID_4 = 0;
			}
			if (secondmin_time_order == 0) {
				Next_receiver_ID_3 = secondmin_time_order;
				Next_receiver_ID_4 = 0;
			}
		}
		Near_Receiver_array[2] = Next_receiver_ID_3;
		Near_Receiver_array[3] = Next_receiver_ID_4;
	}
	else {
		if (min_dis_order == 0) {
			Next_receiver_ID_1 = min_time_order;
			Next_receiver_ID_2 = 0;
		}
		if (min_time_order == 0) {
			Next_receiver_ID_1 = min_time_order;
			Next_receiver_ID_2 = 0;
		}
	}
	Near_Receiver_array[0] = Next_receiver_ID_1;
	Near_Receiver_array[1] = Next_receiver_ID_2;
}



int Balence_Time(struct Car_route* Car, int Node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*计算从当前点到下一个点，所花费的等待时间*/
{
	struct str_node** Node = Array_Struct->Node;
	int ** Time = Array_Struct->Time;
	int wait_time = Node[Node_ID]->first_receive_tm - (Car->car_now_time + Time[Car->now_positon][Node[Node_ID]->ID]);
	if (wait_time < 0) {
		wait_time = 0;
	}
	return wait_time;
}



int Find_nearest_charger(int now_node, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*找到距离当前路线中 距当前所在点醉经的充电站；*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int First_charger = Data_struct->Receiver_num + Data_struct->Sender_num + WAREHOUSE_NUM, Last_charger = First_charger + Data_struct->Charger_num-1;
	int near_charge_dis = 0, near_charge_ID = 0;
	int i, k = 0;
	for (i = First_charger; i <= Last_charger; i++) {
		if (k == 0) {
			Node[1]->ID;
			near_charge_dis = Distance[Node[now_node]->ID][Node[i]->ID];
			near_charge_ID = i;
			k = 1;
		}
		else {
			if (Distance[Node[now_node]->ID][Node[i]->ID] < near_charge_dis) {
				near_charge_dis = (Distance[Node[now_node]->ID][Node[i]->ID]);
				near_charge_ID = i;
			}
		}
	}
	return  near_charge_ID;
}
