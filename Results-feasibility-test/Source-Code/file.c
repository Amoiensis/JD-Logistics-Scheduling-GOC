#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"
#define ZOOM sizeof(struct str_node)
#define NODE_FILE_NAME_LEN 22
#define TIMDIS_FILE_NAME_LEN 29

/*读入Node信息*/
struct Data_str* Create_Data_str(int Data_str_num)
{
	struct Data_str* Data_struct = (struct Data_str*)malloc(sizeof(struct Data_str));
	Data_struct->Data_ID = 0;
	Data_struct->Receiver_num = 1000;
	Data_struct->Sender_num = 0;
	Data_struct->Charger_num = 100;
	return Data_struct;
}

struct Array_str* Create_Array_str(struct Data_str* Data_struct)
{
	int All_Node_num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	int Receiver_Sender_num = Data_struct->Receiver_num + Data_struct->Sender_num;
	struct Array_str* Array_Str = (struct Array_str*)malloc(sizeof(struct Array_str));
	Array_Str->Node = (struct str_node**)malloc(All_Node_num * sizeof(struct str_node*));
	int ** Array = (int**)malloc(All_Node_num * sizeof(int*));
	int i;
	for (i = 0; i<All_Node_num; i++) {
		Array[i] = (int*)malloc(All_Node_num * sizeof(int));
	}
	Array_Str->Distance = Array;
	Array = (int**)malloc(All_Node_num * sizeof(int*));
	for (i = 0; i<All_Node_num; i++) {
		Array[i] = (int*)malloc(All_Node_num * sizeof(int));
	}
	Array_Str->Time = Array;
	Array_Str->Route_List = (struct Car_route**)malloc(Receiver_Sender_num * sizeof(struct Car_route*));
	return Array_Str;
}

int minute(int hour, int minute)
{
	return (hour * 60 + minute);
}

void Find_Node_file_name(char * file_name, struct Data_str* Data_struct)
{
	strcpy(file_name, "inputnode_");
	char temp_num[5];
	char temp__[2] = "_";
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

void fpRead(struct Data_str* Data_struct, struct Array_str* Array_Struct)
{
	struct str_node** Node = Array_Struct->Node;
	int i, All_Node_Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	int hour0, hour1, minute0, minute1;
	FILE *fp_read, *fp0;
	char file_name[NODE_FILE_NAME_LEN];
	Find_Node_file_name(file_name, Data_struct);
	//printf("%s\n", file_name);

	char first_char;
	fp_read = fopen(file_name, "r");
	fp0 = fopen(file_name, "r");

	if (fp_read == NULL)
	{
		printf("无法打开\"inputnode_x_x.txt\"文件，请检查文件名！\n");
		return;
	}

	fscanf(fp0, "%c", &first_char);/*将第一行的英文标题读取掉*/

	if (first_char >= '0' && first_char <= '9')/*这里是用不上的，但是暂时还是保留着，用来防止出现txt文档内的格式保存不当*/
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

void cheak_read_node(struct Data_str* Data_struct, struct Array_str* Array_Struct)
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

void Clean_Node_str(struct Data_str* Data_struct, struct Array_str* Array_Struct)
{
	struct str_node** Node = Array_Struct->Node;
	int i, All_Node_Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
	for (i = 0; i < All_Node_Num; i++) {
		free(Node[i]);
	}
}

/*读入Distance_Time信息*/
void Find_TimDis_file_name(char * file_name, struct Data_str* Data_struct)
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

void fpRead_Dis_tm(struct Data_str* Data_struct, struct Array_str* Array_Struct)
{
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int i, j, All_Node_Num = Data_struct->Receiver_num + Data_struct->Sender_num + Data_struct->Charger_num + WAREHOUSE_NUM;
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
		printf("无法打开\"inputdistancetime_x_x.txt\"文件\n");
		return;
	}

	fscanf(fp0, "c", &first_char);/*这句话不理解，不明白其作用*/

								  /*******************************************/
	if (first_char >= '0' && first_char <= '9')/*这里是用不上的，但是暂时还是保留着，用来防止出现txt文档内的格式保存不当*/
	{
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

void cheak_read_distm(struct Array_str* Array_Struct)
{
	int ** Distance = Array_Struct->Distance;
	int ** Time = Array_Struct->Time;
	int i, j;
	for (i = 0; i<2; i++)
	{
		for (j = 0; j < 2; j++) {
			printf("D:%d-T:%d\t", Distance[i][j], Time[i][j]);
		}
		printf("\n");
	}
}

/*读入待检测csv文件*/
FILE * Open_csv(void)
{
	char * file_name = "Route.csv";
	FILE *fp_read;
	fp_read = fopen(file_name, "r");
	if (fp_read == NULL)
	{
		printf("无法打开\"Route.csv\"文件，请检查文件名！\n");
		system("pause");
	}
	return fp_read;
}

int Read_cvs(struct Solved_Route** Route_store)
{
	int Route_num,Node_num/*每一条路线中的点数*/;
	FILE *fp0,*fp_read = Open_csv();
	fp0 = fp_read;
	char first_char;
	fscanf(fp0, "%c", &first_char);/*将第一行的英文标题读取掉*/

	int Hour = 0, Minute = 0;
		for (Route_num = 0; Route_num< MAX_ROUTE_NUM; Route_num++)
		{
			Route_store[Route_num] = (struct Solved_Route *) malloc(sizeof(struct Solved_Route));
			int Car_type = IVECO;
			int Start_time = START_TIME;
			char Useless_data = 'a';
			Route_store[Route_num]->Pass_node = (int *)malloc(MAX_NODE_NUM*sizeof(int));
			int k;
			for (k=0;k<MAX_NODE_NUM;k++){
				Route_store[Route_num]->Pass_node[k]=0;
			}
			while (Useless_data != ',') {
				fscanf(fp_read, "%c", &Useless_data);
				if (Useless_data == '$') {
					printf("\n*****读取数据结束******\n");
					goto END;
				}
			}
			fscanf(fp_read, "%d", &Route_store[Route_num]->Car_type);
			fscanf(fp_read, "%c", &Useless_data);/*再读掉下一个逗号*/
			Useless_data = 'a';
			fscanf(fp_read,"%d",& Route_store[Route_num]->Pass_node[0]);
			fscanf(fp_read, "%c", &Useless_data);Useless_data = 'a';
			for (Node_num = 1; Useless_data != ',' && Node_num < MAX_NODE_NUM; Node_num++) {
				fscanf(fp_read, "%d", &Route_store[Route_num]->Pass_node[Node_num]);
				fscanf(fp_read, "%c", &Useless_data);
			}
			Useless_data = 'a';
			fscanf(fp_read, "%d:%d",&Hour,&Minute);
			Route_store[Route_num]->Start_time = minute(Hour, Minute);
			while (Useless_data != '\n') {
				fscanf(fp_read, "%c", &Useless_data);
			}
			/*输出测试*/
			printf("\nCar_type = %d\t", Route_store[Route_num]->Car_type);
			printf("Start_time = %d\t", Route_store[Route_num]->Start_time);
			int i;
			for (i = 0; i<Node_num; i++) {
				printf("%d->", Route_store[Route_num]->Pass_node[i]);

			}
			
			/*输出测试*/
		}
	END:;
	return Route_num;
}
