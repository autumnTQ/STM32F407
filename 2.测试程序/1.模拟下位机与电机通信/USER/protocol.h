#ifndef __PROTOCOL_H
#define __PROTOCOL_H	 


//响应PC的协议————系统指令
unsigned char PC_Set_DeviceInformation[]       ={ 0xA1,0x00,0x39,0x00,0x01,0x61,0x00,0x05 };             //响应 读写设备信息
unsigned char PC_Set_SensorInformation[]       ={ 0xA1,0x00,0x39,0x00,0x01,0x61,0x00,0x06 };             //响应 防拆状态信息
unsigned char PC_Set_SoftReset[]               ={ 0xA1,0x00,0x07,0x00,0x01,0x61,0x00,0x08 };             //响应 CPU复位

//响应PC的协议————控制核心板端口
unsigned char PC_Set_GPIOState[]		       = { 0xA1,0x00,0x0A,0x01,0x00,0x61,0x03,0x01 };			//响应 设置GPIO输出状态

//响应PC的协议————电机
unsigned char PC_Set_MotorOK[]					 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x00 };			//响应电机运行完成
unsigned char PC_Set_MotorStop[]				 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x00 };			//关闭电机
unsigned char PC_Set_TargetPosition[]		     = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x01 };			//响应 设置运行目标位置
unsigned char PC_Set_MotorSpeed[]				 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x02 };			//响应 设置运行速度
unsigned char PC_Set_MotorAcc[]					 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x03 };			//响应 设置加速度
unsigned char PC_Set_MotorDec[]					 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x04 };			//响应 设置减速度
unsigned char PC_Set_MotorCra[]					 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x05 };			//响应 设置加速电流
unsigned char PC_Set_MotorCrn[]					 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x06 };			//响应 设置工作电流
unsigned char PC_Set_MotorCrh[]					 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x07 };			//响应 设置保持电流
unsigned char PC_QueryMotorState[]			     = { 0xA1,0x00,0x08,0x01,0x00,0x60,0x01,0x08 };			//响应 查询电机状态
unsigned char PC_SetMotorZero[]					 = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x09 };			//响应 设置电机回零点
//unsigned char PC_QuerySensorState[]			     = { 0xA1,0x00,0x08,0x01,0x00,0x60,0x01,0x0A };			//响应 电机指定传感器的状态
unsigned char PC_Set_MotorRunMode[]		         = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x0B };			//响应 设置电机运行模式
//unsigned char PC_Set_LevelDet[]		             = { 0xA1,0x00,0x08,0x01,0x00,0x61,0x01,0x0C };			//响应 设置液面探测运行
//响应PC的协议————夹爪   
unsigned char PC_Set_GripInit[]                  = { 0xA1,0x00,0x08,0x00,0x01,0x61,0x02,0x01,0x30 };         //响应 设置夹爪上电后初始化
unsigned char PC_Set_GripOpen[]                  = { 0xA1,0x00,0x08,0x00,0x01,0x61,0x02,0x02,0x30 };         //响应 设置夹爪张开
unsigned char PC_Set_GripClose[]                 = { 0xA1,0x00,0x08,0x00,0x01,0x61,0x02,0x03,0x30 };         //响应 设置夹爪闭合
unsigned char PC_Set_QueryGripPosition[]         = { 0xA1,0x00,0x08,0x00,0x01,0x61,0x02,0x04,0x30 };         //响应 查询夹爪当前位置
//响应PC的协议————推杆   
unsigned char PC_Set_PushTargetPosition[]        = { 0xA1,0x00,0x0A,0x00,0x01,0x61,0x04,0x01,0x01 };         //响应 设置推杆运行目标位置
unsigned char PC_Set_QueryPushPosition[]         = { 0xA1,0x00,0x08,0x00,0x01,0x60,0x04,0x02,0x01 };         //响应 查询推杆当前位置
//响应PC的协议————温度控制板
unsigned char PC_Set_TempValue[]                 = { 0xA1,0x00,0x0D,0x00,0x01,0x61,0x05,0x01 };              //响应 设置温度控制板的温度


