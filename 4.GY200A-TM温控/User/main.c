#include "main.h"
#include "PID_Realize.h"
//                  ----ID---/----����P-/----I----/---D----/---------T---------/---����P--/----I---/-----D----/---------T---------
uint8_t data[22] = {0x00, 0x01, 0x0D, 0xAC, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0xB0, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0X24, 0X42}; //
realizePID rPID;																																		 // pid����
realizeError rError;																																	 // ������
float P, I, D;

realizePID rPID2;	  // pid����
realizeError rError2; // ������
float P2, I2, D2;

uint8_t M_slave = 0X01;
float targetTemp;		// Ŀ���¶�ֵ
float targetTemp2;		// Ŀ���¶�ֵ
float curTmp = 0;		// ��ǰ�¶�ֵ
float curTmp2 = 0;		// ��ǰ�¶�ֵ
int TMP_PWM = 9999;		// �����¶ȵ�PWM���
int TMP_PWM2 = 0;		// �����¶ȵ�PWM���
int MAX_TMP_PWM = 9999; // �����¶ȵ�PWM���

u8 flag_rev = 0;	 // �����жϽ��ձ�־λ
u8 flag_rev_num = 0; // ������������
extern float temp_date[8];
int error;
void oledInit(void);
void keyInit(void);
void PidAndErrorInit(void);
void extiSet(void);
void extiAdd(void);
void extiSub(void);
void timer2(void);
uint8_t buf[EE_SIZE]; /* ���ݲ����û����� 16KB */
extern uint8_t buf1[10];
/**
 * @brief  ������
 * @param  ��
 * @retval ��
 */
void idle(void)
{
	/* �������Ź�ι��ʱ�䵽 */
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
	char cByte[4]; // ����һ
	for (int i = 0; i < num; i++)
	{
		cByte[i] = Byte[i];
	}

	float pfValue = *(float *)&cByte;
	return pfValue;
#else
	return *((float *)Byte); // ������
#endif
}

void FloatToByte(float floatNum, unsigned char *byteArry) // ��������ʮ������ת��2
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

	//  ee_WriteBytes((uint8_t *)data,DD_EEPROM_PID , 22);			//��ʼ��һ�μ��ɣ��´�ʹ��ʱע�͵���������һ�䲻��ͬʱ��

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
	tim7_init();											// TIM7��ʱ����ʼ��
	TIM1_GPIO_Config();										// ��ʱ��ͨ��1�������ģʽ
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP); // ��ʼ����LED���ӵ�Ӳ���ӿ�
	bsp_InitI2C();											/* ����I2C����EEPROM��ʼ�� */
	EEpromdata_init();										// ������PID������ʼ��֮ǰ
	PidAndErrorInit();										// PID��Error������ʼ��
	oledInit();												// oled��Ļ��ʼ��
	gpio_i2c_init();										// I2C�˿ڳ�ʼ��
	keyInit();												// ������ʼ��
	// 3·����������
	TIM_PWM_Init(TIM3, PWM_CH1, 1000, ENABLE); // PWM��ʼ��
	TIM_PWM_Init(TIM3, PWM_CH2, 1000, ENABLE); // PWM��ʼ��
	TIM_PWM_Init(TIM3, PWM_CH3, 1000, ENABLE); // PWM��ʼ��
	TIM_SetCompare(TIM3, PWM_CH1, 9999);	   // ����PWMռ�ձ�
	TIM_SetCompare(TIM3, PWM_CH2, 9999);	   // ����PWMռ�ձ�
	TIM_SetCompare(TIM3, PWM_CH3, 9999);	   // ����PWMռ�ձ�
	// 2·���ȴ�����
	TIM_PWM_Init(TIM3, PWM_CH4, 1000, ENABLE); // PWM��ʼ��
	TIM_SetCompare(TIM3, PWM_CH4, 1000);	   // ����PWMռ�ձ�
	TIM1_Configuration(1000, 1000);
	//  TIM1_Configuration(999,0);//999��Ӧ1k
	//	TIM_SetCompare(TIM3, PWM_CH4, 0);           //����PWMռ�ձ�

	TIM_ms_Init(TIM2, 1000, TIM_IT_Update, 2, ENABLE); // ��ʱ����ʼ��
	TIMER_CallbackInstall(HW_TIMER2, timer2);		   // ��ʱ���жϻص�����
	/*��ʼ��485ʹ�õĴ��ڣ�ʹ���ж�ģʽ����*/
	mbslave_init(&mb_usart3_t, M_slave, MB_BAUD);

	iwdg_init(IWDG_Prescaler_128, 3125); /* ��ʱʱ��3125*3.2ms=10s */

	__chan_en = ZAM6228_CHAN1_EN | ZAM6228_CHAN2_EN | ZAM6228_CHAN3_EN | ZAM6228_CHAN4_EN | ZAM6228_CHAN5_EN | ZAM6228_CHAN6_EN | ZAM6228_CHAN7_EN | ZAM6228_CHAN8_EN;

	ret = zam6228_set_chan_en((void *)0x48, ZAM6228_CHAN_EN_ADDR, __chan_en);
	if (ret < 0)
	{
		return -1;
	}
	//   usart3_mode(TX);
	//  printf("\r\n ����PID�¿ذ����\r\n");
	//	usart3_mode(RX);

	while (1)
	{
		idle();
		delay_ms(1000);
	}
}

