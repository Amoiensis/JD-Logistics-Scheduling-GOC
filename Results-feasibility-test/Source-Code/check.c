#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"
#define TEST_NUM 1

int Check(struct Cost* cost) {
	struct Cost Rcost;
	/*得到外部原始数据*/
	int STATE = 0;
	struct Data_str* Data_struct = NULL;
	Data_struct = Create_Data_str(TEST_NUM);/*这里的1代表1组数据*/
	int Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num;
	int All_car_num = 0;
	struct Array_str* Array_struct = Create_Array_str(Data_struct);
	fpRead(Data_struct, Array_struct);
	fpRead_Dis_tm(Data_struct, Array_struct);

	/*得到路线数据*/
	struct Solved_Route* Route_store [MAX_ROUTE_NUM];
	Clean_Route_store(Route_store);
	int i, state = 0, route_node_num = 0,All_node_num = 0;
	int Node_state[Last_Receiver] = { 0 };
	int Route_num = Read_cvs(Route_store);
	double Charge_cost = 0, Dis_cost = 0,Fix_cost=0,Wait_cost=0;
	
/****额外*****/	
	char* w_filename = "Route_cost.txt";
	FILE*fp_w;
	fp_w = fopen(w_filename, "w");
/*********/	
	
	for (i = 0; i < Route_num; i++) {
		state = 0;
		route_node_num = Check_route(Route_store[i],&state,Node_state,Data_struct, Array_struct, &Rcost);
		if (route_node_num) {
			All_node_num += route_node_num;
			/**********/
			fprintf (fp_w,"%d,",route_node_num);
			/**********/
		}
		else {
			printf("\n第%d条路线经过点数非正！请检查！\n",i);
			system("pause");
		}
		switch (state)/*对该线路的状态判断*/
		{
		case 0:
			goto PASS;
			break;
		case 1:
			printf("\n第%d条路线存在迟到！请检查！\n", i);
			break;
		case 2:
			printf("\n第%d条路线存在可用路程不够！请检查！\n", i);
			break;
		case 3:
			printf("\n第%d条路线存在装载体积超标！请检查！\n", i);
			break;
		case 4:
			printf("\n第%d条路线存在装载质量超标！请检查！\n", i);
			break;
		case 5:
			printf("\n第%d条路线出发点不为仓库！请检查！\n", i);
			break;
		case 6:
			printf("\n第%d条路线出现越界点！请检查！\n", i);
			break;
		default:
			printf("\n程序错误！错误状态%d不存在！\n", state);
			break;
		}
	PASS:;
		Charge_cost += Rcost.Charge_cost;
		Dis_cost += Rcost.Dis_cost;
		Fix_cost += Rcost.Fix_cost;
		Wait_cost += Rcost.Wait_cost;
		/********/ 
		fprintf (fp_w,"%lf,%lf,%lf,%lf\n",Rcost.Charge_cost,Rcost.Dis_cost,Rcost.Fix_cost,Rcost.Wait_cost);
		/********/ 
	}
		/********/ 
		printf(">>路线具体成本，见Route_cost.txt\n");
		/********/ 
	
//		cost->Charge_cost += Rcost.Charge_cost;
//		cost->Dis_cost += Rcost.Dis_cost;
//		cost->Fix_cost += Rcost.Fix_cost;
//		cost->Wait_cost += Rcost.Wait_cost;
		cost->Charge_cost = Charge_cost;
		cost->Dis_cost = Dis_cost;
		cost->Fix_cost = Fix_cost;
		cost->Wait_cost = Wait_cost;
	if (All_node_num < Last_Receiver) {
		printf("存在部分点未经过(如下)！：\n");
			STATE = -1;
		for (i=First_Receiver;i<Last_Receiver;i++){
		    if(Node_state[i]==0){
			printf("#%d\t",i);
		    }
	    }
	    system ("pause"); printf("\n");
	}
	if (All_node_num > Last_Receiver){
		printf("存在部分点重复经过（如下）！\n");
		STATE = -1;
		for (i=First_Receiver;i<Last_Receiver;i++){
		    if(Node_state[i]>1){
			printf("#%d\t",i);printf("\n");
		    }
	    }
		system ("pause"); 
	}
	return STATE;
}

