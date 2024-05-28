#include "Internal_FLASH.h"

//�ڲ�FLASHд�����ݺ���
//data��д������ݡ�
//dataLength��д�����ݵĳ���
//����д��ĵ�ַ�� FLASH_Place = 0x080E0000!   ����11
FLASH_Status Internal_FLASH_Write(uint32_t FLASH_Sector, uint32_t FLASH_Place, char * data, int dataLength)
{
    //�������������ķ���ֵ����
    FLASH_Status Write_Status;
    FLASH_Status status;
    //����
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGPERR| FLASH_FLAG_PGSERR);
    //������11������ 
    status = FLASH_EraseSector(FLASH_Sector, VoltageRange_3);
    
    if(status == FLASH_COMPLETE)   //�жϲ�����״̬  �����������
    {
        for(uint32_t i=0; i<dataLength; i++)
        {
            //д������  д��һ���ֽ�
            status = FLASH_ProgramByte(FLASH_Place++, data[i]);
                    
            if(status == FLASH_COMPLETE)   //�ж�д���״̬ д���������
            {
                Write_Status = FLASH_COMPLETE;
            }
            else
            {
                Write_Status = FLASH_ERROR_RD;
                break;
            }
        }
        //����
        FLASH_Lock();

        return Write_Status;   
    }
    else
    {
        return FLASH_ERROR_RD;
    }
}

//�ڲ�FLASH��ȡ���ݺ���
//dataLength����ȡ���ݵĳ���
//�����ȡ�ĵ�ַ�� FLASH_Place = 0x080E0000!   ����11
void Internal_FLASH_Read(uint32_t FLASH_Place, char* Read_Data, int dataLength)
{
    char *p = (char*) FLASH_Place; 

    for(int i=0; i<dataLength; i++)
    {
        Read_Data[i] = *p++;
//        Read_Data[dataLength] = '\0';
    }
}

