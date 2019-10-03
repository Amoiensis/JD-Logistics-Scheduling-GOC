#include <stdio.h>
#include <stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"



int Optimization (struct Data_str* Data_struct, struct Array_str* Array_Struct,int* Route_ID)/*���ڰ��� �ض��Ż����� ���н��·�ߴ��Ż�*/ 
{
	int DoneorNot = 0;/*0��ʾû���Ż���1��ʾ�Ż���*/
	struct Car_route** Route_List = Array_Struct->Route_List;
	struct Car_route* Storage [MAX_GROUP_NUM];
	Storage[0] = Route_List[Route_ID[0]];
	Storage[1] = Route_List[Route_ID[1]];
	int temperory_ID[MAX_RECOMBINANT_NODE_NUM] = { 0 }; //�����Ŀ����ʹ�� define�����
	int temperory_ID_len = Extract_ID(Storage, temperory_ID, Data_struct);
	struct Recombinant_ID_array* ID_array = (struct Recombinant_ID_array*)malloc(sizeof(struct Recombinant_ID_array));
	ID_array->ID_head = temperory_ID;
	ID_array->Node_num = temperory_ID_len;
	struct Car_route** Route = Recombinant(ID_array, Data_struct, Array_Struct);//�µõ���·��

	int i =0;
	double Cost_original = 0, Cost_optimize = 0;
	while (i<2){
		Cost_original += part_check(Storage[i++],Data_struct,Array_Struct);//Route_List[i++]
	}
	Cost_optimize = Calculate_Group_Cost(Data_struct,Array_Struct,Route);
	if(Cost_optimize <= Cost_original){/*�������Ҳ֧�ֱ䶯*/
		Free_Route (Storage[0]);
		Free_Route (Storage[1]);
//		Route_List[0] = Route[0];/*������ܳ����Ż���·�����ӵ������Ӧ�ý��*/ 
//		Route_List[1] = Route[1];
		Replace (Route_List,Route,Route_ID);
		DoneorNot = 1; 
	}else{
		i = 0;
		while (Route[i]){
			Free_Route (Route[i++]);
		}
		
	}
	return DoneorNot;
}



void Replace (struct Car_route** Route_List, struct Car_route** Route, int* Route_ID)/*���Ż����·���滻ԭ����·��*/ 
/*�������壺ԭ���ܵĳ��������µĲ���·�ߣ�ԭ�����������·�ߵ����*/
{
	int Car_num_Route_List = 0,Car_num_Route,i,k;
	while (Route_List[Car_num_Route_List]) {
		Car_num_Route_List++;
	}
	while (Route[Car_num_Route]) {
		Car_num_Route++;
	}
	if (Car_num_Route > MAX_GROUP_NUM){
		Route_List [Route_ID[0]] = Route [0];
		Route_List [Route_ID[1]] = Route [1];
		for (i = 0, k = Car_num_Route - MAX_GROUP_NUM; i < k ;i++){
			Route_List [Car_num_Route_List + i] = Route [MAX_GROUP_NUM + i] ;
		}
	}else{
		Route_List [Route_ID[0]] = Route [0];
		Route_List [Route_ID[1]] = Route [1];
	}
}



void Free_Route (struct Car_route* Route_free){/*�ͷ�·����ռ�õ��ڴ�ռ�*/ 
	struct Time_Node* free_point1 = NULL, *free_point2 = NULL;
	free_point1 = Route_free->HEAD_Time_Node_point;
		while (free_point1) {
			free_point2 = free_point1->Next_Time_Node;
			free(free_point1);
			free_point1 = free_point2;
	    }
}


double Calculate_Group_Cost (struct Data_str* Data_struct, struct Array_str* Array_Struct, struct Car_route** Route)/*��������·�ߵ��ܳɱ�������·�����ܳɱ�����*/ 
{
	int Car_Num = 0,i = 0;
	double Group_cost = 0;
	while (Route[i]){
		i++;
	};
	Car_Num = i-1;
	for (i = 0;i<=Car_Num;i++){
		Group_cost += part_check (Route[i],Data_struct,Array_Struct);
	}
	return Group_cost;
}



int Extract_ID ( struct Car_route** Storage ,int *temperory_ID,struct Data_str* Data_struct)/*���ڽ������ĵ��������Ϣ �����еĵ��������Ϣ�г�ȡ������ר�Ŵ�ŵ�temperory_ID[]��*/ 
//Ŀǰ��������ϣ������ʵ���д��һ�㣬��������*
{
	int First_charger = Data_struct->Receiver_num + Data_struct->Sender_num + WAREHOUSE_NUM;
	struct Time_Node *temp;
	int i = 0,j=0;
	
//	temp = (struct Time_Node*) malloc (sizeof(struct Time_Node));
	
	while (j<MAX_GROUP_NUM){
			temp = Storage[j]->HEAD_Time_Node_point; 
			for ( ;temp != NULL; )
 			{
 				if (temp->Now_node->ID < First_charger && temp->Now_node->ID > WAREHOUSE){
 					temperory_ID[i++] = temp->Now_node->ID;/*û��ð������*/
				 }
				temp = temp->Next_Time_Node;
			}
			j++;
	}
	
	int len = i, k,temp_num,Storage_ID = 0;
	for (k = 1;k<len;k++ ){
		for (j = k-1; j<len;j++){
			if (temperory_ID[j] < temperory_ID[Storage_ID]){
				Storage_ID = j;
			}
		}
		temp_num = temperory_ID[Storage_ID];
		temperory_ID[Storage_ID] = temperory_ID[k-1];
		temperory_ID[k-1] = temp_num;
	}
	
	return i;
}



