#include "main.h"
#include "PID_Realize.h"
//                  ----ID---/----制冷P-/----I----/---D----/---------T---------/---制热P--/----I---/-----D----/---------T---------
uint8_t data[22] = {0x00, 0x01, 0x0D, 0xAC, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0xB0, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0X24, 0X42}; //
realizePID rPID;																																		 // pid参数
realizeError rError;																																	 // 误差参数
float P, I, D;

realizePID rPID2;	  // pid参数
realizeError rError2; // 误差参数
float P2, I2, D2;

uint8_t M_slave = 0X01;
float targetTemp;		// 目标温度值
float targetTemp2;		// 目标温度值
float curTmp = 0;		// 当前温度值
float curTmp2 = 0;		// 当前温度值
int TMP_PWM = 9999;		// 控制温度的PWM输出
int TMP_PWM2 = 0;		// 控制温度的PWM输出
int MAX_TMP_PWM = 9999; // 控制温度的PWM输出

u8 flag_rev = 0;	 // 串口中断接收标志位
u8 flag_rev_num = 0; // 串口数据索引
extern float temp_date[8];
int error;
void oledInit(void);
void keyInit(void);
void PidAndErrorInit(void);
void extiSet(void);
void extiAdd(void);
void extiSub(void);
void timer2(void);
uint8_t buf[EE_SIZE]; /* 数据测试用缓冲区 16KB */
extern uint8_t buf1[10];
/**
 * @brief  主函数
 * @param  无
 * @retval 无
 */
void idle(void)
{
	/* 独立看门狗喂狗时间到 */
	if (iwdg_feed_flg == 1)
	{
		iwdg_feed_flg = 0;
		iwdg_feed();
	}
	mbslave_poll(&mb_usart3_t);
}

float Hex_To_Decimal(unsigned char *Byte, int num)
{
#if 1
	char cByte[4]; // 方法一
	for (int i = 0; i < num; i++)
	{
		cByte[i] = Byte[i];
	}

	float pfValue = *(float *)&cByte;
	return pfValue;
#else
	return *((float *)Byte); // 方法二
#endif
}

void FloatToByte(float floatNum, unsigned char *byteArry) // 浮点数到十六进制转换2
{
	char *pchar = (char *)&floatNum;
	int i = 0;
	for (i = 0; i < sizeof(float); i++)
	{
		*byteArry = *pchar;
		pchar++;
		byteArry++;
	}
}

void EEpromdata_init(void)
{

	//  ee_WriteBytes((uint8_t *)data,DD_EEPROM_PID , 22);			//初始化一次即可，下次使用时注释掉，与下面一句不能同时用

	if (ee_ReadBytes((uint8_t *)buf, DD_EEPROM_PID, 22) == 1)
	{
		M_slave = (buf[0] << 8) + buf[1];
		rPID.P = (buf[2] << 8) + buf[3];
		rPID.I = (buf[4] << 8) + buf[5];
		rPID.D = (buf[6] << 8) + buf[7];

		targetTemp = Hex_To_Decimal(&buf[8], 4);

		rPID2.P = (buf[12] << 8) + buf[13];
		rPID2.I = (buf[14] << 8) + buf[15];
		rPID2.D = (buf[16] << 8) + buf[17];

		targetTemp2 = Hex_To_Decimal(&buf[18], 4);
	}
}

int main(void)
{
	int ret = 0;
	uint8_t __chan_en;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	delay_init();
	tim7_init();											// TIM7定时器初始化
	TIM1_GPIO_Config();										// 定时器通道1输出引脚模式
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP); // 初始化与LED连接的硬件接口
	bsp_InitI2C();											/* 配置I2C总线EEPROM初始化 */
	EEpromdata_init();										// 必须在PID参数初始化之前
	PidAndErrorInit();										// PID与Error参数初始化
	oledInit();												// oled屏幕初始化
	gpio_i2c_init();										// I2C端口初始化
	keyInit();												// 按键初始化
	// 3路帕尔贴制冷
	TIM_PWM_Init(TIM3, PWM_CH1, 1000, ENABLE); // PWM初始化
	TIM_PWM_Init(TIM3, PWM_CH2, 1000, ENABLE); // PWM初始化
	TIM_PWM_Init(TIM3, PWM_CH3, 1000, ENABLE); // PWM初始化
	TIM_SetCompare(TIM3, PWM_CH1, 9999);	   // 设置PWM占空比
	TIM_SetCompare(TIM3, PWM_CH2, 9999);	   // 设置PWM占空比
	TIM_SetCompare(TIM3, PWM_CH3, 9999);	   // 设置PWM占空比
	// 2路加热带制热
	TIM_PWM_Init(TIM3, PWM_CH4, 1000, ENABLE); // PWM初始化
	TIM_SetCompare(TIM3, PWM_CH4, 1000);	   // 设置PWM占空比
	TIM1_Configuration(1000, 1000);
	//  TIM1_Configuration(999,0);//999对应1k
	//	TIM_SetCompare(TIM3, PWM_CH4, 0);           //设置PWM占空比

	TIM_ms_Init(TIM2, 1000, TIM_IT_Update, 2, ENABLE); // 定时器初始化
	TIMER_CallbackInstall(HW_TIMER2, timer2);		   // 定时器中断回调函数
	/*初始化485使用的串口，使用中断模式接收*/
	mbslave_init(&mb_usart3_t, M_slave, MB_BAUD);

	iwdg_init(IWDG_Prescaler_128, 3125); /* 超时时间3125*3.2ms=10s */

	__chan_en = ZAM6228_CHAN1_EN | ZAM6228_CHAN2_EN | ZAM6228_CHAN3_EN | ZAM6228_CHAN4_EN | ZAM6228_CHAN5_EN | ZAM6228_CHAN6_EN | ZAM6228_CHAN7_EN | ZAM6228_CHAN8_EN;

	ret = zam6228_set_chan_en((void *)0x48, ZAM6228_CHAN_EN_ADDR, __chan_en);
	if (ret < 0)
	{
		return -1;
	}
	//   usart3_mode(TX);
	//  printf("\r\n 国研PID温控版测试\r\n");
	//	usart3_mode(RX);

	while (1)
	{
		idle();
		delay_ms(1000);
	}
}

