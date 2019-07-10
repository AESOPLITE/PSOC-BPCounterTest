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
//#define WRAPINC(a,b) (((a)>=(b-1))?(0):(a + 1))
#define WRAPINC(a,b) ((a + 1) % (b))
#define WRAP3INC(a,b) ((a + 3) % (b))
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
const uint8 frame00FF[2] = {0x00u, 0xFFu};
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
    uint8 tempBuffWrite = buffSPIWrite[iSPIDev];
    if (tempBuffWrite != buffSPIRead[iSPIDev]) //Check if buffer is full
    {
        buffSPIWrite[iSPIDev] = WRAPINC(tempBuffWrite, SPI_BUFFER_SIZE);
        if (0u !=(SPIM_BP_RX_STATUS_REG & SPIM_BP_STS_RX_FIFO_NOT_EMPTY))
        {
            buffSPI[iSPIDev][tempBuffWrite] = SPIM_BP_ReadRxData();
        }
        if ((0u == Pin_nDrdy_Read()))
        {
            SPIM_BP_WriteTxData(FILLBYTE);
        }
    }
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
    enum readStatus readStatusBP = CHECKDATA;
    
    uint16 tempSpinTimer = 0; //TODO replace
    
    SPIM_BP_Start();
    SPIM_BP_ClearFIFO();
    USBUART_CD_Start(USBFS_DEVICE, USBUART_CD_5V_OPERATION);
    UART_Cmd_Start();
   
    Timer_Tsync_Start();
    Control_Reg_R_Write(0x00u);

    Control_Reg_SS_Write(tabSPISel[0u]);
    Control_Reg_CD_Write(1u);
    isr_W_StartEx(SPINext);
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