double part_check ( struct Car_route *route,struct Data_str* Data_struct, struct Array_str* Array_Struct) /*1. ���·�߿�����
																											2. ����������������·�ĳɱ�*/ 
//��ɱ��������ϵ���0�������ܷ��سɱ� 
{ 
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	double distance;/*�ܾ���*/
	double trans_cost;/*�ܾ���ɱ�*/ 
	int charge_cost;/*���ɱ�*/ 
	double wait_cost;/*�ȴ��ɱ�*/ 
	int fixed_use_cost;/*�̶��ɱ�*/ 
	double total_cost;/*�ܳɱ�*/ 
	int charge_cnt;/*������*/ 
	
	double check_distance;
	double check_weight;
	double check_volume;
	double final_cost = 0;
	int arr0;
	int last_place;
	//int i;
	//int x;
	int place_number;
	struct Time_Node *temp;
	short flag = 1;
	
	struct Time_Node *temp0;
	
	int j=0;
	
    arr0=0;   
			
	if ( route->type==IVECO )   //////
	{
		check_distance = I_driving_range;
		check_weight = I_max_volume;
		check_volume = I_max_weight;
	}
			
	if ( route->type==TRUCK )   //////
	{
		check_distance = T_driving_range;
		check_weight = T_max_volume;
		check_volume = T_max_weight;
	}
			 
	distance = calculate_distance(route,Array_Struct);
	trans_cost = calculate_trans_cost(route->type,distance);
	int distribute_lea_tm = route->HEAD_Time_Node_point->Arrived_time ;  
		    
	place_number = 1; 
			
	fixed_use_cost = calculate_fixed_use_cost(route->type);
			
	temp = route->HEAD_Time_Node_point;
						
	charge_cnt = 0;
	wait_cost = 0;
	last_place = temp->Now_node->ID;
			
    temp0 = temp->Next_Time_Node;
            
	if(temp0->Arrived_time < temp0->Now_node->first_receive_tm)
    {
        return 0;/*�������жϵ�һ����ĵ������*/ 
	}
			
	while ( temp )
	{	
        place_number = place_number + 1;
                
		check_distance = check_distance - Distance[last_place][temp->Now_node->ID];
        check_weight = check_weight - Node[temp->Now_node->ID]->pack_total_weight;
        check_volume = check_volume - Node[temp->Now_node->ID]->pack_total_volume;
			    
		last_place = temp->Now_node->ID;
			    
        if ( check_distance<0 )
        { 
            return 0;
		}
				
//		if ( check_weight<0 )
//        { 
//            return 0;
//		}
//                
//        if ( check_weight<0 )
//        {
//            flag = 0; 
//            return 0;
//		}
//				
		if ( temp->Now_node->type== WAREHOUSE_TYPE )
        {
            arr0 = arr0 + 1;
            if ( route->type== IVECO)
            {
               	check_distance = 100000.0;
			}
                	
            if ( route->type== TRUCK)
            {
                check_distance = 120000.0;
			} 
		}
                
        if ( temp->Now_node->type== CHARGER_TYPE )
        {
            if ( route->type== IVECO)
            {
                check_distance = 100000.0;
			}
                	
            if ( route->type== TRUCK)
            {
                check_distance = 120000.0;
			}
			charge_cnt = charge_cnt + 1;
		}
				
		if ( temp->Now_node->type== RECEIVER_TYPE || temp->Now_node->type== SENDER_TYPE)
		{
			if ( temp->Arrived_time > temp->Now_node->last_receive_tm ) 
			{
				return 0;
			}
		    else
			{
				if ( temp->Arrived_time < temp->Now_node->first_receive_tm )
				{
					wait_cost = wait_cost + calculate_wait_cost(temp->Now_node->first_receive_tm,temp->Arrived_time);					
				}
			}
		}        
		temp = temp->Next_Time_Node;
	}	
			
    wait_cost = wait_cost + 24 * (arr0 - 2);
			
	charge_cost = calculate_charge_cost(charge_cnt); 
	total_cost = calculate_total_cost(trans_cost,charge_cost,wait_cost,fixed_use_cost);
	return 	total_cost;		
}



