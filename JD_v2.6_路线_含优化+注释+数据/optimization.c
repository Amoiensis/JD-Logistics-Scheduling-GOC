#include <stdio.h>
#include <stdlib.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#include "time_distance.h"



int Optimization (struct Data_str* Data_struct, struct Array_str* Array_Struct,int* Route_ID)/*用于按照 特定优化方案 进行结果路线簇优化*/ 
{
	int DoneorNot = 0;/*0表示没有优化，1表示优化了*/
	struct Car_route** Route_List = Array_Struct->Route_List;
	struct Car_route* Storage [MAX_GROUP_NUM];
	Storage[0] = Route_List[Route_ID[0]];
	Storage[1] = Route_List[Route_ID[1]];
	int temperory_ID[MAX_RECOMBINANT_NODE_NUM] = { 0 }; //这个数目考虑使用 define来解决
	int temperory_ID_len = Extract_ID(Storage, temperory_ID, Data_struct);
	struct Recombinant_ID_array* ID_array = (struct Recombinant_ID_array*)malloc(sizeof(struct Recombinant_ID_array));
	ID_array->ID_head = temperory_ID;
	ID_array->Node_num = temperory_ID_len;
	struct Car_route** Route = Recombinant(ID_array, Data_struct, Array_Struct);//新得到的路径

	int i =0;
	double Cost_original = 0, Cost_optimize = 0;
	while (i<2){
		Cost_original += part_check(Storage[i++],Data_struct,Array_Struct);//Route_List[i++]
	}
	Cost_optimize = Calculate_Group_Cost(Data_struct,Array_Struct,Route);
	if(Cost_optimize <= Cost_original){/*这里相等也支持变动*/
		Free_Route (Storage[0]);
		Free_Route (Storage[1]);
//		Route_List[0] = Route[0];/*这里可能出现优化后路线增加的情况，应该解决*/ 
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



void Replace (struct Car_route** Route_List, struct Car_route** Route, int* Route_ID)/*将优化后的路线替换原本的路线*/ 
/*参数意义：原本总的车辆数，新的部分路线，原来用于重组的路线的序号*/
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



void Free_Route (struct Car_route* Route_free){/*释放路线所占用的内存空间*/ 
	struct Time_Node* free_point1 = NULL, *free_point2 = NULL;
	free_point1 = Route_free->HEAD_Time_Node_point;
		while (free_point1) {
			free_point2 = free_point1->Next_Time_Node;
			free(free_point1);
			free_point1 = free_point2;
	    }
}


double Calculate_Group_Cost (struct Data_str* Data_struct, struct Array_str* Array_Struct, struct Car_route** Route)/*计算所有路线的总成本（即，路线组总成本）。*/ 
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



int Extract_ID ( struct Car_route** Storage ,int *temperory_ID,struct Data_str* Data_struct)/*用于将给定的点的属性信息 从所有的点的属性信息中抽取出来，专门存放到temperory_ID[]内*/ 
//目前是两两混合，可以适当的写大一点，可以组混合*
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
 					temperory_ID[i++] = temp->Now_node->ID;/*没有冒泡排序*/
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



double part_check ( struct Car_route *route,struct Data_str* Data_struct, struct Array_str* Array_Struct) /*1. 检查路线可行性
																											2. 计算出给定点组成线路的成本*/ 
//算成本，不符合的是0，可以跑返回成本 
{ 
	struct str_node** Node = Array_Struct->Node;
	int ** Distance = Array_Struct->Distance;
	double distance;/*总距离*/
	double trans_cost;/*总距离成本*/ 
	int charge_cost;/*充电成本*/ 
	double wait_cost;/*等待成本*/ 
	int fixed_use_cost;/*固定成本*/ 
	double total_cost;/*总成本*/ 
	int charge_cnt;/*充电次数*/ 
	
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
        return 0;/*这里在判断第一个点的到达情况*/ 
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



struct Car_route** Recombinant(struct Recombinant_ID_array* ID_array, struct Data_str* Data_struct, struct Array_str* Array_Struct)/*将特定的点重新组合(recombinant)*/ 
{
	struct All_Data* Recombinant_All_data = Translator_ID_array2Array_str(ID_array, Data_struct, Array_Struct);
	int Car_num = Find_MapRoute((Recombinant_All_data->Data_struct), (Recombinant_All_data->Array_Struct));/*这里记录了用车的数目但是没有返回，
																									   上层函判断指针是否为空*/
	return (Recombinant_All_data->Array_Struct->Route_List);
}



struct All_Data* Translator_ID_array2Array_str(struct Recombinant_ID_array* ID_array, struct Data_str* Data_struct, struct Array_str* Array_Struct)
/*将ID_array 结构存储的 点的性质 转换成Array_str 结构中*/
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
			printf("待重组优化的数组中，含有不是送货和揽货的情况！请检查！\nNode_ID = %d\tID_type = %d\n", ID_Array[i], ID_type);
			system("pause");
			break;
		}
		}
		New_node_group[i] = Node[ID_Array[i-1]];//这里应该注意tell_pass 应该被置为0，表示未前往
		New_node_group[i]->Tell_pass = 0; 
		//New_node_group[i] = Copy_Node_str_data(Node[ID_Array[i]]);
	}
	for (i = Array_len+1,k = 1; i < All_Node_Num;i++,k++) {
		New_node_group[i] = Node[First_charger_ID + k];
		New_node_group[i]->Tell_pass = 0;
		//New_node_group[i] = Copy_Node_str_data(Node[First_charger_ID + k]);
	}
	Recombinant_Data_strcut->Charger_num = Data_struct->Charger_num;
	Recombinant_Data_strcut->Data_ID = -1;/*-1代表内部优化检测，不会调用外部数据*/
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



struct str_node * Copy_Node_str_data (struct str_node * Fountain )/*复制 Node_str 的信息（从Fountain到Copy_Node_str_data）*/ 
																		/*复制的时候，tell_pass一律要求设置为未过，
																			   这个函数可以添加一个参数用于确定是否全盘复制
																			   目前暂未设置这一个参数*/
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
