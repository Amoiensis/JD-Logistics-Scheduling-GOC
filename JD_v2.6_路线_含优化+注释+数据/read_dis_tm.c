#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#define TIMDIS_FILE_NAME_LEN 29



void Find_TimDis_file_name(char * file_name, struct Data_str* Data_struct)/*用于获得 待读取（存有点间 时间、距离）的文件的文件名*/ 
{
	strcpy(file_name, "inputdistancetime_");
	char temp_num[5];
	char temp__[2] = "_";
	char temp_txt[5] = ".txt";
	int Num = Data_struct->Data_ID;
	_itoa(Num, temp_num, 10);
	strcat(file_name, temp_num);
	strcat(file_name, temp__);
	Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	_itoa(Num, temp_num, 10);
	strcat(file_name, temp_num);
	strcat(file_name, temp_txt);
}



void fpRead_Dis_tm(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*将 点间距离、时间 信息读入*/ 
{
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int i,j,All_Node_Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	int temp_ID, temp_from_node, temp_to_node;
	FILE *fp_read, *fp0;
	char file_name[TIMDIS_FILE_NAME_LEN];

	Find_TimDis_file_name(file_name, Data_struct);
	//printf("%s\n", file_name);

	char first_char;
	fp_read = fopen(file_name, "r");
	fp0 = fopen(file_name, "r");

	if (fp_read == NULL)
	{
		printf("无法打开该文件\n");
		return;
	}

	fscanf(fp0, "c", &first_char);/*这句话不理解，不明白其作用*/

	/*******************************************/
	if (first_char >= '0' && first_char <= '9')/*这里是用不上的，但是暂时还是保留着，用来防止出现txt文档内的格式保存不当*/
	{
		for (i = 0; i<All_Node_Num; i++)
		{
			for (j = 0;j<All_Node_Num;j++) {
				if (i != j) {
                             fscanf(fp_read, "%d,%d,%d,%d,%d", &temp_ID, &temp_from_node,
				               &temp_to_node, &Distance[i][j],&Time[i][j]);
				}
				else {
					Distance[i][j] = 0;
					Time[i][j] = 0;
				}
                      
			}			
		}
	}
	/*******************************************/

	if (!(first_char >= '0' && first_char <= '9'))
	{
		fscanf(fp_read, "%*[^\n]");
		for (i = 0; i<All_Node_Num; i++)
		{
			for (j = 0; j<All_Node_Num; j++) {
				if (i != j) {
					fscanf(fp_read, "%d,%d,%d,%d,%d", &temp_ID, &temp_from_node,
						&temp_to_node, &Distance[i][j], &Time[i][j]);
				}
				else {
					Distance[i][j] = 0;
					Time[i][j] = 0;
				}

			}
		}
	}

	fclose(fp_read);
	fclose(fp0);
}



void cheak_read_distm(struct Array_str* Array_Struct)/*用于检查 点间时间、距离信息 读入是否出错*/ 
{
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int i,j;
	for (i = 0; i<2; i++)
	{
		for (j = 0; j < 2; j++) {
            printf("D:%d-T:%d\t", Distance[i][j], Time[i][j]);
		}
		printf("\n");
	}
}


//int Distance(int from_node, int to_node)
//{
//
//	if (from_node == to_node) {
//		return 0;
//	}
//	else {
//		int k = 0;
//		if (from_node<to_node) {
//			k = from_node * 1100 + to_node - 1;
//		}
//		if (from_node>to_node) {
//			k = from_node * 1100 + to_node;
//		}
//		return (Dis_tm[k]->distance);
//	}
//
//}
//
//int Time(int from_node, int to_node)
//{
//	if (from_node == to_node) {
//		return 0;
//	}
//	else {
//		int k = 0;
//		if (from_node<to_node) {
//			k = from_node * 1100 + to_node - 1;
//		}
//		if (from_node>to_node) {
//			k = from_node * 1100 + to_node;
//		}
//		return (Dis_tm[k]->spend_tm);
//	}
//}

