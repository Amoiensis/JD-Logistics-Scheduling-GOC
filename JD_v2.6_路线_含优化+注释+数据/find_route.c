#include <stdio.h>
#include <stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"

#define SAFETY_DISTANCE 0 /*ע����Ϊ���ǵ�������ͳ���ʱ�� ���ᳬ��ʱ������(24:00)��
								�Ӷ�����ȥ�� ���ڰ�ȫʣ����� ������*/ /*��14000 //8450 //15030 ����Ϊ��ȫ��ʣ�����ľ���ֵ��*/ 
#define SAFETY_LEFT_TIME 117 /*��ȫ��ʣ��ʱ�䣬��ֹ�������زֿ�����ʱ������(24:00)��
								ע����ĿǰΪ֮��û�г��ֳ�������ʱ�䳬�޵�����*/
#define TtoD_factor 447   /*ʱ��ת��������ķŴ���������ֵ*/
						      /*ע��447//350//477 Ϊ���εľ���ֵ��
							  		���壺��ʱ����ֵת����Ϊ�;���һ��������ʹ�õķŴ�����
									�õ���ʽ�����ܵľ����ƽ��ֵ���ܵ�ʱ���ƽ��ֵ����*/
#define T_chance 3.9  /*Ȩ���жϵľ���ֵ��ʱ�������ϵ����*/
						/*3.9//2.4//2.04 Ϊ���ξ���ֵ�� 
						�õ���ʽ��Ȩֵ���ڲ��ñ��������õ����ڴ�δ��������ѧ��������õ�*/
#define D_chance 0.7  /*Ȩ���жϵľ���ֵ�����������ϵ����*/ 
						/*0.7 //1-0.7 Ϊ���ξ���ֵ��
						�õ���ʽ��Ȩ���жϵľ���ֵ������ʹ�õ��Ǳ��������õ����ڴ�δ�����ü������ѧ�����õ�*/
#define Distance_rate 2800  /*2800 //2000 //1-2800//1600 Ϊ���ξ���ֵ��*/ 
                            /*�����ǽ�ѡ����Ѿ��ܹ��ĵ�ҹ�����ΪԽ������ɱ�Խ��Խ�ߣ���������ʹ��һ�����Թ�ϵ������ϡ�
                              ����������ͼѰ�ң���������Ե�ͼ��Ҳ����һ�����Թ�ϵ*/


/*���������仰�������ڼ���վ�����ƶ�֮��Ӧ�ò�������Щ��������ȷ�ԣ�Ҫ�ʵ��Ŀ۳���Щ���ۼƵĲ��֣�*/
int All_Passed_Receiver = 0;/*�����Ѿ��������ջ���*/
int All_Passed_Sender = 0;/*�����Ѿ������ķ�����*/
int All_Dis_1 = 0;/*IVECOʻ��·��*/
int All_Dis_2 = 0;/*TRUCKʻ��·��*/
int All_Waittime = 0;/*�ܵĵȴ�ʱ��*/
int All_Charger_Times = 0;/*�ܵĳ��վ������*/
int All_Go_warehouse_Times = 0;/*�ܵ�·���лؼҵĴ���*/
int Times = 0;/*���о�����(�ֿ⡢�ջ��ߡ����վ)�Ĵ���֮��*/



