/*#define MAX 1101*//*��Щ����Ҫ�ģ�������main���������ṹ�����*/
//#define MAX_CAR_NUM 1000 

struct Data_str {
	int Data_ID;
	/*char Node_File[30];
	char Dis_Time_File[30];*/
	int Receiver_num;
	int Sender_num;
	int Charger_num;
};

struct Array_str {/*����ȡ��ȫ������ʹ�õ�*/
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


struct str_node/*ÿһ�������Ϣ*/
{/*�Ժ����ҲҪ�������ջ���or������������type�Ͽ��Բ�����λ�����淽�������߿��ǽ�Լ�ڴ�ʱ��ʹ��λ��ķ�ʽ��ͬʱ�ڶԱ�����ֵ��ʱ��Ҳ����
   �ʵ���ʹ��ö�ٵ���ص��﷨������﷨�����ر���Ϥ*/
	int ID;
	int type;
	double pack_total_weight;
	double pack_total_volume;
	int first_receive_tm;
	int last_receive_tm;
	short Tell_pass;
} /**Node[MAX]*/;

struct Time_Node {/*����������ͬʱ��������ʹ�õĽṹ*/
	struct str_node* Now_node;
	int Array_ID;
	short Arrived_time;
	struct Time_Node * Next_Time_Node;
};

//struct distance_time/*���������ʱ��ģ������ݽṹ�����￼��ʹ����or�����Ǿ������һ������Ҳ���Կ���ʹ�ö�ά�������һ�£�*/
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

struct Car_route/*���ɳ���ÿһ̨����Ϣ�ļ�¼ �� ����·���洢��*/
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

struct trans_delivery/*��������ɱ�����ʹ�õĽṹ�塢�����˽�һ�¡������Ƿ���Ҫ�������ڴ��ʹ�����*/
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
struct trans_delivery**Result; /* ��check_count.c�п����˿ռ� struct trans_delivery*Result[1001]*/




int car_arr_places_num[1001];
int **car_go_num;/*��check_count.c�п����˿ռ�int *car_go_num[MAX]; MAX 1101*/
double Final_total_cost;
int max_car_go_num;


/*solve.c*/
void Solve(struct Data_str* Data_struct);/*���������ڲ��ṹ�������������*/
struct Data_str* Create_Data_str(int Data_str_num);/*�ƻ������ⲿtxt�ĵ��ж�ȡ�����ڲ��ṹ*/
struct Array_str* Create_Array_str(struct Data_str* Data_struct);/*��ԭ����ȫ�ֱ�����ΪһЩ�����ں����䴫�ݵĲ������������ݰ�ȫ*/
void Clean_Array_str_Time_Dis(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*���Node��Time��Distance��Car_Route����ص�����*/


/*solution_write.c*/
void Find_Putout_file_name(char * file_name, struct Data_str* Data_struct);
void solution_write(struct Data_str* Data_struct);


/*read_node.c*/
void Find_Node_file_name(char * file_name, struct Data_str* Data_struct);/*���ݸ��������ݵĽṹ�ҵ����Ӧ��	Node�ļ�*/
void fpRead(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*����ÿ�������Ϣ*/
void cheak_read_node(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*����Ķ����Ƿ���ȷ*/
void Clean_Node_str(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*�ͷŴ洢�����Ϣ���ڴ�ռ�*/

/*read_dis_tm.c*/
int minute(int hour, int minute);/*��������00:00��ʽ��ʱ��ת��Ϊ���Ӹ�ʽ*/
void Find_TimDis_file_name(char * file_name, struct Data_str* Data_struct);/*���ݸ��������ݵĽṹ�ҵ����Ӧ��Time_Dis�ļ�*/
void fpRead_Dis_tm(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*���������ʱ��;���*/
//int Distance(int from_node, int to_node);/*�õ�����֮��ľ��룬Ŀǰ�ǻ��ڵ��ú�������ģ�δ���ĳɶ�ά����*/
//int Time(int from_node, int to_node);/*ͬ��*/
void cheak_read_distm(struct Array_str* Array_Struct);/*�����ʱ�䡢����Ķ����Ƿ���ȷ*/


 /*find.c*/
double Print_DATA(void);/*���ȫ�ֱ�������ֵ*/
int Balence_Time(struct Car_route* Car, int Node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*����ȴ�ʱ��ĺ���*/
void Clean_Global_Storage(void);/*���ȫ�ֱ�����ALL-XXXϵ��*/
void Clean_MapRoute(int All_car_num, struct Array_str* Array_Struct);/*�ͷŴ洢����·������Ϣ���ڴ�ռ�*/
int Clean_Route_List(struct Data_str* Data_struct, struct Array_str* Array_Str);/*��ս���������ȫ�ֱ���->���ȫ�ֱ���Ҫ�иı������main�����е��û��ߵ���дһ��������*/
int Find_MapRoute(struct Data_str* Data_struct, struct Array_str* Array_Str);/*�ҵ����г��ĸ���·�����������еĵ�*/
int Find_Car_route(struct Car_route * Car, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*�ҵ���̨����·��*/
int Find_nearest_charger(int now_node, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*�ҵ�����ĳ��վ*/
void Find_nearest_receiver(struct Car_route* Car, int *Near_Receiver_array, int Passed_Receiver_num, struct Data_str* Data_struct, struct Array_str* Array_Struct);
 /*(up)�ҵ�����Ľ����ߣ�δ����Ҫ���������ջ�֮��ѡ��*/

int Find_nextnode(struct Car_route *  Car, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*�ҵ�ȥ����һ����*/
int Tell_GOorNOT(struct Car_route *  Car, int Next_node_ID, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*�ж���һ����ȥ��ȥ*/
void Arrive_Node(struct Car_route *  Car, struct Time_Node * T_Node, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*����ָ���㣬�������ز������޸�*/
struct Car_route* Add_New_Car(int car_ID, int car_type, int car_time, struct Array_str* Array_Str);/*���һ̨�³��������³����ֲ�����ʼ��*/
int choose_car_type(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*ѡ����*/
int Tell_ID_type(int node_ID, struct Data_str* Data_struct);/*�ж�ĳһ��������࣬�����δ����һ���Ƚ�ֵ��ע������飬��Ϊ�����ڱ�����Ӧ��ע�����ѡ���жϷ�ΧҲ�ڱ�*/
struct Time_Node* NtoTN_Transform(int Node_ID, struct Array_str* Array_Struct);/*��������Ϊ��Ҫ���������Ľṹ��*/
struct Car_route* Solve_Cant_Go_Place(int Cant_Go_ID, int Car_ID);/*����������ڽ���޷�����Ĳ��ֵ㣻
																   δ���޷�����ĵ�Ӧ���ٴ�check����������ָ����ʼ��ѡ��*/

/*Choose_Car_Type��new��*/
void Clean_Global_Storage_CC (void);
void Free_Test_Route (struct Car_route * Car);
void Arrive_Node_CC(struct Car_route *  Car, struct Time_Node * T_Node, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*ѡ�������е�ģ�⵽��*/
int Find_Car_route_CC(struct Car_route * Car, struct Data_str* Data_struct, struct Array_str* Array_Struct);/*ѡ�������е�ģ��Ѱ��*/

/*check_count.c*/
void Create_Check_Area(struct Data_str* Data_struct);
void Clean_Check_Area(void);
void final_check(struct Data_str* Data_struct, struct Array_str* Array_Struct);/*���·���Ƿ���Ϲ涨�ĺ���*/


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
