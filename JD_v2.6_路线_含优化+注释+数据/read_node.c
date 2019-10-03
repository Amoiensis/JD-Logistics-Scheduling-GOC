#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "str_func_state.h"
#include "nodeID_car_state.h"
#define ZOOM sizeof(struct str_node)
#define NODE_FILE_NAME_LEN 22/*�ļ�������Ϊ21*/



int minute(int hour, int minute)
{
	return (hour * 60 + minute);
}



void Find_Node_file_name(char * file_name,struct Data_str* Data_struct)/*���ڻ�� ����ȡ�����е�����ԣ����ļ����ļ���*/ 
{
	strcpy(file_name,"inputnode_");
	char temp_num [5];
	char temp__ [2] = "_";
	char temp_txt[5] = ".txt";
	int Num = Data_struct->Data_ID;
	_itoa(Num, temp_num, 10);
	strcat(file_name, temp_num);
	strcat(file_name, temp__);
	Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + 1;
	_itoa(Num, temp_num, 10);
	strcat(file_name, temp_num);
	strcat(file_name, temp_txt);
}



void fpRead(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*�����������Ϣ����*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int i, All_Node_Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	int hour0, hour1, minute0, minute1;
	FILE *fp_read, *fp0;
	char file_name [NODE_FILE_NAME_LEN];
	Find_Node_file_name(file_name, Data_struct);
	//printf("%s\n", file_name);

	char first_char;
	fp_read = fopen(file_name, "r");
	fp0 = fopen(file_name, "r");

	if (fp_read == NULL)
	{
		printf("�޷��򿪸��ļ��������ļ�����\n");
		return;
	}

	fscanf(fp0, "%c", &first_char);/*����һ�е�Ӣ�ı����ȡ��*/

	if (first_char >= '0' && first_char <= '9')/*�������ò��ϵģ�������ʱ���Ǳ����ţ�������ֹ����txt�ĵ��ڵĸ�ʽ���治��*/
	{
		for (i = 0; i< All_Node_Num; i++)
		{
			Node[i] = (struct str_node*) malloc(ZOOM);
			hour0 = 0;
			hour1 = 0;
			minute0 = 0;
			minute1 = 0;
			fscanf(fp_read, "%d", &Node[i]->ID);
			Node[i]->ID = Node[i]->ID % 10000;
			fscanf(fp_read, "%d", &Node[i]->type);
			fscanf(fp_read, "%lf", &Node[i]->pack_total_weight);
			fscanf(fp_read, "%lf", &Node[i]->pack_total_volume);
			fscanf(fp_read, "%d:%d", &hour0, &minute0);
			Node[i]->first_receive_tm = minute(hour0, minute0);
			fscanf(fp_read, "%d:%d", &hour1, &minute1);
			Node[i]->last_receive_tm = minute(hour1, minute1);
			Node[i]->Tell_pass = 0;
		}
	}

	if (!(first_char >= '0' && first_char <= '9'))
	{
		fscanf(fp_read, "%*[^\n]");
		for (i = 0; i < All_Node_Num; i++)
		{
			Node[i] = (struct str_node*) malloc(ZOOM);
			hour0 = 0;
			hour1 = 0;
			minute0 = 0;
			minute1 = 0;
			fscanf(fp_read, "%d", &Node[i]->ID);
			Node[i]->ID = Node[i]->ID % 10000;
			fscanf(fp_read, "%d", &Node[i]->type);
			fscanf(fp_read, "%lf", &Node[i]->pack_total_weight);
			fscanf(fp_read, "%lf", &Node[i]->pack_total_volume);
			fscanf(fp_read, "%d:%d", &hour0, &minute0);
			Node[i]->first_receive_tm = minute(hour0, minute0);
			fscanf(fp_read, "%d:%d", &hour1, &minute1);
			Node[i]->last_receive_tm = minute(hour1, minute1);
			Node[i]->Tell_pass = 0;
		}
	}
	fclose(fp_read);
	fclose(fp0);
}



void cheak_read_node(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*���ڷ�����ʹ����  ��ǰ�����е���� ����ʹ����ѡ��������*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int j, All_Node_Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	for (j = 0; j< All_Node_Num; j++)
	{
		printf("%d %d %lf %lf %d %d %d\n", Node[j]->ID, Node[j]->type,
			Node[j]->pack_total_weight, Node[j]->pack_total_volume,
			Node[j]->first_receive_tm, Node[j]->last_receive_tm,
			Node[j]->Tell_pass);
	}
}



void Clean_Node_str(struct Data_str* Data_struct, struct Array_str* Array_Struct)/*�ͷ� ��������Ե� �ڴ�ռ�*/ 
{
	struct str_node** Node = Array_Struct->Node;
	int i, All_Node_Num = Data_struct->Receiver_num+ Data_struct->Sender_num+ Data_struct->Charger_num + WAREHOUSE_NUM;
	for (i = 0; i < All_Node_Num; i++) {
		free(Node[i]);
	}
}
