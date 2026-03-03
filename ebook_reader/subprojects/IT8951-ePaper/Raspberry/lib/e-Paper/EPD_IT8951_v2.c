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

static void
V2_EPD_IT8951_HostPackedPixelWrite_4bp(IT8951_Load_Img_Info *Load_Img_Info,
                                       IT8951_Area_Img_Info *Area_Img_Info);
static void V2_EPD_IT8951_Display_Area(UWORD X, UWORD Y, UWORD W, UWORD H,
                                    UWORD Mode);

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
void V2_EPD_IT8951_Sleep(void) {
  V2_EPD_IT8951_WriteCommand(IT8951_TCON_SLEEP);
}

/******************************************************************************
function :	EPD_IT8951_Clear_Init
parameter:
info:           Clear screen for the first time after power on.
******************************************************************************/
void V2_EPD_IT8951_Clear_Init(IT8951_Dev_Info Dev_Info,
                              UDOUBLE Target_Memory_Addr) {
  puts(__func__);
  UDOUBLE ImageSize =
      ((Dev_Info.Panel_W * 4 % 8 == 0) ? (Dev_Info.Panel_W * 4 / 8)
                                       : (Dev_Info.Panel_W * 4 / 8 + 1)) *
      Dev_Info.Panel_H;
  UBYTE *Frame_Buf = malloc(ImageSize);
  memset(Frame_Buf, 0xFF, ImageSize);

  IT8951_Load_Img_Info Load_Img_Info;
  IT8951_Area_Img_Info Area_Img_Info;

  /* EPD_IT8951_WaitForDisplayReady(); */

  Load_Img_Info.Source_Buffer_Addr = Frame_Buf;
  Load_Img_Info.Endian_Type = IT8951_LDIMG_L_ENDIAN;
  Load_Img_Info.Pixel_Format = IT8951_4BPP;
  Load_Img_Info.Rotate = IT8951_ROTATE_0;
  Load_Img_Info.Target_Memory_Addr = Target_Memory_Addr;

  Area_Img_Info.Area_X = 0;
  Area_Img_Info.Area_Y = 0;
  Area_Img_Info.Area_W = Dev_Info.Panel_W;
  Area_Img_Info.Area_H = Dev_Info.Panel_H;

  V2_EPD_IT8951_HostPackedPixelWrite_4bp(&Load_Img_Info, &Area_Img_Info);

  V2_EPD_IT8951_Display_Area(0, 0, Dev_Info.Panel_W, Dev_Info.Panel_H,
                             INIT_Mode);

  free(Frame_Buf);
  Frame_Buf = NULL;
}

/******************************************************************************
function :	V2_EPD_IT8951_Set_Target_Memory_Addr
parameter:
******************************************************************************/
static void V2_EPD_IT8951_SetTargetMemoryAddr(UDOUBLE Target_Memory_Addr) {
  UWORD WordH = (UWORD)((Target_Memory_Addr >> 16) & 0x0000FFFF);
  UWORD WordL = (UWORD)(Target_Memory_Addr & 0x0000FFFF);

  V2_EPD_IT8951_WriteReg(LISAR + 2, WordH);
  V2_EPD_IT8951_WriteReg(LISAR, WordL);
}

/******************************************************************************
function :	Cmd10 LD_IMG
parameter:
******************************************************************************/
static void V2_EPD_IT8951_LoadImgStart(IT8951_Load_Img_Info *Load_Img_Info) {
  (void)V2_EPD_IT8951_LoadImgStart;
  UWORD Args;
  Args = (Load_Img_Info->Endian_Type << 8 | Load_Img_Info->Pixel_Format << 4 |
          Load_Img_Info->Rotate);
  V2_EPD_IT8951_WriteCommand(IT8951_TCON_LD_IMG);
  V2_EPD_IT8951_WriteData(Args);
}

/******************************************************************************
function :	Cmd12 LD_IMG_End
parameter:
******************************************************************************/
static void V2_EPD_IT8951_LoadImgEnd(void) {
  V2_EPD_IT8951_WriteCommand(IT8951_TCON_LD_IMG_END);
}

/******************************************************************************
function :	write multi data
parameter:  data
******************************************************************************/
static void V2_EPD_IT8951_WriteMuitiData(UWORD *Data_Buf, UDOUBLE Length) {
  puts(__func__);
  // Set Preamble for Write Command
  UWORD Write_Preamble = 0x0000;

  V2_EPD_IT8951_ReadBusy();

  DEV_Digital_Write(EPD_CS_PIN, LOW);

  DEV_SPI_WriteByte(Write_Preamble >> 8);
  DEV_SPI_WriteByte(Write_Preamble);

  V2_EPD_IT8951_ReadBusy();

  /* uint8_t buf */
  for (UDOUBLE i = 0; i < Length; i++) {
    DEV_SPI_WriteByte(Data_Buf[i] >> 8);
    DEV_SPI_WriteByte(Data_Buf[i]);
  }

  DEV_Digital_Write(EPD_CS_PIN, HIGH);
}

/******************************************************************************
function :	V2_EPD_IT8951_HostAreaPackedPixelWrite_4bp
parameter:
******************************************************************************/
static void
V2_EPD_IT8951_HostPackedPixelWrite_4bp(IT8951_Load_Img_Info *Load_Img_Info,
                                       IT8951_Area_Img_Info *Area_Img_Info) {
  puts(__func__);
  UWORD Source_Buffer_Width, Source_Buffer_Height;
  UWORD Source_Buffer_Length;

  UWORD *Source_Buffer = (UWORD *)Load_Img_Info->Source_Buffer_Addr;
  V2_EPD_IT8951_SetTargetMemoryAddr(Load_Img_Info->Target_Memory_Addr);
  V2_EPD_IT8951_LoadImgStart(Load_Img_Info);

  // from byte to word
  Source_Buffer_Width = (Area_Img_Info->Area_W * 4 / 8) / 2;
  Source_Buffer_Height = Area_Img_Info->Area_H;
  Source_Buffer_Length = Source_Buffer_Width * Source_Buffer_Height;

  V2_EPD_IT8951_WriteMuitiData(Source_Buffer, Source_Buffer_Length);

  V2_EPD_IT8951_LoadImgEnd();
  printf("%s DONE\n", __func__);
}


/******************************************************************************
function:	write multi arg
parameter:	data
description:	some situation like this:
* 1 commander     0    argument
* 1 commander     1    argument
* 1 commander   multi  argument
******************************************************************************/
static void V2_EPD_IT8951_WriteMultiArg(UWORD Arg_Cmd, UWORD *Arg_Buf,
                                     UWORD Arg_Num) {
  puts(__func__);    
  //Send Cmd code
  V2_EPD_IT8951_WriteCommand(Arg_Cmd);
  // Send Data
  for(UWORD i=0; i<Arg_Num; i++)
    {
      V2_EPD_IT8951_WriteData(Arg_Buf[i]);
    }
}

static void V2_EPD_IT8951_Display_Area(UWORD X, UWORD Y, UWORD W, UWORD H,
                                    UWORD Mode) {
  UWORD Args[5];
  Args[0] = X;
  Args[1] = Y;
  Args[2] = W;
  Args[3] = H;
  Args[4] = Mode;
  // 0x0034
  
  V2_EPD_IT8951_WriteMultiArg(USDEF_I80_CMD_DPY_AREA, Args, 5);
}
