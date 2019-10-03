/*以下为 有关点的属性、车辆初始状态 的外部参数  以及 限制条件*/ 

#define WAREHOUSE 0 
#define WAREHOUSE_NUM 1
#define First_Receiver 1
#define MAX_CAN_USED_CAR_NUM 500 /*要求单次使用车辆不超过500台*/
#define MAX_RECEIVER_ARRAY 4

/**************//*车型 及其初始状态*/ 
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
/**************/ 

/**************//*点的类型*/ 
//#define MAX_CAR_NUM 1000 
#define CHARGER_TYPE 4
#define SENDER_TYPE 3
#define RECEIVER_TYPE 2
#define WAREHOUSE_TYPE 1 
/**************/

/**************//*优化用到 的外部参数*/
#define MAX_RECOMBINANT_NODE_NUM 30 /*混合车辆数上限值*/ 
#define MAX_GROUP_NUM 2
/**************/
