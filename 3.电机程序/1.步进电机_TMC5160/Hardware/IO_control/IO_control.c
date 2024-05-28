#include "IO_control.h"


/*******************************************************************************
 * @函数名称
 * @函数说明   主函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void GPIO_cfg(void)
{
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC ,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;  // 端口类型变量声明
    GPIO_StructInit(&GPIO_InitStructure); // 把 GPIO_InitStruct中的每个参数按缺省值填入
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//改变指定管脚的映射JTAG禁用 SWD 启用
    ///////////端口  A  ///////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // 上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // 上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;         // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //漏输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //漏输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //开漏输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;        // 端口号	TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            // 端口号  RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器
                                                     ////////////端口  B   ///////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;  //下拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //开漏输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //开漏输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //开漏输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;         // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
                                                  // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;  //浮空输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;  //开漏输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
                                                  // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);        // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // 上拉输入
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器

    
    ///////////////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器
                                                     ///////////////////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出
                                                     // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);           // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器
                                                     ///////////////////////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;       // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;            // 上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);                   // 根据 GPIO_InitStruct 指定的参数初始化GPIOx 寄存器
                                                             ////////////////////////////////
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //?JTDO，JTDI，JTCK 作为普通IO使用
                                                             //-----定义外部中断---------------------------
                                                             // GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);//CW
                                                             // GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);//STP
                                                             //-----------------

}
