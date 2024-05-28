#include "Internal_FLASH.h"

//内部FLASH写入数据函数
//data：写入的数据。
//dataLength：写入数据的长度
//这里写入的地址是 FLASH_Place = 0x080E0000!   扇区11
FLASH_Status Internal_FLASH_Write(uint32_t FLASH_Sector, uint32_t FLASH_Place, char * data, int dataLength)
{
    //扇区擦除函数的返回值类型
    FLASH_Status Write_Status;
    FLASH_Status status;
    //解锁
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGPERR| FLASH_FLAG_PGSERR);
    //擦除第11个扇区 
    status = FLASH_EraseSector(FLASH_Sector, VoltageRange_3);
    
    if(status == FLASH_COMPLETE)   //判断擦除的状态  扇区擦除完成
    {
        for(uint32_t i=0; i<dataLength; i++)
        {
            //写入内容  写入一个字节
            status = FLASH_ProgramByte(FLASH_Place++, data[i]);
                    
            if(status == FLASH_COMPLETE)   //判断写入的状态 写入数据完成
            {
                Write_Status = FLASH_COMPLETE;
            }
            else
            {
                Write_Status = FLASH_ERROR_RD;
                break;
            }
        }
        //上锁
        FLASH_Lock();

        return Write_Status;   
    }
    else
    {
        return FLASH_ERROR_RD;
    }
}

//内部FLASH读取数据函数
//dataLength：读取数据的长度
//这里读取的地址是 FLASH_Place = 0x080E0000!   扇区11
void Internal_FLASH_Read(uint32_t FLASH_Place, char* Read_Data, int dataLength)
{
    char *p = (char*) FLASH_Place; 

    for(int i=0; i<dataLength; i++)
    {
        Read_Data[i] = *p++;
//        Read_Data[dataLength] = '\0';
    }
}

