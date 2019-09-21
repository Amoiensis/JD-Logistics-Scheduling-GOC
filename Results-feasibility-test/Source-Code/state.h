#pragma once
/*ʱ������*/
#define START_TIME 480
#define FINAL_TIME 1400
#define UNLOAD_TIME 30 
#define CHARGE_TIME 30
#define RESET_TIME 60 

/*���͵���Ϣ����*/
#define WAREHOUSE 0 
#define WAREHOUSE_NUM 1
#define First_Receiver 1
#define Last_Receiver 1000
#define First_Charger 1001
#define Last_Charger 1100
#define RECEIVER_NUM 1000
#define CHARGER_TYPE 4
#define SENDER_TYPE 3/*����Ҫע��û�����ѡ��*/
#define RECEIVER_TYPE 2
#define WAREHOUSE_TYPE 1 


/*������Ϣ����*/
#define IVECO 1
#define I_max_volume 12
#define I_max_weight 2
#define I_driving_range 100000
#define TRUCK 2
#define T_max_volume 16
#define T_max_weight 2.5
#define T_driving_range 120000
#define First_ID 0  
#define First_use 1
//#define MAX_CAR_NUM 1000 
/*�������·*/
#define MAX_ROUTE_NUM 1000
#define MAX_NODE_NUM 20/*һ��·�����20����*/

/*�ṹ������*/
struct Array_str {/*����ȡ��ȫ������ʹ�õ�*/
	struct str_node** Node;
	int ** Distance;
	int ** Time;
	struct Car_route** Route_List;
};

struct Data_str {
	int Data_ID;
	/*char Node_File[30];
	char Dis_Time_File[30];*/
	int Receiver_num;
	int Sender_num;
	int Charger_num;
};

struct str_node/*ÿһ�������Ϣ*/
{
	int ID;
	int type;
	double pack_total_weight;
	double pack_total_volume;
	int first_receive_tm;
	int last_receive_tm;
	short Tell_pass;
} /**Node[MAX]*/;

struct Solved_Route {
	int Car_type;
	int Start_time;
	int * Pass_node;/*�������·�ϵ�Node*/
	//struct Solved_Route *Next_Route��
};

struct Cost {
	double Fix_cost;
	double Dis_cost;
	double Wait_cost;
	double Charge_cost;
};

/*��������*/
/*����Node��Ϣ*/
struct Data_str* Create_Data_str(int Data_str_num);
struct Array_str* Create_Array_str(struct Data_str* Data_struct);
int minute(int hour, int minute);
void Find_Node_file_name(char * file_name, struct Data_str* Data_struct);
void fpRead(struct Data_str* Data_struct, struct Array_str* Array_Struct);
void cheak_read_node(struct Data_str* Data_struct, struct Array_str* Array_Struct);
void Clean_Node_str(struct Data_str* Data_struct, struct Array_str* Array_Struct);
/*����Distance_Time��Ϣ*/
void Find_TimDis_file_name(char * file_name, struct Data_str* Data_struct);
void fpRead_Dis_tm(struct Data_str* Data_struct, struct Array_str* Array_Struct);
void cheak_read_distm(struct Array_str* Array_Struct);
/*��������csv�ļ�*/
FILE * Open_csv(void);
int Read_cvs(struct Solved_Route** Route_store);
/*����ļ�*/
int Clean_Route_store(struct Solved_Route** Route_store);
int Check(struct Cost* cost);
int Check_route(struct Solved_Route* Route, int* state, int Node_state[], struct Data_str* Data_struct, struct Array_str* Array_struct, struct Cost* cost);
double Print_Cost (struct Cost* cost);