//    cmdBuff[0] = 0x0Fu;
//    cmdBuff[1] = 0xF0u;
//    SPIM_BP_WriteTxData(cmdBuff[0]);
//    iCmdBuff = 1;
    SPIM_BP_TxDisable();
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    memset(buffSPIRead, 0, NUM_SPI_DEV);
    memset(buffSPIWrite, 0, NUM_SPI_DEV);
    
    
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
            }
                
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
                    memcpy(buffUsbTx + 2, curCmd, COMMAND_CHARS);
                    iBuffUsbTx += 6;
                }
                iCurCmd = 0;    
            }
            
        }
        
        switch (readStatusBP)
        {
            case CHECKDATA:
                if (0u == Pin_nDrdy_Read())
                {
                    uint8 tempBuffWrite = buffSPIWrite[iSPIDev];
                    Control_Reg_CD_Write(0u);
                    buffSPIWrite[iSPIDev] = WRAP3INC(tempBuffWrite, SPI_BUFFER_SIZE);
                    buffSPI[iSPIDev][tempBuffWrite] = tabSPIHead[iSPIDev];
                    tempBuffWrite=WRAPINC(tempBuffWrite, SPI_BUFFER_SIZE);
                    if((SPI_BUFFER_SIZE - 1) == tempBuffWrite) //check for 2 byte wrap
                    {
                        buffSPI[iSPIDev][(SPI_BUFFER_SIZE - 1)] = frame00FF[0];
                        buffSPI[iSPIDev][0] = frame00FF[1];
                    }
                    else
                    {
                        memcpy(&(buffSPI[iSPIDev][tempBuffWrite]), frame00FF, 2);
                    }
                    SPIM_BP_WriteTxData(FILLBYTE);
                    readStatusBP = READOUTDATA;
                    tempSpinTimer = 0;
                }
                else
                {  
//                    if (iSPIDev >= (NUM_SPI_DEV - 1))
//                    {
//                        iSPIDev = 0;
//                    }
//                    else
//                    {
//                        iSPIDev++;
//                    }
                    if (0x0FFFu == ++tempSpinTimer)
                    {
                        Control_Reg_CD_Write(0u);
                        iSPIDev = WRAPINC(iSPIDev, NUM_SPI_DEV);
                        Control_Reg_SS_Write(tabSPISel[iSPIDev]);
                        Control_Reg_CD_Write(1u);
                        tempSpinTimer = 0;
                    }
                }
                break;
                
            case READOUTDATA:
                //TODO actually check 3 byte EOR, count errrors 
                if (1u == Pin_nDrdy_Read())
                {
                    uint8 tempBuffWrite = buffSPIWrite[iSPIDev];
                    
                    uint8 nBytes;
                    
                    if (buffSPIRead[iSPIDev] >= tempBuffWrite)
                    {
                        nBytes = SPI_BUFFER_SIZE - buffSPIRead[iSPIDev];
                        memcpy((buffUsbTx + iBuffUsbTx), &(buffSPI[iSPIDev][buffSPIRead[iSPIDev]]), nBytes);
                        iBuffUsbTx += nBytes;
                        buffSPIRead[iSPIDev] = 0;
                    }
                    nBytes = tempBuffWrite - buffSPIRead[iSPIDev];
                    memcpy((buffUsbTx + iBuffUsbTx), &(buffSPI[iSPIDev][buffSPIRead[iSPIDev]]), nBytes);
                    iBuffUsbTx += nBytes;
                    buffSPIRead[iSPIDev] = tempBuffWrite;
                    readStatusBP = EORFOUND;
                }
                else if (buffSPIRead[iSPIDev] == buffSPIWrite[iSPIDev])
                {
                                        
                    uint8 nBytes = SPI_BUFFER_SIZE - buffSPIRead[iSPIDev];
                    
                    
                    memcpy((buffUsbTx + iBuffUsbTx), &(buffSPI[iSPIDev][buffSPIRead[iSPIDev]]), nBytes);
                    iBuffUsbTx += nBytes;
                    if (nBytes < SPI_BUFFER_SIZE)
                    {
                        nBytes = SPI_BUFFER_SIZE - nBytes;
                        memcpy((buffUsbTx + iBuffUsbTx), &(buffSPI[iSPIDev][0]), nBytes);
                        iBuffUsbTx += nBytes;
                    }
                    readStatusBP = EORERROR;
                }
                break;
                
            case EORERROR:
            case EORFOUND:  
                
                if (0u != (SPIM_BP_STS_SPI_IDLE | SPIM_BP_ReadTxStatus()))
                {
                    if (0u !=(SPIM_BP_RX_STATUS_REG & SPIM_BP_STS_RX_FIFO_NOT_EMPTY)) //Readout any further bytes
                    {   
                        
                        uint8 tempBuffWrite = buffSPIWrite[iSPIDev];
                        buffSPIWrite[iSPIDev] = WRAPINC(tempBuffWrite, SPI_BUFFER_SIZE);
                        buffSPI[iSPIDev][tempBuffWrite] = SPIM_BP_ReadRxData();
                    }
                    else
                    {
                        if (buffSPIRead[iSPIDev] != buffSPIWrite[iSPIDev])
                        {
                            uint8 tempBuffWrite = buffSPIWrite[iSPIDev];
                    
                            uint8 nBytes;
                            
                            if (buffSPIRead[iSPIDev] >= tempBuffWrite)
                            {
                                nBytes = SPI_BUFFER_SIZE - buffSPIRead[iSPIDev];
                                memcpy((buffUsbTx + iBuffUsbTx), &(buffSPI[iSPIDev][buffSPIRead[iSPIDev]]), nBytes);
                                iBuffUsbTx += nBytes;
                                buffSPIRead[iSPIDev] = 0;
                            }
                            nBytes = tempBuffWrite - buffSPIRead[iSPIDev];
                            memcpy((buffUsbTx + iBuffUsbTx), &(buffSPI[iSPIDev][buffSPIRead[iSPIDev]]), nBytes);
                            iBuffUsbTx += nBytes;
                            buffSPIRead[iSPIDev] = tempBuffWrite;
                        }
                        iSPIDev = WRAPINC(iSPIDev, NUM_SPI_DEV);
                        Control_Reg_SS_Write(tabSPISel[iSPIDev]);
                        Control_Reg_CD_Write(1u);
                        readStatusBP = CHECKDATA;
                    }
                }
                break;
        }
//                if (NewTransmit)
//        {
             /* Service USB CDC when device is configured. */
        if ((0u != USBUART_CD_GetConfiguration()) && (iBuffUsbTx > 0))
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