int Find_MapRoute(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*�ҳ���������·��ͼ��������������·����ɵ�·�ߴأ�*/
{
	Clean_Global_Storage();/*��ԭ����ȫ�ֱ��� ����ʼֵ*/
	Clean_Route_List(Data_struct, Array_Struct);/*���������*/
	struct Car_route** Route_List = Array_Struct->Route_List;
	int Reciver_and_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	int Receiver_num = Data_struct->Receiver_num, Sender_num = Data_struct->Sender_num;
	int All_receiver = 0, Route_receiver = 0;/*�����ڲ���¼�ľ����Ľ����ߵĸ�����ÿ��·�������Ľ��ܵ�ĸ���*/
	int Car_ID = 0, car_type = TRUCK;/*Car_ID�ǳ���������-1 Ҳ�ǳ����ı��*/
	struct Car_route* Add_Car_Route = NULL;/*���������ӳ���/·��*/
	int APS = 0,k =0,Gap =0;
	while (All_Passed_Receiver != Receiver_num || All_Passed_Sender!= Sender_num/*All_receiver != Reciver_and_Sender_num*/) {
		car_type = choose_car_type(Data_struct, Array_Struct);/*����ѡ����*/
		APS = All_Passed_Sender;
		Add_Car_Route = Add_New_Car(Car_ID, car_type, START_TIME, Array_Struct);/*��������ĵ�����������ʵ�Ƕ���ģ�
																  ���Կ�����ȫ�ַ�Χ��ɾ������������*/
		Route_List[Car_ID] = Add_Car_Route;/*�������͵�ȫ�ֵĽ����*/
		Route_receiver = Find_Car_route(Add_Car_Route, Data_struct, Array_Struct);/*ÿһ̨����·���ҳ���*/
		printf ("\n");/*IMPORT*/
		All_receiver += Route_receiver;/*��¼�����ڲ��ľ����ջ�����Ŀ*/
		Car_ID++;
		
		if (APS == All_Passed_Sender) {/*��ֹ������ָ�����������޷���������Ӷ��ӵ���������*/
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
			printf ("���������꣡\n"); 
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
	printf("�����ԣ�Receiver:\t%d\n", All_Passed_Receiver);
	printf("All Node_num:%d\n", Times);
	
	return Car_ID;
}



double Print_DATA(void)/*�������ȫ�ֱ�����ֵ���������м���ɱ�*/
{
	double OTHER_COST = (All_Dis_1 / 1000.0) * 12 + (All_Dis_2 / 1000.0) * 14 + 24 * (All_Waittime + /*All_Charger_Times * 30 +*/ All_Go_warehouse_Times * 60) / 60.0;
	printf("All_Dis_1 = \t%d\nAll_Dis_2 = \t%d\n", All_Dis_1, All_Dis_2);
	printf("All_Waittime = \t%d\n", All_Waittime);
	printf("All_Charger_Times = \t%d\n", All_Charger_Times);
	printf("All_Go_warehouse_Times = \t%d\n", All_Go_warehouse_Times);
	printf("@@@Cost_1 = \t%lf\n", OTHER_COST);
	return OTHER_COST;
}



void Clean_Global_Storage(void)/*��ԭ����ȫ�ֱ��� ����ʼֵ*/
{
	All_Passed_Receiver = 0;/*�����Ѿ��������ջ���*/
	All_Passed_Sender = 0;/*�����Ѿ������ķ�����*/
	All_Dis_1 = 0;/*IVECOʻ��·��*/
	All_Dis_2 = 0;/*TRUCKʻ��·��*/
	All_Waittime = 0;/*�ܵĵȴ�ʱ��*/
	All_Charger_Times = 0;/*�ܵĳ��վ������*/
	All_Go_warehouse_Times = 0;/*�ܵ�·���лؼҵĴ���*/
	Times = 0;/*���о�����(�ֿ⡢�ջ��ߡ����վ)�Ĵ���֮��*/
}



void Clean_MapRoute(int All_car_num, struct Array_str* Array_Struct)/*�ͷ�����·�ߵص�ͼ ռ�õ��ڴ�ռ�*/ 
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



int Clean_Route_List(struct Data_str* Data_struct, struct Array_str* Array_Str) {/*�ͷŵ���·��ռ�õ��ڴ�ռ䣨���ȫ�ִ洢���������飩*/
	int i,Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	struct Car_route** Route_List = Array_Str->Route_List;
	for (i = 0; i<Receiver_Sender_num; i++) {
		Route_List[i] = NULL;
	}
	return i;
}


struct Car_route* Add_New_Car(int car_ID, int car_type, int car_time, struct Array_str* Array_Struct)/*�����һ���������������һ��·�ߣ����ҳ�ʼ�������*/ 
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
			printf("����Car_type����Car_ID:%d", car_ID);
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



int Tell_ID_type(int node_ID, struct Data_str* Data_struct)/*����һ��������(node_ID),���ظõ�����ͣ��ֿ⡢�ջ��ߡ������ߡ����վ��*/ 
{
	int First_receiver = WAREHOUSE_NUM, First_sender = WAREHOUSE_NUM + Data_struct->Receiver_num;
	int First_charger = First_sender + Data_struct->Sender_num, Last_charger = First_charger + Data_struct->Charger_num - WAREHOUSE_NUM;
	int Type = 1;
	if (node_ID <= WAREHOUSE) {
		if (node_ID == WAREHOUSE) {
           Type = WAREHOUSE_TYPE;
		}
		else {
			printf("node_ID��%dΪ�������飡\n", node_ID);
			system("pause");
		}
	}
	else {
		if (/*node_ID >= First_receiver &&*/ node_ID < First_sender) {/*Ϊ�ջ���*/
			Type = RECEIVER_TYPE;
		}
		else {
			if (node_ID < First_charger) {/*Ϊ������*/
				Type = SENDER_TYPE;
			}
			else {
				if (node_ID <= Last_charger) {
					Type = CHARGER_TYPE;/*δ��Ҫ�ĳ�4 ������Ϊ���õ�һ�����ݲ��Գ��� �Ӷ���ʱû�п��� ������*/
				}
				else {
				printf("node_ID��%d������Χ�����飡\n", node_ID);
				system("pause");
				}
			}
		}
	}
	return Type;
}



int Find_Car_route(struct Car_route * Car, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*���õ������浥��·�ߵ�·��*/
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
	
	return Receiver_Sender_num;/*����δ������ʹ��һ����ͨ�����㴫������ֵ��
									�磺��λ��������0/1��ʾ��RECEIVER/Sender*/
}



struct Time_Node* NtoTN_Transform(int Node_ID, struct Array_str* Array_Struct)/*����ʼ�ĵ�����Դ�Node��װ��Time_Node�ṹ;����N(Node)toTN(Time_Node)�����������е�toδ�����Ը�Ϊ2��*/
{
	struct str_node** Node = Array_Struct->Node;
	struct Time_Node* Now_Time_Node = (struct Time_Node*) malloc(sizeof(struct Time_Node));
	Now_Time_Node->Now_node = Node[Node_ID];
	Now_Time_Node->Next_Time_Node = NULL;
	Now_Time_Node->Arrived_time = START_TIME;
	Now_Time_Node->Array_ID = Node_ID;
	return Now_Time_Node;
}



void Arrive_Node(struct Car_route *  Car, struct Time_Node * T_Node, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*���ڸ��ĵ��������Ϣ���Ա�ʾ�����˸õ�*/ 
{/*ע�������� ������'Choose_Car_Type.c'��Arrive_Node_CC������
		�Ժ���Կ��� �����������ϲ�ʹ�ã��磺���һ������tell�������ж�ʱȡֵΪ1��·�����ʱֵΪ0��*/ 
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
				printf("����Car_type����Car_ID:%d\t (Arrive_Node)\n", Car->ID);
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
				printf("�����ٵ������飡");
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
				printf("�����ٵ������飡");
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
				printf("����Car_type����Car_ID:%d\t (Arrive_Node)\n", Car->ID);
				system("pause");
			}
		}
		Car->now_positon = Node_ID;
		break;
	}
	default: {
		printf("(Arrive_Node) ������ǰCar ID��%d\n��ǰ ǰ��λ��Node-ID:%d\n", Car->ID, Node_ID);
		system("pause");
		break;
	}
	}
}



