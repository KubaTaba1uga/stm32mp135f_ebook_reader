/*****************************************************************************
* | File      	:   V2_EPD_IT8951.c
* | Author      :   Jakbu Buczynski
* | Function    :   IT8951 Common driver v2
* | Info        :   V1 does not support full refresh in A2 mode.
*----------------
* |	This version:   V1.0
* | Date        :   2019-09-17
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "EPD_IT8951.h"
#include "Raspberry/lib/Config/DEV_Config.h"

// basic mode definition
UBYTE INIT_Mode = 0;
UBYTE GC16_Mode = 2;
// A2_Mode's value is not fixed, is decide by firmware's LUT
UBYTE A2_Mode = 6;

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void V2_EPD_IT8951_Reset(void) {
  puts(__func__);
  DEV_Digital_Write(EPD_RST_PIN, HIGH);
  DEV_Delay_ms(200);
  DEV_Digital_Write(EPD_RST_PIN, LOW);
  DEV_Delay_ms(10);
  DEV_Digital_Write(EPD_RST_PIN, HIGH);
  DEV_Delay_ms(200);
  puts("V2_EPD_IT8951_Reset DONE");
}

/******************************************************************************
function :	Wait until the busy_pin goes HIGH
parameter:
******************************************************************************/
static void V2_EPD_IT8951_ReadBusy(void) {
  /* Debug("Busy ------\r\n"); */
  UBYTE Busy_State = DEV_Digital_Read(EPD_BUSY_PIN);
  /* 0: busy, 1: idle */
  while (Busy_State == 0) {
    Busy_State = DEV_Digital_Read(EPD_BUSY_PIN);
  }
  /* Debug("Busy Release ------\r\n"); */
}

/******************************************************************************
function :	write command
parameter:  command
******************************************************************************/
static void V2_EPD_IT8951_WriteCommand(UWORD Command) {
  // Set Preamble for Write Command
  UWORD Write_Preamble = 0x6000;

  V2_EPD_IT8951_ReadBusy();

  DEV_Digital_Write(EPD_CS_PIN, LOW);

  DEV_SPI_WriteByte(Write_Preamble >> 8);
  DEV_SPI_WriteByte(Write_Preamble);

  V2_EPD_IT8951_ReadBusy();

  DEV_SPI_WriteByte(Command >> 8);
  DEV_SPI_WriteByte(Command);

  DEV_Digital_Write(EPD_CS_PIN, HIGH);
}

/******************************************************************************
function :	write data
parameter:  data
******************************************************************************/
static void V2_EPD_IT8951_WriteData(UWORD Data) {
  puts(__func__);
  // Set Preamble for Write Command
  UWORD Write_Preamble = 0x0000;

  V2_EPD_IT8951_ReadBusy();

  DEV_Digital_Write(EPD_CS_PIN, LOW);

  /* uint8_t buf[] = {Write_Preamble>>8, Write_Preamble, Data>>8, Data}; */
  /* DEV_SPI_WriteBytes(buf, sizeof(buf)/sizeof(uint8_t)); */

  DEV_SPI_WriteByte(Write_Preamble >> 8);
  DEV_SPI_WriteByte(Write_Preamble);

  V2_EPD_IT8951_ReadBusy();

  DEV_SPI_WriteByte(Data >> 8);
  DEV_SPI_WriteByte(Data);

  DEV_Digital_Write(EPD_CS_PIN, HIGH);
}

/******************************************************************************
function :	read data
parameter:  data
******************************************************************************/
static UWORD V2_EPD_IT8951_ReadData() {
  UWORD ReadData;
  UWORD Write_Preamble = 0x1000;
  UWORD Read_Dummy;
  (void)Read_Dummy;
  V2_EPD_IT8951_ReadBusy();

  DEV_Digital_Write(EPD_CS_PIN, LOW);

  DEV_SPI_WriteByte(Write_Preamble >> 8);
  DEV_SPI_WriteByte(Write_Preamble);

  V2_EPD_IT8951_ReadBusy();

  // dummy
  Read_Dummy = DEV_SPI_ReadByte() << 8;
  Read_Dummy |= DEV_SPI_ReadByte();

  V2_EPD_IT8951_ReadBusy();

  ReadData = DEV_SPI_ReadByte() << 8;
  ReadData |= DEV_SPI_ReadByte();

  DEV_Digital_Write(EPD_CS_PIN, HIGH);

  return ReadData;
}

