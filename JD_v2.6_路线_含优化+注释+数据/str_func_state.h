/*#define MAX 1101*//*这些参数要改，可能在main函数中做结构体调用*/
//#define MAX_CAR_NUM 1000 

struct Data_str {
	int Data_ID;
	/*char Node_File[30];
	char Dis_Time_File[30];*/
	int Receiver_num;
	int Sender_num;
	int Charger_num;
};

struct Array_str {/*用来取代全集变量使用的*/
	struct str_node** Node;
	int ** Distance;
	int ** Time;
	struct Car_route** Route_List;
};

struct All_Data {
	struct Data_str* Data_struct;
	struct Array_str* Array_Struct;
};


struct Recombinant_ID_array {
	int* ID_head;
	int Node_num;
};


struct str_node/*每一个点的信息*/
{/*以后店面也要区分是收货者or揽货者这里在type上可以采用两位数储存方法；或者考虑节约内存时，使用位域的方式；同时在对变量赋值的时候也可以
   适当的使用枚举等相关的语法，这个语法不是特别熟悉*/
	int ID;
	int type;
	double pack_total_weight;
	double pack_total_volume;
	int first_receive_tm;
	int last_receive_tm;
	short Tell_pass;
} /**Node[MAX]*/;

struct Time_Node {/*用来包括点同时连接起来使用的结构*/
	struct str_node* Now_node;
	int Array_ID;
	short Arrived_time;
	struct Time_Node * Next_Time_Node;
};

//struct distance_time/*两点间距离和时间的，的数据结构；这里考虑使用上or下三角矩阵代替一个矩阵（也可以考虑使用二维数组代替一下）*/
//{
//	int ID;
//	short from_node;
//	short to_node;
//	int distance;
//	int spend_tm;
//
//}*Dis_tm[MAX*MAX];


struct Blank_WV_str {
	double Blank_weight;
	double Blank_volume;
	struct Blank_WV_str* Next_Blank_struct;
};

struct Car_route/*对派出的每一台车信息的记录 和 车辆路径存储是*/
{
	int ID;
	int type;
	double Blank_weight;
	double Blank_volume;
	double Offset_weight;
	double Offset_volume;
	int left_mileage;
	int car_now_time;
	int now_positon;
	int pass_warehouse_time;
	struct Blank_WV_str * blank_data;
	struct Time_Node * HEAD_Time_Node_point;
	struct Time_Node * Time_Node_point;
}/**Route_List[MAX_CAR_NUM]*/;

struct trans_delivery/*用来处理成本计算使用的结构体、可以了解一下、考虑是否需要，还有内存的使用情况*/
{
	int trans_code[4];
	short vehicle_type;
	int dis_seq[20];
	int distribute_lea_tm;
	int distribute_arr_tm;
	double distance;
	double trans_cost;
	int charge_cost;
	double wait_cost;
	int fixed_use_cost;
	double total_cost;
	int charge_cnt;
};
struct trans_delivery**Result; /* 在check_count.c中开辟了空间 struct trans_delivery*Result[1001]*/




int car_arr_places_num[1001];
int **car_go_num;/*在check_count.c中开辟了空间int *car_go_num[MAX]; MAX 1101*/
double Final_total_cost;
int max_car_go_num;