int Find_nextnode(struct Car_route *  Car, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*�ڵ���·������У�Ѱ��·������һ����*/
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
	printf("Find_nextnode ��������©������! ������ֵԽ�磡\tNow_position: %d\n", Car->now_positon);
NO_choice_WHorCH:;
Go_warehouse:;
Find_Charger_OK:;
Find_node_OK:;
	return Next_node_ID;
}



int Tell_GOorNOT(struct Car_route *  Car, int Next_node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*�жϱ�ѡ�� �Ƿ����ǰ��*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int Next_step = 0;
	int Now_Time = Car->car_now_time;
	int Now_positon = Car->now_positon;
	if ((Time[Node[Now_positon]->ID][Node[Next_node_ID]->ID] + Now_Time) > (FINAL_TIME - SAFETY_LEFT_TIME)) {
		if ((Car->left_mileage) < Distance[Node[Now_positon]->ID][WAREHOUSE]) {
			Next_step = 1;/*���վ*/
		}
		else {
			Next_step = 2;/*�زֿ�*/
		}
	}
	else {
		if (((Car->Blank_weight) < Node[Next_node_ID]->pack_total_weight) || ((Car->Blank_volume) < Node[Next_node_ID]->pack_total_volume)) {
			if ((Car->left_mileage) < Distance[Node[Now_positon]->ID] [WAREHOUSE]) {
				Next_step = 1;//���վ 
			}
			else {
				Next_step = 2;/*�زֿ�*/ 
			}
		}
		else {
			if (Car->left_mileage < Distance[Node[Now_positon]->ID][Node[Next_node_ID]->ID] + Distance[Node[Next_node_ID]->ID][Node[Find_nearest_charger(Next_node_ID,Data_struct,Array_Struct)]->ID]
			/*(Car->left_mileage) < Distance[Now_positon][Next_node_ID]*/) {
				Next_step = 1;/*ȥ���վ*/ 
			}
			else {
				Next_step = 0;/*����ǰ��*/
			}
		}
	}
	return Next_step;
}



