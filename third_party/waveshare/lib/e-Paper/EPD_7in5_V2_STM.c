#include "EPD_7in5_V2_STM.h"
#include "Debug.h"


/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
static void EPD_WaitUntilIdle(void) {
  /* Debug("e-Paper busy\r\n"); */
  /* EPD_SendCommand(0x71); */
  /* int busy = DEV_Digital_Read(EPD_BUSY_PIN); */
  /* while (busy == 0) { */
  /*   EPD_SendCommand(0x71); */
  /*   busy = DEV_Digital_Read(EPD_BUSY_PIN);     */
  /* } */
  /* DEV_Delay_ms(20); */
  /* Debug("e-Paper busy release\r\n");   */
    Debug("e-Paper busy\r\n");
	do{
		DEV_Delay_ms(20);
	}while(!(DEV_Digital_Read(EPD_BUSY_PIN)));
	DEV_Delay_ms(5);
    Debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_SendData2(UBYTE *pData, UDOUBLE len)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_Write_nByte(pData, len);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_7IN5_V2_TurnOnDisplay(void)
{	
    EPD_SendCommand(0x12);			//DISPLAY REFRESH
    DEV_Delay_ms(100);	        //!!!The delay here is necessary, 200uS at least!!!
    EPD_WaitUntilIdle();
}



UBYTE EPD_7IN5_V2_STM_Init(void) {
  (void)EPD_7IN5_V2_TurnOnDisplay;  
  (void)EPD_SendData2;
    EPD_Reset();
    EPD_SendCommand(0x01);			//POWER SETTING
	EPD_SendData(0x07);
	EPD_SendData(0x07);    //VGH=20V,VGL=-20V
	EPD_SendData(0x3f);		//VDH=15V
	EPD_SendData(0x3f);		//VDL=-15V

	//Enhanced display drive(Add 0x06 command)
	EPD_SendCommand(0x06);			//Booster Soft Start
	EPD_SendData(0x17);
	EPD_SendData(0x17);
	EPD_SendData(0x28);
	EPD_SendData(0x17);

	//Enhanced display drive(Add 0x06 command)

	EPD_SendCommand(0x04); //POWER ON
        DEV_Delay_ms(200);
        
	EPD_WaitUntilIdle();        //waiting for the electronic paper IC to release the idl
	return 0;
}        