/*solve.c*/
void Solve(struct Data_str* Data_struct);/*给出数据内部结构（各组分数量）*/
struct Data_str* Create_Data_str(int Data_str_num);/*计划：从外部txt文档中读取数据内部结构*/
struct Array_str* Create_Array_str(struct Data_str* Data_struct);/*将原来的全局变量化为一些可用于函数间传递的参数，保障数据安全*/
void Clean_Array_str_Time_Dis(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*清空Node、Time、Distance、Car_Route等相关的数组*/


/*solution_write.c*/
void Find_Putout_file_name(char * file_name, struct Data_str* Data_struct);
void solution_write(struct Data_str* Data_struct);


/*read_node.c*/
void Find_Node_file_name(char * file_name, struct Data_str* Data_struct);/*根据给定的数据的结构找到相对应的	Node文件*/
void fpRead(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*读入每个点的信息*/
void cheak_read_node(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*检查点的读入是否正确*/
void Clean_Node_str(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*释放存储点的信息的内存空间*/

/*read_dis_tm.c*/
int minute(int hour, int minute);/*将正常的00:00格式的时间转化为分钟格式*/
void Find_TimDis_file_name(char * file_name, struct Data_str* Data_struct);/*根据给定的数据的结构找到相对应的Time_Dis文件*/
void fpRead_Dis_tm(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*读入两点间时间和距离*/
//int Distance(int from_node, int to_node);/*得到两点之间的距离，目前是基于调用函数计算的，未来改成二维数组*/
//int Time(int from_node, int to_node);/*同上*/
void cheak_read_distm(struct Array_str* Array_Struct);/*检查点间时间、距离的读入是否正确*/


 /*find.c*/
double Print_DATA(void);/*输出全局变量的数值*/
int Balence_Time(struct Car_route* Car, int Node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*处理等待时间的函数*/
void Clean_Global_Storage(void);/*清空全局变量：ALL-XXX系列*/
void Clean_MapRoute(int All_car_num, struct Array_str* Array_Struct);/*释放存储车辆路径的信息的内存空间*/
int Clean_Route_List(struct Data_str* Data_struct, struct Array_str* Array_Str);/*清空结果的输出表（全局变量->这个全局变量要有改变可能在main函数中调用或者单独写一个出来）*/
int Find_MapRoute(struct Data_str* Data_struct, struct Array_str* Array_Str);/*找到所有车的各个路径，跑完所有的点*/
int Find_Car_route(struct Car_route * Car, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*找到但台车的路径*/
int Find_nearest_charger(int now_node, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*找到最近的充电站*/
void Find_nearest_receiver(struct Car_route* Car, int *Near_Receiver_array, int Passed_Receiver_num, struct Data_str* Data_struct, struct Array_str* Array_Struct);
 /*(up)找到最近的接收者；未来还要在揽货和收货之间选择*/

int Find_nextnode(struct Car_route *  Car, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*找到去的下一个点*/
int Tell_GOorNOT(struct Car_route *  Car, int Next_node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*判断下一个点去不去*/
void Arrive_Node(struct Car_route *  Car, struct Time_Node * T_Node, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*到达指定点，并完成相关参数的修改*/
struct Car_route* Add_New_Car(int car_ID, int car_type, int car_time, struct Array_str* Array_Str);/*添加一台新车，并将新车部分参数初始化*/
int choose_car_type(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*选择车型*/
int Tell_ID_type(int node_ID, struct Data_str* Data_struct);/*判断某一个点的种类，这个在未来是一件比较值得注意的事情，因为总数在变所以应该注意相关选择判断范围也在变*/
struct Time_Node* NtoTN_Transform(int Node_ID, struct Array_str* Array_Struct);/*将点打包成为需要连接起来的结构体*/
struct Car_route* Solve_Cant_Go_Place(int Cant_Go_ID, int Car_ID);/*这个函数用于解决无法到达的部分点；
																   未来无法到达的点应该再从check函数返回来指导开始的选择*/

/*Choose_Car_Type（new）*/
void Clean_Global_Storage_CC (void);
void Free_Test_Route (struct Car_route * Car);
void Arrive_Node_CC(struct Car_route *  Car, struct Time_Node * T_Node, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*选车函数中的模拟到达*/
int Find_Car_route_CC(struct Car_route * Car, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*选车函数中的模拟寻径*/

/*check_count.c*/
void Create_Check_Area(struct Data_str* Data_struct);
void Clean_Check_Area(void);
void final_check(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*检查路线是否符合规定的函数*/


/*optimization.c*/
struct str_node * Copy_Node_str_data(struct str_node * Fountain);
struct All_Data* Translator_ID_array2Array_str(struct Recombinant_ID_array* ID_array, struct Data_str* Data_struct, struct Array_str* Array_Struct);
struct Car_route** Recombinant(struct Recombinant_ID_array* ID_array, struct Data_str* Data_struct, struct Array_str* Array_Struct);

int Optimization (struct Data_str* Data_struct, struct Array_str* Array_Struct,int* Route_ID);
void Free_Route (struct Car_route* Route_free);
double Calculate_Group_Cost (struct Data_str* Data_struct, struct Array_str* Array_Struct, struct Car_route** Route);
int Extract_ID ( struct Car_route** Storage ,int *temperory_ID,struct Data_str* Data_struct);
double part_check( struct Car_route *route,struct Data_str* Data_struct, struct Array_str* Array_Struct);

/*check_count.c*/
int check_first(struct Data_str* Data_struct, struct Array_str* Array_Struct);
double calculate_total_cost(double trans_cost, int charge_cost, double wait_cost, int fixed_use_cost);
int calculate_charge_cost(int charge_cnt);
double calculate_wait_cost(int first_receive_tm, int car_now_time);
int calculate_fixed_use_cost(short vehicle_type);
double calculate_trans_cost(short vehicle_type, int distance);
int check_volume(short vehicle_type, double go_volume);
int check_weight(short vehicle_type, double go_weight);
double calculate_distance(struct Car_route *Route, struct Array_str* Array_Struct);
void final_check(struct Data_str* Data_struct, struct Array_str* Array_Struct);
void Replace (struct Car_route** Route_List, struct Car_route** Route, int* Route_ID);