struct Car_route** Recombinant(struct Recombinant_ID_array* ID_array, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*���ض��ĵ��������(recombinant)*/ 
{
	struct All_Data* Recombinant_All_data = Translator_ID_array2Array_str(ID_array, Data_struct, Array_Struct);
	int Car_num = Find_MapRoute((Recombinant_All_data->Data_struct), (Recombinant_All_data->Array_Struct));/*�����¼���ó�����Ŀ����û�з��أ�
																									   �ϲ㺯�ж�ָ���Ƿ�Ϊ��*/
	return (Recombinant_All_data->Array_Struct->Route_List);
}



struct All_Data* Translator_ID_array2Array_str(struct Recombinant_ID_array* ID_array, struct Data_str* Data_struct, struct Array_str* Array_Struct)
/*��ID_array �ṹ�洢�� ������� ת����Array_str �ṹ��*/
{
	struct str_node** Node = Array_Struct->Node;
	int Array_len = ID_array->Node_num,ID_type, Receiver_num = 0, Sender_num = 0;
	int i,k, All_Node_Num = Array_len + Data_struct->Charger_num + WAREHOUSE_NUM;
	int * ID_Array = ID_array->ID_head;
	int First_charger_ID = Data_struct->Receiver_num + Data_struct->Sender_num;
	struct All_Data* Recombinant_All_data = (struct All_Data*)malloc(sizeof(struct All_Data));
	struct str_node ** New_node_group = (struct str_node **)malloc(All_Node_Num *sizeof(struct str_node*));
	struct Array_str* Recombinant_Array_Struct = (struct Array_str*)malloc(sizeof(struct Array_str));
	struct Data_str* Recombinant_Data_strcut = (struct Data_str*)malloc(sizeof(struct Data_str));
	//New_node_group[WAREHOUSE] = Copy_Node_str_data(Node[WAREHOUSE]);
	New_node_group[WAREHOUSE] = Node[WAREHOUSE];
	New_node_group[WAREHOUSE]->Tell_pass = 0;
	for (i = 1; i <= Array_len; i++) {
		ID_type = Tell_ID_type(ID_Array[i-1], Data_struct);
		switch (ID_type) {
		case RECEIVER_TYPE:
		{
			Receiver_num++;
			break;
		}
		case SENDER_TYPE: 
		{
			Sender_num++;
			break;
		}
		default:
		{
			printf("�������Ż��������У����в����ͻ�����������������飡\nNode_ID = %d\tID_type = %d\n", ID_Array[i], ID_type);
			system("pause");
			break;
		}
		}
		New_node_group[i] = Node[ID_Array[i-1]];//����Ӧ��ע��tell_pass Ӧ�ñ���Ϊ0����ʾδǰ��
		New_node_group[i]->Tell_pass = 0; 
		//New_node_group[i] = Copy_Node_str_data(Node[ID_Array[i]]);
	}
	for (i = Array_len+1,k = 1; i < All_Node_Num;i++,k++) {
		New_node_group[i] = Node[First_charger_ID + k];
		New_node_group[i]->Tell_pass = 0;
		//New_node_group[i] = Copy_Node_str_data(Node[First_charger_ID + k]);
	}
	Recombinant_Data_strcut->Charger_num = Data_struct->Charger_num;
	Recombinant_Data_strcut->Data_ID = -1;/*-1�����ڲ��Ż���⣬��������ⲿ����*/
	Recombinant_Data_strcut->Receiver_num = Receiver_num;
	Recombinant_Data_strcut->Sender_num = Sender_num;
	Recombinant_Array_Struct->Node = New_node_group;
	Recombinant_Array_Struct->Time = Array_Struct->Time;
	Recombinant_Array_Struct->Distance = Array_Struct->Distance;
	Recombinant_Array_Struct->Route_List = (struct Car_route**)malloc((Receiver_num + Sender_num)*sizeof(struct Car_route*));
	Recombinant_All_data->Array_Struct = Recombinant_Array_Struct;
	Recombinant_All_data->Data_struct = Recombinant_Data_strcut;
	return Recombinant_All_data;
}



struct str_node * Copy_Node_str_data (struct str_node * Fountain )/*���� Node_str ����Ϣ����Fountain��Copy_Node_str_data��*/ 
																		/*���Ƶ�ʱ��tell_passһ��Ҫ������Ϊδ����
																			   ��������������һ����������ȷ���Ƿ�ȫ�̸���
																			   Ŀǰ��δ������һ������*/
{
	struct str_node * Replica = (struct str_node *)malloc(sizeof(struct str_node));
	Replica->first_receive_tm = Fountain->first_receive_tm;
	Replica->last_receive_tm = Fountain->last_receive_tm;
	Replica->ID = Fountain->ID;
	Replica->pack_total_volume = Fountain->pack_total_volume;
	Replica->pack_total_weight = Fountain->pack_total_weight;
	Replica->type = Fountain->type;
	Replica->Tell_pass = 0;
	return Replica;
}