/******************************************************************************
function :	read multi data
parameter:  data
******************************************************************************/
static void V2_EPD_IT8951_ReadMultiData(UWORD *Data_Buf, UDOUBLE Length) {
  UWORD Write_Preamble = 0x1000;
  UWORD Read_Dummy;
  (void)Read_Dummy;
  V2_EPD_IT8951_ReadBusy();

  DEV_Digital_Write(EPD_CS_PIN, LOW);

  DEV_SPI_WriteByte(Write_Preamble >> 8);
  DEV_SPI_WriteByte(Write_Preamble);

  V2_EPD_IT8951_ReadBusy();

  // dummy
  Read_Dummy = DEV_SPI_ReadByte() << 8;
  Read_Dummy |= DEV_SPI_ReadByte();

  V2_EPD_IT8951_ReadBusy();

  for (UDOUBLE i = 0; i < Length; i++) {
    Data_Buf[i] = DEV_SPI_ReadByte() << 8;
    Data_Buf[i] |= DEV_SPI_ReadByte();
    printf("Data_Buf[%d]=0x%x\n", i, Data_Buf[i]);
  }

  DEV_Digital_Write(EPD_CS_PIN, HIGH);
}

/******************************************************************************
function :	Cmd5 WriteReg
parameter:
******************************************************************************/
static void V2_EPD_IT8951_WriteReg(UWORD Reg_Address, UWORD Reg_Value) {
  puts(__func__);
  V2_EPD_IT8951_WriteCommand(IT8951_TCON_REG_WR);
  V2_EPD_IT8951_WriteData(Reg_Address);
  V2_EPD_IT8951_WriteData(Reg_Value);
}

/******************************************************************************
function :	get VCOM
parameter:
******************************************************************************/
static UWORD V2_EPD_IT8951_GetVCOM(void) {
  puts(__func__);
  UWORD VCOM;
  V2_EPD_IT8951_WriteCommand(USDEF_I80_CMD_VCOM);
  V2_EPD_IT8951_WriteData(0x0000);
  VCOM = V2_EPD_IT8951_ReadData();
  return VCOM;
}

/******************************************************************************
function :	set VCOM
parameter:
******************************************************************************/
static void V2_EPD_IT8951_SetVCOM(UWORD VCOM) {
  V2_EPD_IT8951_WriteCommand(USDEF_I80_CMD_VCOM);
  V2_EPD_IT8951_WriteData(0x0001);
  V2_EPD_IT8951_WriteData(VCOM);
}


/******************************************************************************
function :	EPD_IT8951_Get_System_Info
parameter:
******************************************************************************/
static void V2_EPD_IT8951_GetSystemInfo(void *Buf) {
  puts(__func__);
  IT8951_Dev_Info *Dev_Info;

  V2_EPD_IT8951_WriteCommand(USDEF_I80_CMD_GET_DEV_INFO);

  V2_EPD_IT8951_ReadMultiData((UWORD *)Buf, sizeof(IT8951_Dev_Info) / 2);

  Dev_Info = (IT8951_Dev_Info *)Buf;

  Debug("Panel(W,H) = (%d,%d)\r\n", Dev_Info->Panel_W, Dev_Info->Panel_H);
  Debug("Memory Address = %X\r\n",
        Dev_Info->Memory_Addr_L | (Dev_Info->Memory_Addr_H << 16));
  Debug("FW Version = %s\r\n", (UBYTE *)Dev_Info->FW_Version);
  Debug("LUT Version = %s\r\n", (UBYTE *)Dev_Info->LUT_Version);

  assert(strlen((char *)Dev_Info->FW_Version) != 0);
  assert(strlen((char *)Dev_Info->LUT_Version) != 0);
}

/******************************************************************************
function :	Cmd1 SYS_RUN
parameter:  Run the system
******************************************************************************/
void V2_EPD_IT8951_SystemRun(void) {
  puts(__func__);
  V2_EPD_IT8951_WriteCommand(IT8951_TCON_SYS_RUN);
}

/******************************************************************************
function :	V2_EPD_IT8951_Init
parameter:
******************************************************************************/
IT8951_Dev_Info V2_EPD_IT8951_Init(UWORD VCOM) {
  IT8951_Dev_Info Dev_Info;

  V2_EPD_IT8951_Reset();

  V2_EPD_IT8951_SystemRun();

  V2_EPD_IT8951_GetSystemInfo(&Dev_Info);

  // Enable Pack write
  V2_EPD_IT8951_WriteReg(I80CPCR, 0x0001);

  // Set VCOM by handle
  if (VCOM != V2_EPD_IT8951_GetVCOM()) {
    V2_EPD_IT8951_SetVCOM(VCOM);
    Debug("VCOM = -%.02fV\n", (float)V2_EPD_IT8951_GetVCOM() / 1000);
  }

  puts("V2_EPD_IT8951_Init done");
  return Dev_Info;
}

/******************************************************************************
function :	Cmd3 SLEEP
parameter:  Sleep
******************************************************************************/
void V2_EPD_IT8951_Sleep(void)
{
    V2_EPD_IT8951_WriteCommand(IT8951_TCON_SLEEP);
}
