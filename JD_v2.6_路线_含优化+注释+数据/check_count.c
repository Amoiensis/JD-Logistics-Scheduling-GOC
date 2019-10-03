#include<stdio.h>
#include<stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#define ZOOM_TRANS_DELIVERY sizeof(struct trans_delivery)

/*本文件中的函数 及其相关作用为： 用于检查求解所得的路线是否符合各个限制条件
	注： 本部分中 大量注释调的部分因为添加了送货者，所以需要更新。*/ 

void Create_Check_Area(struct Data_str* Data_struct) 
{
	int All_Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	Result = (struct trans_delivery**)malloc(All_Node_num * sizeof(struct trans_delivery*));
	car_go_num = (int **)malloc(All_Node_num*sizeof(int *));
}



void Clean_Check_Area(void)
{
	free(Result);
	free(car_go_num);
}



int check_first(struct Data_str* Data_struct, struct Array_str* Array_Struct)
{
	struct str_node** Node = Array_Struct->Node;
	int i, Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	int final_first = 0;

	for (i = First_Receiver; i<Receiver_Sender_num; i++)
	{
		if (Node[i]->type == RECEIVER_TYPE || Node[i]->type == SENDER_TYPE)
		{
			if (Node[i]->Tell_pass == 0)/*判断是否经过*/
			{
				printf("Node_ID:%d 未经过！请检查！", i);
				system("pause");
				final_first = 1;
				break;
			}
		}
	}

	if (final_first == 1)
	{
		printf("没有到达全部商家\n");
		return (-1);
	}
	else
	{
		return 1;
	}
}



double calculate_total_cost(double trans_cost, int charge_cost, double wait_cost, int fixed_use_cost)
{
	double sum;
	sum = trans_cost + charge_cost + wait_cost + fixed_use_cost;
	return (sum);
}



int calculate_charge_cost(int charge_cnt)
{
	return (50 * charge_cnt);
}



double calculate_wait_cost(int first_receive_tm, int car_now_time)
{
	double sum;
	sum = (first_receive_tm - car_now_time)  * 24.0 / 60.0;
	return sum;
}



int calculate_fixed_use_cost(short vehicle_type)
{
	if (vehicle_type == IVECO)
	{
		return 200;
	}
	else {
        if (vehicle_type == TRUCK)
	    {
		    return 300;
		}
		else {
			printf("车辆vehicle_type出错！（calculate_fixed_use_cost），请检查！\n");
			system("pause");
			return -1;
		}
	}
}



double calculate_distance(struct Car_route *Route,struct Array_str* Array_Struct)
{
	int ** Distance = Array_Struct->Distance;
	double total_distance = 0;
	struct Time_Node *p1, *p2;
	p2 = Route->HEAD_Time_Node_point;
	while (p2)
	{
		p1 = p2->Next_Time_Node;
		if (p1){
			total_distance +=  Distance[(p1->Now_node->ID)][(p2->Now_node->ID)];			
		}

		p2 = p1;
	}
	return  total_distance;
}



double calculate_trans_cost(short vehicle_type, int distance)
{
	double sum;
	if (vehicle_type == IVECO)
	{
		sum = distance * 12.0 / 1000.0;
	}
	if (vehicle_type == TRUCK)
	{
		sum = distance * 14.0 / 1000.0;
	}
	return sum;
}