int Check_route(struct Solved_Route* Route,int*state, int Node_state[], struct Data_str* Data_struct, struct Array_str* Array_struct, struct Cost* cost) {
	struct str_node** Node = Array_struct->Node;
	//state=(int*)malloc(sizeof(int));
	int ** Distance = Array_struct->Distance;
	int ** Time = Array_struct->Time;
	*state = 0;/*
			  0-正常
			  1-迟到
			  2-可用路程不够
			  3-体积超标
			  4-质量超标
			  5-出发点不为仓库
			  6-出现越界点
			  */
	/*在这儿做出判断！还要计算成本！(Core)*/
	int Car_type = Route->Car_type, Now_position = WAREHOUSE,Next_position = 0,start_time = START_TIME;
	int distance = 0, time = Route ->Start_time;
	double volume = 0, weight=0;
	int wait_time = 0, dis_1 = 0, dis_2 = 0, ch_time=0, home_time = 0;
	int i,Reciver = 0;
	/*if (Route->Pass_node[0] != WAREHOUSE) {
		printf("\n本条路线出发点不为仓库！请检查！\n");
		system("pause");
		goto AAA;
	}*/ 
	for (i = 0; i < MAX_NODE_NUM ; i++) {
		Next_position = Route->Pass_node[i];
		if (Next_position == WAREHOUSE) {
			
			if (Car_type == IVECO) {
				distance = I_driving_range;
				volume = I_max_volume;
				weight = I_max_weight;
			}
			if (Car_type == TRUCK) {
				distance = T_driving_range;
				volume = T_max_volume;
				weight = T_max_weight;
			}
			time += Time[Now_position][Next_position];
			if(home_time!=0){
				time+=60;//飞第一次经过仓库出发有等待时间60  
				if (Next_position == 0&& Now_position ==0){
					goto GGG;
				}
			}
			home_time++;
		}
		if ( ( Next_position < First_Charger )&&(WAREHOUSE <Next_position)) {
			Node_state [Next_position] ++;
			Reciver ++;
			distance -= Distance[Now_position][Next_position];
			volume -= Node[Next_position]->pack_total_volume;
			weight -= Node[Next_position]->pack_total_weight;
			
		  if ((time+Time[Now_position][Next_position]) <= Node[Next_position]->last_receive_tm) {
			   if ((time+Time[Now_position][Next_position]) <= Node[Next_position]->first_receive_tm) {
				wait_time += Node[Next_position]->first_receive_tm -(time + Time[Now_position][Next_position]);
				time = Node[Next_position]->first_receive_tm + 30;
			  }else{
				time += (Time[Now_position][Next_position] + 30);//卸货时间30 
			    }
		    }else {
			*state = 1;/*迟到*/
			goto CCC;
		    }			
		}
		
		if (Next_position >= First_Charger && Next_position <= Last_Charger) {
			ch_time++;
			distance -= Distance[Now_position][Next_position];
			if (Car_type == IVECO) {
				distance = I_driving_range;
			}
			if (Car_type == TRUCK) {
				distance = T_driving_range;
			}
			time += (Time[Now_position][Next_position] + 30);//充电时间30 
		}
		if (Next_position > Last_Charger) {
			printf("\n出现越界点:%d!请检查\n", Next_position);
			goto BBB;
		}

		if (distance<0) {
			*state = 2;/*可用路程不够*/
			goto DDD;
		}
		if (volume < 0) {
			*state = 3;/*体积超标*/
			goto EEE;
		}
		if (weight < 0) {
			*state = 4;/*质量超标*/
			goto FFF;
		}
		
		if (Car_type == IVECO) {
			dis_1 += Distance[Now_position][Next_position];
		}
		if (Car_type == TRUCK) {
			dis_2 += Distance[Now_position][Next_position];
		}
		Now_position = Next_position;
	}
	GGG:;
	cost->Charge_cost = ch_time* 100;
	cost->Dis_cost = dis_1/1000*12+ dis_2/1000*14;
	if (Car_type == IVECO) {
		cost->Fix_cost = 200;
	}
	if (Car_type == TRUCK) {
		cost->Fix_cost = 300;
	}
	cost->Wait_cost = (wait_time+ch_time*30.0+(home_time-1)*60.0)/60.0*24.0;
FFF:;EEE:;DDD:;CCC:;BBB:;//AAA:;
	int Rnode_num = Reciver;
	return Rnode_num;
}

int Clean_Route_store(struct Solved_Route** Route_store) {
	int i;
	for (i = 0; i < MAX_ROUTE_NUM; i++) {
		Route_store[i] = NULL;
	}
	return i;
}

double Print_Cost (struct Cost* cost){
	printf ("Car_cost = %.2lf\n",cost->Fix_cost);
	printf ("Charge_cost = %.2lf\n",cost->Charge_cost);
	printf ("Wait_cost = %.2lf\n",cost->Wait_cost);
	printf ("Distance_cost = %.2lf\n",cost->Dis_cost);
	double Total_cost = cost->Fix_cost+cost->Charge_cost+cost->Wait_cost+cost->Dis_cost;
	return Total_cost;
}