//发送给电机的协议
unsigned char QueryMotorState[]					= { 0x01,0x04,0x00,0x00,0x00,0x06 };		            //查询电机运行状态
unsigned char QueryMotorConfigure[]			    = { 0x01,0x03,0x00,0x00,0x00,0x09 };		            //查询电机配置参数
// unsigned char Set_TargetPosition[]			    = { 0x01,0x10,0x00,0x01,0x00,0x02,0x04 };			    //设置目标位置
//unsigned char Set_MotorSpeed[]					= { 0x01,0x10,0x00,0x03,0x00,0x02,0x04 };			    //设置运行速度
unsigned char Set_MotorAcc[]					= { 0x01,0x10,0x00,0x05,0x00,0x02,0x04 };			    //设置加速度
unsigned char Set_MotorDec[]					= { 0x01,0x10,0x00,0x07,0x00,0x02,0x04 };			    //设置减速度
unsigned char Set_MotorCra[]					= { 0x01,0x10,0x00,0x09,0x00,0x02,0x04 };			    //设置加速电流
unsigned char Set_MotorCrn[]					= { 0x01,0x10,0x00,0x0B,0x00,0x02,0x04 };			    //设置工作电流
unsigned char Set_MotorCrh[]					= { 0x01,0x10,0x00,0x0D,0x00,0x02,0x04 };			    //设置保持电流
unsigned char Set_MotorStop[]					= { 0x01,0x06,0x00,0x00,0x04,0x01 };			        //立刻停止运行
//unsigned char Set_MotorZero[]					= { 0x01,0x06,0x00,0x00,0x06,0x01 };			        //设置电机回零点 启动归零动作


//电机归零后查询的数据   01 04 0C 00 00 00 00 9B 37 FF FF 21 07 00 01 E1 8D
//设置电机速度反馈的数据  01 03 12 02 00 03 E8 00 00 50 00 47 C3 50 00 48 43 50 00 48 43 E5 34

unsigned char Set_MotorZero[]					= { 0x01, 0x04, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x07, 0x00, 0x01 };			        //设置电机回零点 启动归零动作
unsigned char Set_TargetPosition[]				= { 0x01, 0x04, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x07, 0x00, 0x01 };	

//液面探测指令
unsigned char PC_Set_LevelDet[]				    = { 0x01, 0x04, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x07, 0x00, 0xFF };	
//设置电机回零点 启动归零动作
unsigned char PC_QuerySensorState[]				= { 0x01, 0x04, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };			        //设置电机回零点 启动归零动作
unsigned char Set_MotorSpeed[]					= { 0x01, 0x03, 0x12, 0x02, 0x00, 0x03, 0xE8, 0x00, 0x00, 0x50, 0x00, 0x47, 0xC3, 0x50, 0x00, 0x48, 0x43, 0x50, 0x00, 0x48, 0x43 };			    //设置运行速度

    
    
    
    
    //01 04 02 03 E8 B9 8E 
unsigned char Set_PushTargetPosition[]              = { 0x01,0x04,0x02,0x03,0xE8 };                   //设置推杆运行目标位置


unsigned char Set_MotorRunMode[]			    = { 0x01,0x06,0x00,0x00,0x03,0x00 };			        //设置电机运行模式
unsigned char Set_LevelDet[]			        = { 0x01,0x10,0x00,0x01,0x00,0x02,0x04 };			    //设置液面探测运行

//发送给夹爪的协议—————夹爪地址以0x30为例
unsigned char Set_GripInit[]                        = { 0xEB,0x91,0x30,0x08,0x00,0x00,0x00,0x00,0x38 };         //设置夹爪上电后初始化动作
unsigned char Set_GripOpen[]                        = { 0xEB,0x91,0x30,0x06,0x00,0x00,0x00,0x00,0x36 };         //设置夹爪张开
unsigned char Set_GripClose[]                       = { 0xEB,0x91,0x30,0x06,0x01,0x00,0x00,0x00,0x37 };         //设置夹爪闭合
unsigned char QueryGripPosition[]                   = { 0xEB,0x91,0x30,0x05,0x00,0x00,0x00,0x00,0x35 };         //查询夹爪当前位置

//发送给推杆的协议—————推杆地址以0x10为例
//unsigned char Set_PushTargetPosition[]              = { 0x01,0x06,0x00,0x02 };                   //设置推杆运行目标位置
unsigned char QueryPushPosition[]                   = { 0x01,0x04,0x00,0x02,0x00,0x01 };         //查询推杆当前位置

// 发送给温度控制板的协议
unsigned char Set_TempValue[]                   = { 0x01,0x10,0x05,0x01 };         //设置温度控制板的控制温度
#endif
