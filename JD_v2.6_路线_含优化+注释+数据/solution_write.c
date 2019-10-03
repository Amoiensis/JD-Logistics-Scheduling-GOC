#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "str_func_state.h"
#define NODE_FILE_NAME_LEN 22/*文件名长度为21*/


void Find_Putout_file_name(char * file_name, struct Data_str* Data_struct)/*设置输出文件名称*/ 
{
	strcpy(file_name, "总路线_");
	char temp_num[5];
	char temp_txt[5] = ".txt";
	int Num = Data_struct->Data_ID;
	_itoa(Num, temp_num, 10);
	strcat(file_name, temp_num);
	strcat(file_name, temp_txt);
}

void solution_write(struct Data_str* Data_struct)/*将路线标准化格式信息输出*/ 
{
	int i, j, k;
	FILE *fp_write;
	int lea_tm_hour, lea_tm_minute;
	int arr_tm_hour, arr_tm_minute;
	/*char final_solution_name[100];*/
	int ID_num = 10000*Data_struct->Data_ID;
	printf("具体已写入：总路线_%d.txt 请查看！\n", Data_struct->Data_ID);
	char file_name[NODE_FILE_NAME_LEN];
	Find_Putout_file_name(file_name, Data_struct);

	fp_write = fopen(file_name, "w");

	fprintf(fp_write, "%lf\n", Final_total_cost);

	fputs("trans_code vehicle_type dis_seq distribute_lea_tm distribute_arr_tm distance trans_cost charge_cost wait_cost fixed_use_cost total_cost charge_cnt\n", fp_write);

	for (i = 0; i<max_car_go_num; i++)
	{
		fprintf(fp_write, "DP");

		for (j = 0; j<4; j++)
		{
			fprintf(fp_write, "%d", Result[i]->trans_code[j]);
		}

		fprintf(fp_write, " ");

		fprintf(fp_write, "%d ", Result[i]->vehicle_type);

		for (k = 1; k<*car_go_num[i]; k++)
		{
			if (k<(*car_go_num[i] - 1))
			{
				fprintf(fp_write, "%d;", ID_num+Result[i]->dis_seq[k]);
			}
			if (k == *car_go_num[i] - 1)
			{
				fprintf(fp_write, "%d ", ID_num+Result[i]->dis_seq[k]);
			}
		}

		lea_tm_hour = Result[i]->distribute_lea_tm / 60;
		lea_tm_minute = Result[i]->distribute_lea_tm % 60;
		fprintf(fp_write, "%d:%d ", lea_tm_hour, lea_tm_minute);

		arr_tm_hour = Result[i]->distribute_arr_tm / 60;
		arr_tm_minute = Result[i]->distribute_arr_tm % 60;
		fprintf(fp_write, "%d:%d ", arr_tm_hour, arr_tm_minute);

		fprintf(fp_write, "%.0lf ", Result[i]->distance);

		fprintf(fp_write, "%.2lf ", Result[i]->trans_cost);

		fprintf(fp_write, "%d ", Result[i]->charge_cost);

		fprintf(fp_write, "%.2lf ", Result[i]->wait_cost);

		fprintf(fp_write, "%d ", Result[i]->fixed_use_cost);

		fprintf(fp_write, "%.2lf ", Result[i]->total_cost);

		fprintf(fp_write, "%d\n", Result[i]->charge_cnt);
	}
}