void final_check(struct Data_str* Data_struct, struct Array_str* Array_Struct)
{
	Create_Check_Area(Data_struct);/*将头部设为全局变量，然后在函数内给它们赋值，这不是一种很好的方案，但是可以行*/
	struct str_node** Node = Array_Struct->Node;
	struct Car_route** Route_List = Array_Struct->Route_List;
	int ** Distance = Array_Struct->Distance;
	double check_distance;
	double Load_weight,Offset_weight;
	double Load_volume,Offset_volume;
	double final_cost = 0;
	int arr0;
	int last_place;
	int i;
	int x;
	int place_number;
	struct Time_Node *temp;
	short flag = 1;
	int Node_type;
	struct Time_Node *temp0;

	int j = 0;

	if (check_first(Data_struct, Array_Struct) == 1)/*判断是否所有需要到达的点都经过了*/
	{
		for (i = 0, x = 1; Route_List[i]; i++, x++)
		{
			arr0 = 0;
			car_go_num[i] = (int*)malloc(sizeof(int));

			Result[i] = (struct trans_delivery*) malloc(ZOOM_TRANS_DELIVERY);

			Result[i]->trans_code[0] = x / 1000;/*这里不明白其作用*/
			Result[i]->trans_code[1] = (x / 100) % 10;
			Result[i]->trans_code[2] = (x / 10) % 10;
			Result[i]->trans_code[3] = x % 10;

			Result[i]->vehicle_type = Route_List[i]->type;

			if (Route_List[i]->type == IVECO)
			{
				check_distance = I_driving_range;
				if ((Route_List[i]->Blank_weight <= I_max_weight) && (Route_List[i]->Blank_weight >= 0) &&
					(Route_List[i]->Blank_volume <= I_max_volume) && (Route_List[i]->Blank_volume >= 0)) {
						Load_weight = I_max_weight - Route_List[i]->Blank_weight;
						Load_volume =  I_max_volume - Route_List[i]->Blank_volume;
						Offset_weight = Route_List[i]->Blank_weight;
						Offset_volume =Route_List[i]->Blank_volume;
				}
				else {
					printf("车辆Car_ID:%d Type:%d装货完后剩余空间错误！请检查！",i+1, Route_List[i]->type);
					system("pause");
				}	
			}

			if (Route_List[i]->type == TRUCK)
			{
				if ((Route_List[i]->Blank_weight <= T_max_weight) && (Route_List[i]->Blank_weight >= 0) &&
					(Route_List[i]->Blank_volume <= T_max_volume) && (Route_List[i]->Blank_volume >= 0)) {
						check_distance = T_driving_range;
						Load_weight = T_max_weight - Route_List[i]->Blank_weight;
						Load_volume = T_max_volume - Route_List[i]->Blank_volume;
						Offset_weight = Route_List[i]->Blank_weight;
						Offset_volume = Route_List[i]->Blank_volume;
				}
				else {
					printf("车辆Car_ID:%d Type:%d装货完后剩余空间错误！请检查！",i+1, Route_List[i]->type);
					system("pause");
				}
			}

			Result[i]->distance = calculate_distance(Route_List[i], Array_Struct);
			Result[i]->trans_cost = calculate_trans_cost(Route_List[i]->type, Result[i]->distance);
			Result[i]->distribute_lea_tm = Route_List[i]->HEAD_Time_Node_point->Arrived_time;

			place_number = 1;

			Result[i]->fixed_use_cost = calculate_fixed_use_cost(Route_List[i]->type);

			temp = (struct Time_Node*) malloc(sizeof(struct Time_Node));
			temp = Route_List[i]->HEAD_Time_Node_point;

			Result[i]->charge_cnt = 0;
			Result[i]->wait_cost = 0;
			last_place = temp->Now_node->ID;

			temp0 = temp->Next_Time_Node;

			if (temp0->Arrived_time < temp0->Now_node->first_receive_tm)
			{
				printf("第%d条路线到达第一点有等待时间\n", i + 1);
				break;
			}

			while (temp)
			{
				Result[i]->dis_seq[place_number] = temp->Now_node->ID;
				place_number = place_number + 1;
				*car_go_num[i] = place_number;
				Node_type = Node[temp->Now_node->ID]->type;
				check_distance -= Distance[last_place][temp->Now_node->ID];

				//if (Node_type == RECEIVER_TYPE) {/*这里有一点需要注意，这里的初始值应该确保是正确的*/
				//	Load_weight -= Node[temp->Now_node->ID]->pack_total_weight;
				//	Offset_weight += Node[temp->Now_node->ID]->pack_total_weight;
				//	Load_volume -= Node[temp->Now_node->ID]->pack_total_volume;
				//	Offset_volume += Node[temp->Now_node->ID]->pack_total_volume;
				//}
				//else {
				//	if (Node_type == SENDER_TYPE) {
				//		Offset_weight -= Node[temp->Now_node->ID]->pack_total_weight;
				//		Offset_volume -= Node[temp->Now_node->ID]->pack_total_volume;
				//	}
				//	/*else {
				//		printf("final_check中，Node_ID:%d\t点的类型出错！请检查！", temp->Now_node->ID);
				//		system("pause");
				//	}*/
				//}

				//last_place = temp->Now_node->ID;

				//if (check_distance<0)
				//{
				//	flag = 0;
				//	printf("第%d条路线电量用完！\n", i + 1);
				//	break;
				//}

				//if (Load_volume<0)
				//{
				//	flag = 0;
				//	printf("路线%d\tNode:%d配送超过最高装载体积！\n", i + 1, temp->Now_node->ID);
				//	break;
				//}

				//if (Load_weight<0)
				//{
				//	flag = 0;
				//	printf("路线%d\tNode:%d配送超过核定载重！\n", i + 1, temp->Now_node->ID);
				//	break;
				//}
				//if (Offset_weight < 0) {
				//	flag = 0;
				//	printf("路线%d\tNode:%d揽货可用载重不足！\n", i + 1, temp->Now_node->ID);
				//	system("puase");
				//}
				//if (Offset_volume < 0) {
				//	flag = 0;
				//	printf("路线%d\tNode:%d揽货可用体积不足！\n", i + 1, temp->Now_node->ID);
				//	system("pause");
				//}



				if (temp->Now_node->type == WAREHOUSE_TYPE)
				{
					arr0 = arr0 + 1;
					if (Route_List[i]->type == IVECO)
					{
						check_distance = I_driving_range;
					}
					if (Route_List[i]->type == TRUCK)
					{
						check_distance = T_driving_range;
					}
				}

				if (temp->Now_node->type == CHARGER_TYPE)
				{
					if (Route_List[i]->type == IVECO)
					{
						check_distance = I_driving_range;
					}

					if (Route_List[i]->type == IVECO)
					{
						check_distance = T_driving_range;
					}
					Result[i]->charge_cnt = Result[i]->charge_cnt + 1;
				}

				if (temp->Now_node->type == RECEIVER_TYPE || temp->Now_node->type == SENDER_TYPE)
				{
					if (temp->Arrived_time>temp->Now_node->last_receive_tm)
					{
						printf("第%d条路线超过商家等待时间\n", i);
						flag = 0;
						break;
					}
					else
					{
						if (temp->Arrived_time<temp->Now_node->first_receive_tm)
						{
							Result[i]->wait_cost = Result[i]->wait_cost + calculate_wait_cost(temp->Now_node->first_receive_tm, temp->Arrived_time);
						}
					}
				}
				Result[i]->distribute_arr_tm = temp->Arrived_time;
				temp = temp->Next_Time_Node;
			}

			Result[i]->wait_cost = Result[i]->wait_cost + 24 * (arr0 - 2);

			if (flag == 0)
			{
				break;
			}

			Result[i]->charge_cost = calculate_charge_cost(Result[i]->charge_cnt);
			Result[i]->total_cost = calculate_total_cost(Result[i]->trans_cost, Result[i]->charge_cost, Result[i]->wait_cost, Result[i]->fixed_use_cost);
			final_cost = final_cost + Result[i]->total_cost;
		}
		max_car_go_num = i;
		Final_total_cost = final_cost;
		printf("最终需要成本%.2lf\n", final_cost);
	}

	else
	{
		printf("此方案不可行\n");
	}
}