// ��ʱ��������
void timer2()
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{ // ���TIM3�����жϷ������

		if (0 == zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN1, &temp_date[0]))
		{
			curTmp = temp_date[0]; // ��õ�ǰ�¶�
		}
		if (0 == zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN2, &temp_date[1]))
		{
			curTmp2 = temp_date[1]; // ��õ�ǰ�¶�
		}
		rError.Current_Error = curTmp - targetTemp; // ��õ�ǰ���
		TMP_PWM = (int)PID_Realize(&rError, &rPID); // ����PID������������PWM

		if (TMP_PWM > 9999)
		{
			TMP_PWM = 9999;
		}
		TIM_SetCompare(TIM3, PWM_CH1, TMP_PWM); // ����PWMֵ
		TIM_SetCompare(TIM3, PWM_CH2, TMP_PWM); // ����PWMֵ
		TIM_SetCompare(TIM3, PWM_CH3, TMP_PWM); // ����PWMֵ

		rError2.Current_Error = targetTemp2 - curTmp2; // ��õ�ǰ���
		TMP_PWM2 = (int)PID_Realize(&rError2, &rPID2); // ����PID������������PWM

		if (TMP_PWM2 < 0)
		{
			TMP_PWM2 = 0;
		}
		TIM_SetCompare(TIM3, PWM_CH4, TMP_PWM2); // ����PWMֵ
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

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // ���TIMx�����жϱ�־
	}
}

// oled��Ļ��ʼ������
void oledInit()
{
	IIC_Init();	 // IICЭ���ʼ��
	OLED_Init(); // oled��Ļ��ʼ��
	for (int i = 1; i <= 4; i++)
	{
		OLED_P16x16Ch(i * 16 + 16, 2, i);
	} // ������ʾ�ߺ���������
	OLED_P8x16Str(40, 4, "GY200A");
	delay_ms(1800);
	OLED_CLS(); // ����
				//	OLED_P8x16Str(0,2," P:"); OLED_P8x16Num(32, 2, rPID.P, 1);  //��ʾPֵ
				//	OLED_P8x16Str(0, 0, "CT:");
				//	OLED_P8x16Str(0,4," I:"); OLED_P8x16Num(32, 4, rPID.I, 1);  //��ʾIֵ
				////		OLED_P8x16Str(80, 2, "S:");
				//	OLED_P8x16Str(0,6," D:"); OLED_P8x16Num(32, 6, rPID.D, 1);  //��ʾDֵ
				//	delay_ms(10000);
				//	OLED_CLS();   //����
				//	OLED_P8x16Str(0,2," P2:"); OLED_P8x16Num(32, 2, rPID2.P, 1);  //��ʾPֵ
				//	OLED_P8x16Str(0, 0, "CT:");
				//	OLED_P8x16Str(0,4," I2:"); OLED_P8x16Num(32, 4, rPID2.I, 1);  //��ʾIֵ
				////		OLED_P8x16Str(80, 2, "S:");
				//	OLED_P8x16Str(0,6," D2:"); OLED_P8x16Num(32, 6, rPID2.D, 1);  //��ʾDֵ
}
// ������ʼ������
void keyInit(void)
{
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_5, 3, 3); // ���ð�����ʼ��
	EXTI_CallbackInstall(EXTI_Pin_5, extiSet);
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_6, 3, 3); // �Ӱ�����ʼ��
	EXTI_CallbackInstall(EXTI_Pin_6, extiAdd);
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_7, 3, 3); // ��������ʼ��
	EXTI_CallbackInstall(EXTI_Pin_7, extiSub);
}

// PID��Error������ʼ������
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
// ���ð���������
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

// �Ӱ���������
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

// ������������
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
