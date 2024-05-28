#include "IO_control.h"


/*******************************************************************************
 * @��������
 * @����˵��   ������
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void GPIO_cfg(void)
{
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC ,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;  // �˿����ͱ�������
    GPIO_StructInit(&GPIO_InitStructure); // �� GPIO_InitStruct�е�ÿ��������ȱʡֵ����
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//�ı�ָ���ܽŵ�ӳ��JTAG���� SWD ����
    ///////////�˿�  A  ///////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // ��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // ��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;         // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //©���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //©���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //��©���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;        // �˿ں�	TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            // �˿ں�  RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���
                                                     ////////////�˿�  B   ///////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;  //��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //��©���
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //��©���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //��©���
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;         // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // ��©���
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // ��©���
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
                                                  // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //��©���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
                                                  // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // ��������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_Init(GPIOB, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure); // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���

    
    ///////////////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // ��©���
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���
                                                     ///////////////////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // ��©���
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);           // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���
                                                     ///////////////////////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;       // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;            // ��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);                   // ���� GPIO_InitStruct ָ���Ĳ�����ʼ��GPIOx �Ĵ���
                                                             ////////////////////////////////
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //?JTDO��JTDI��JTCK ��Ϊ��ͨIOʹ��
                                                             //-----�����ⲿ�ж�---------------------------
                                                             // GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);//CW
                                                             // GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);//STP
                                                             //-----------------

}
