/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#define FILLBYTE 0xA3u
#define CMDBUFFSIZE 3


#include "project.h"
uint8 cmdBuff[CMDBUFFSIZE];
uint8 iCmdBuff = CMDBUFFSIZE - 1;

CY_ISR(SPINext)
{
    if (iCmdBuff < CMDBUFFSIZE - 1)
    {
        SPIM_BP_WriteTxData(cmdBuff[iCmdBuff++]);
    }
    else if (!Pin_nDrdy_Read())
    {
        iCmdBuff = CMDBUFFSIZE - 1;
        SPIM_BP_WriteTxData(cmdBuff[iCmdBuff]);
    }
    
}
int main(void)
{
//    uint8 status;
//    uint8 fillByte = 0xA3u;
    cmdBuff[CMDBUFFSIZE - 1] = FILLBYTE;
    
    
    
    
    SPIM_BP_Start();
    SPIM_BP_ClearFIFO();
    
    Timer_Tsync_Start();
    Control_Reg_R_Write(0x00u);
    Control_Reg_SS_Write(0x0Fu);
    isr_W_StartEx(SPINext);
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    cmdBuff[0] = 0x0Fu;
    cmdBuff[1] = 0xF0u;
    SPIM_BP_WriteTxData(cmdBuff[0]);
    iCmdBuff = 1;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    for(;;)
    {
        
        /* Place your application code here. */
        //if (SPIM_BP_GetRxBufferSize > 0)
        //{
//            SPIM_BP_ReadRxData();
            

        //while(Pin_Sel_Read());
        
            
//            do{
//                status = SPIM_BP_ReadTxStatus();
//            }while (!(status & ( SPIM_BP_STS_SPI_IDLE)));
            
//            while((Status_Reg_nSS_Read()));
//            SPIM_BP_ClearTxBuffer();
//
//            SPIM_BP_WriteTxData(fillByte);
        //}
    }
}

/* [] END OF FILE */
