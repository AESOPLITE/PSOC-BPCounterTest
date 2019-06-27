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


#include "project.h"
#include "stdio.h"
#include "string.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// From LROA103.ASM
//;The format for the serial command is:
//; S1234<sp>xyWS1234<sp>xyWS1234<sp>xyW<cr><lf>
//; where 1234 is an ASCII encoded 16 bit command, with the format:
//; Data Byte for boards:    1 = MSB high nibble, 2 = MSB low nibble
//; Address Byte for boards: 3 = LSB high nibble, 4 = LSB low nibble
//; S & W are literal format characters,<sp> = space, xy = CIP address (ignored).
//; 9 characters are repeated 3 times followed by a carriage return - line feed,
//; and all alpha characters must be capitalized, baud rate = 1200.
#define START_COMMAND   (uint8*)("S") //Start command string before the 4 command char 
#define START_COMMAND_SIZE   1u //Size of Start command string before the 4 command char 
#define END_COMMAND     (uint8*)(" 01W") //End command string after the 4 command char, CIP is 01 which is ignored
#define END_COMMAND_SIZE     4u //Size of End command string after the 4 command char, CIP is 01 which is ignored
#define CR    (0x0Du) //Carriage return in hex
#define LF    (0x0Au) //Line feed in hex
#define FILLBYTE (0xA3u) //SPI never transmits  so could be anything
//#define CMDBUFFSIZE 3
/* Project Defines */
#define FALSE  0
#define TRUE   1
#define SPI_BUFFER_SIZE  (64u)
//uint8 cmdBuff[CMDBUFFSIZE];
//uint8 iCmdBuff = CMDBUFFSIZE - 1;

#define USBFS_DEVICE    (0u)
/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH     (20u)

#define NUM_SPI_DEV     (1u)
uint8 iSPIDev = 0u;
#define CTR1_SEL     (0x0Fu)
const uint8 tabSPISel[NUM_SPI_DEV] = {CTR1_SEL};
#define CTR1_HEAD     (0xFAu)
const uint8 tabSPIHead[NUM_SPI_DEV] = {CTR1_HEAD};
uint8 buffSPI[NUM_SPI_DEV][SPI_BUFFER_SIZE];
uint8 buffSPIRead[NUM_SPI_DEV];
uint8 buffSPIWrite[NUM_SPI_DEV];
enum readStatus {CHECKDATA, READOUTDATA, EORFOUND, EORERROR};
#define COMMAND_CHARS     (4u)
uint8 curCmd[COMMAND_CHARS];
uint8 iCurCmd = 0;



CY_ISR(SPINext)
{
//    if (iCmdBuff < CMDBUFFSIZE - 1)
//    {
//        SPIM_BP_WriteTxData(cmdBuff[iCmdBuff++]);
//    }
//    else if (!Pin_nDrdy_Read())
//    {
//        iCmdBuff = CMDBUFFSIZE - 1;
//        SPIM_BP_WriteTxData(cmdBuff[iCmdBuff]);
//    }
    SPIM_BP_WriteTxData(FILLBYTE);
}
int main(void)
{
//    uint8 status;
//    uint8 fillByte = 0xA3u;
//    cmdBuff[CMDBUFFSIZE - 1] = FILLBYTE;
    uint8 buffUsbTx[USBUART_BUFFER_SIZE];
    uint8 iBuffUsbTx = 0;
    uint8 buffUsbRx[USBUART_BUFFER_SIZE];
    uint8 iBuffUsbRx = 0;
    uint8 nBuffUsbRx = 0;
    
    
    SPIM_BP_Start();
    SPIM_BP_ClearFIFO();
    USBUART_CD_Start(USBFS_DEVICE, USBUART_CD_5V_OPERATION);
    UART_Cmd_Start();
   
    Timer_Tsync_Start();
    Control_Reg_R_Write(0x00u);
    Control_Reg_SS_Write(tabSPISel[0]);
    isr_W_StartEx(SPINext);
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

//    cmdBuff[0] = 0x0Fu;
//    cmdBuff[1] = 0xF0u;
//    SPIM_BP_WriteTxData(cmdBuff[0]);
//    iCmdBuff = 1;
    SPIM_BP_TxDisable();
    
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
    	if (0u != USBUART_CD_IsConfigurationChanged())
    	{
    		/* Initialize IN endpoints when device is configured. */
    		if (0u != USBUART_CD_GetConfiguration())
    		{
    			/* Enumeration is done, enable OUT endpoint to receive data 
    			 * from host. */
    			USBUART_CD_CDC_Init();
    		}
    	}

        /* Service USB CDC when device is configured. */
        if ((nBuffUsbRx == iBuffUsbRx) && (0u != USBUART_CD_GetConfiguration()))
        {
            /* Check for input data from host. */
            if (0u != USBUART_CD_DataIsReady())
            {
                /* Read received data and re-enable OUT endpoint. */
                nBuffUsbRx = USBUART_CD_GetAll(buffUsbRx);
                iBuffUsbRx = 0;

            }
        }
        
        if (nBuffUsbRx > iBuffUsbRx)
        {
            uint8 nByteCpy =  MIN(COMMAND_CHARS - iCurCmd, nBuffUsbRx - iBuffUsbRx);
            if (nByteCpy > 0)
            {
                memcpy((curCmd + iCurCmd), (buffUsbRx + iBuffUsbRx), nByteCpy);
                iCurCmd += nByteCpy;
                iBuffUsbRx += nByteCpy;
                
                if ((iCurCmd >= COMMAND_CHARS) && (0u != (UART_Cmd_TX_STS_FIFO_EMPTY | UART_Cmd_ReadTxStatus())))
                {
                    uint8 cmdValid = TRUE;
                    //all nibbles of the command must be uppercase hex char 
                    for(uint8 x = 0; ((x < COMMAND_CHARS) && cmdValid); x++)
                    {
                        if ((!(isxdigit(curCmd[x]))) || (curCmd[x] > 'F'))
                        {
                            cmdValid = FALSE; 
                        }
                    }
                    if (cmdValid)
                    {
                        //DEBUG echo command no boundary check
                        memcpy(buffUsbTx, "++", 2);
                        memcpy(buffUsbTx +2, curCmd, COMMAND_CHARS);
                        iBuffUsbTx += 6;
                        //Write 3 times cmd on backplane
                        for (uint8 x=0; x<3; x++)
                        {
                            UART_Cmd_PutArray(START_COMMAND, START_COMMAND_SIZE);
                            UART_Cmd_PutArray(curCmd, COMMAND_CHARS);
                            UART_Cmd_PutArray(END_COMMAND, END_COMMAND_SIZE);
                        }
                        //Unix style line end
                        UART_Cmd_PutChar(CR);
                        UART_Cmd_PutChar(LF);    
                    }
                    else 
                    {
                        //DEBUG echo command no boundary check
                        memcpy(buffUsbTx, "--", 2);
                        memcpy(buffUsbTx +2, curCmd, COMMAND_CHARS);
                        iBuffUsbTx += 6;
                    }
                    iCurCmd = 0;    
                }
            }
        }
//                if (NewTransmit)
//        {
             /* Service USB CDC when device is configured. */
        if (0u != USBUART_CD_GetConfiguration())
        {
 
            /* Wait until component is ready to send data to host. */
            if (USBUART_CD_CDCIsReady())
            {
                USBUART_CD_PutData(buffUsbTx, iBuffUsbTx);
                iBuffUsbTx = 0;
            }
        }
                /* Send data back to host. */
               
//                NewTransmit = FALSE;
//
//
//            }
//        }
    }
}

/* [] END OF FILE */