// 定时器处理函数
void timer2()
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{ // 检查TIM3更新中断发生与否

		if (0 == zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN1, &temp_date[0]))
		{
			curTmp = temp_date[0]; // 求得当前温度
		}
		if (0 == zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN2, &temp_date[1]))
		{
			curTmp2 = temp_date[1]; // 求得当前温度
		}
		rError.Current_Error = curTmp - targetTemp; // 求得当前误差
		TMP_PWM = (int)PID_Realize(&rError, &rPID); // 根据PID参数求得输出的PWM

		if (TMP_PWM > 9999)
		{
			TMP_PWM = 9999;
		}
		TIM_SetCompare(TIM3, PWM_CH1, TMP_PWM); // 设置PWM值
		TIM_SetCompare(TIM3, PWM_CH2, TMP_PWM); // 设置PWM值
		TIM_SetCompare(TIM3, PWM_CH3, TMP_PWM); // 设置PWM值

		rError2.Current_Error = targetTemp2 - curTmp2; // 求得当前误差
		TMP_PWM2 = (int)PID_Realize(&rError2, &rPID2); // 根据PID参数求得输出的PWM

		if (TMP_PWM2 < 0)
		{
			TMP_PWM2 = 0;
		}
		TIM_SetCompare(TIM3, PWM_CH4, TMP_PWM2); // 设置PWM值
		TIM1_Configuration(1000, TMP_PWM2);

		OLED_P8x16Str(0, 0, "T1:");
		OLED_P8x16Num(24, 0, curTmp, 2);
		OLED_P8x16Num(84, 0, targetTemp, 1);
		OLED_P8x16Str(0, 2, "PWM:");
		OLED_P8x16Num(48, 2, TMP_PWM, 0);

		OLED_P8x16Str(0, 4, "T2:");
		OLED_P8x16Num(24, 4, curTmp2, 2);
		OLED_P8x16Num(84, 4, targetTemp2, 1);
		OLED_P8x16Str(0, 6, "PWM:");
		OLED_P8x16Num(48, 6, TMP_PWM2, 0);

		//			OLED_P8x16Num(30, 0, curTmp, 2);
		//			OLED_P8x16Num(90, 2, targetTemp, 1);
		//			OLED_P8x16Str(80,4,"PWM");
		//			OLED_P8x16Str(70,6,"     "); OLED_P8x16Num(70, 6, TMP_PWM, 0);

		//			OLED_P8x16Num(30, 0, curTmp2, 2);
		//			OLED_P8x16Num(90, 2, targetTemp2, 1);
		//			OLED_P8x16Str(80,4,"PWM");
		//			OLED_P8x16Str(70,6,"     "); OLED_P8x16Num(70, 6, TMP_PWM2, 0);

		PAout(3) = !PAout(3);

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除TIMx更新中断标志
	}
}