void Find_nearest_receiver(struct Car_route* Car, int *Near_Receiver_array, int Passed_Receiver_num, struct Data_str* Data_struct, struct Array_str* Array_Struct)
/*�ҵ����뵱ǰ·���� �൱ǰ���ڵ������ջ��߻����ͻ��ߣ�
	ע�����������Ƚ��磬�������������е��ã����ıȽϸ��ӣ���ʱ��δ���ģ�
		��ȷ����� Find_nearest_receiver Ӧ��Ϊ Find_nearest_receiver_or_sender*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int now_time = Car->car_now_time;
	int now_node = Car->now_positon;
	int Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;/*Ŀǰ�ĳ�����ʱû�п����ջ��ߵ����⣬Ŀǰ��ʱ�ڴ���ֻ���ջ��˵�����*/
	int Near_charger_ID = Find_nearest_charger(now_node, Data_struct, Array_Struct);
	int Next_receiver_ID_1 = 0, Next_receiver_ID_2 = 0, Next_receiver_ID_3 = 0, Next_receiver_ID_4 = 0;
	double factor_1 = 0, factor_2 = 0, factor_3 = 0, factor_4 = 0;
	int min_dis = 0, secondmin_dis = 0, min_time_left = 0, secondmin_time_left = 0;
	int min_dis_order = 0, secondmin_dis_order = 0;
	int min_time_order = 0, secondmin_time_order = 0;
	int Car_type = Car->type;
	int Judge = 1;/*0��ʾ���У�1��ʾ������*/
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
			printf("Car_type �������飡");
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
					printf("Car_type���ִ������飡��Find_nearest_receiver��");
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



int Balence_Time(struct Car_route* Car, int Node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*����ӵ�ǰ�㵽��һ���㣬�����ѵĵȴ�ʱ��*/
{
	struct str_node** Node = Array_Struct->Node;
	int ** Time = Array_Struct->Time;
	int wait_time = Node[Node_ID]->first_receive_tm - (Car->car_now_time + Time[Car->now_positon][Node[Node_ID]->ID]);
	if (wait_time < 0) {
		wait_time = 0;
	}
	return wait_time;
}



int Find_nearest_charger(int now_node, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*�ҵ����뵱ǰ·���� �൱ǰ���ڵ����ĳ��վ��*/ 
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
