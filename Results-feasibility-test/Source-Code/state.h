#pragma once
/*时间声明*/
#define START_TIME 480
#define FINAL_TIME 1400
#define UNLOAD_TIME 30 
#define CHARGE_TIME 30
#define RESET_TIME 60 

/*配送点信息声明*/
#define WAREHOUSE 0 
#define WAREHOUSE_NUM 1
#define First_Receiver 1
#define Last_Receiver 1000
#define First_Charger 1001
#define Last_Charger 1100
#define RECEIVER_NUM 1000
#define CHARGER_TYPE 4
#define SENDER_TYPE 3/*这里要注意没有这个选项*/
#define RECEIVER_TYPE 2
#define WAREHOUSE_TYPE 1 


/*车辆信息声明*/
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
/*待检测线路*/
#define MAX_ROUTE_NUM 1000
#define MAX_NODE_NUM 20/*一条路上最多20个点*/

/*结构体声明*/
struct Array_str {/*用来取代全集变量使用的*/
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

struct str_node/*每一个点的信息*/
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
	int * Pass_node;/*储存该线路上的Node*/
	//struct Solved_Route *Next_Route；
};

struct Cost {
	double Fix_cost;
	double Dis_cost;
	double Wait_cost;
	double Charge_cost;
};

/*函数声明*/
/*读入Node信息*/
struct Data_str* Create_Data_str(int Data_str_num);
struct Array_str* Create_Array_str(struct Data_str* Data_struct);
int minute(int hour, int minute);
void Find_Node_file_name(char * file_name, struct Data_str* Data_struct);
void fpRead(struct Data_str* Data_struct, struct Array_str* Array_Struct);
void cheak_read_node(struct Data_str* Data_struct, struct Array_str* Array_Struct);
void Clean_Node_str(struct Data_str* Data_struct, struct Array_str* Array_Struct);
/*读入Distance_Time信息*/
void Find_TimDis_file_name(char * file_name, struct Data_str* Data_struct);
void fpRead_Dis_tm(struct Data_str* Data_struct, struct Array_str* Array_Struct);
void cheak_read_distm(struct Array_str* Array_Struct);
/*读入待检测csv文件*/
FILE * Open_csv(void);
int Read_cvs(struct Solved_Route** Route_store);
/*检测文件*/
int Clean_Route_store(struct Solved_Route** Route_store);
int Check(struct Cost* cost);
int Check_route(struct Solved_Route* Route, int* state, int Node_state[], struct Data_str* Data_struct, struct Array_str* Array_struct, struct Cost* cost);
double Print_Cost (struct Cost* cost);