// oled屏幕初始化函数
void oledInit()
{
	IIC_Init();	 // IIC协议初始化
	OLED_Init(); // oled屏幕初始化
	for (int i = 1; i <= 4; i++)
	{
		OLED_P16x16Ch(i * 16 + 16, 2, i);
	} // 开机显示沁和智能字样
	OLED_P8x16Str(40, 4, "GY200A");
	delay_ms(1800);
	OLED_CLS(); // 清屏
				//	OLED_P8x16Str(0,2," P:"); OLED_P8x16Num(32, 2, rPID.P, 1);  //显示P值
				//	OLED_P8x16Str(0, 0, "CT:");
				//	OLED_P8x16Str(0,4," I:"); OLED_P8x16Num(32, 4, rPID.I, 1);  //显示I值
				////		OLED_P8x16Str(80, 2, "S:");
				//	OLED_P8x16Str(0,6," D:"); OLED_P8x16Num(32, 6, rPID.D, 1);  //显示D值
				//	delay_ms(10000);
				//	OLED_CLS();   //清屏
				//	OLED_P8x16Str(0,2," P2:"); OLED_P8x16Num(32, 2, rPID2.P, 1);  //显示P值
				//	OLED_P8x16Str(0, 0, "CT:");
				//	OLED_P8x16Str(0,4," I2:"); OLED_P8x16Num(32, 4, rPID2.I, 1);  //显示I值
				////		OLED_P8x16Str(80, 2, "S:");
				//	OLED_P8x16Str(0,6," D2:"); OLED_P8x16Num(32, 6, rPID2.D, 1);  //显示D值
}
// 按键初始化函数
void keyInit(void)
{
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_5, 3, 3); // 设置按键初始化
	EXTI_CallbackInstall(EXTI_Pin_5, extiSet);
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_6, 3, 3); // 加按键初始化
	EXTI_CallbackInstall(EXTI_Pin_6, extiAdd);
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_7, 3, 3); // 减按键初始化
	EXTI_CallbackInstall(EXTI_Pin_7, extiSub);
}

// PID与Error参数初始化函数
void PidAndErrorInit()
{
	rPID.PWMLIMITE = MAX_TMP_PWM;
	P = rPID.P;
	I = rPID.I;
	D = rPID.D;
	rPID.I_limit = 300;
	rError.Current_Error = temp_date[0] - targetTemp;
	rError.Last_Error = rError.Current_Error;

	rPID2.PWMLIMITE = MAX_TMP_PWM;
	P2 = rPID2.P;
	I2 = rPID2.I;
	D2 = rPID2.D;
	rPID2.I_limit = 300;
	rError2.Current_Error = targetTemp2 - temp_date[1];
	rError2.Last_Error = rError2.Current_Error;
}

int flag_set = 0;
// 设置按键处理函数
void extiSet(void)
{
	delay_ms(30);
	if (PAin(5) == 1)
	{
		flag_set++;
		if (flag_set > 3)
			flag_set = 0;
		switch (flag_set)
		{
		case 0:
			OLED_P8x16Str(0, 2, " P:");
			OLED_P8x16Str(0, 4, " I:");
			OLED_P8x16Str(0, 6, " D:");
			break;
		case 1:
			OLED_P8x16Str(0, 2, "*P:");
			OLED_P8x16Str(0, 4, " I:");
			OLED_P8x16Str(0, 6, " D:");
			break;
		case 2:
			OLED_P8x16Str(0, 2, " P:");
			OLED_P8x16Str(0, 4, "*I:");
			OLED_P8x16Str(0, 6, " D:");
			break;
		case 3:
			OLED_P8x16Str(0, 2, " P:");
			OLED_P8x16Str(0, 4, " I:");
			OLED_P8x16Str(0, 6, "*D:");
			break;
		}
	}
}

// 加按键处理函数
void extiAdd(void)
{
	delay_ms(30);
	if (PAin(6) == 1)
	{
		switch (flag_set)
		{
		case 1:
			OLED_P8x16Str(0, 2, "*P:");
			OLED_P8x16Str(32, 2, "      ");
			OLED_P8x16Num(32, 2, ++rPID.P, 1);
			break;
		case 2:
			OLED_P8x16Str(0, 4, "*I:");
			OLED_P8x16Str(32, 4, "      ");
			OLED_P8x16Num(32, 4, ++rPID.I, 1);
			break;
		case 3:
			OLED_P8x16Str(0, 6, "*D:");
			OLED_P8x16Str(32, 6, "      ");
			OLED_P8x16Num(32, 6, ++rPID.D, 1);
			break;
		}
	}
}

// 减按键处理函数
void extiSub(void)
{
	delay_ms(30);
	if (PAin(7) == 1)
	{
		switch (flag_set)
		{
		case 1:
			OLED_P8x16Str(0, 2, "*P:");
			OLED_P8x16Str(32, 2, "      ");
			OLED_P8x16Num(32, 2, --rPID.P, 1);
			break;
		case 2:
			OLED_P8x16Str(0, 4, "*I:");
			OLED_P8x16Str(32, 4, "      ");
			OLED_P8x16Num(32, 4, --rPID.I, 1);
			break;
		case 3:
			OLED_P8x16Str(0, 6, "*D:");
			OLED_P8x16Str(32, 6, "      ");
			OLED_P8x16Num(32, 6, --rPID.D, 1);
			break;
		}
	}
}

/*********************************************END OF FILE**********************/
