#include "../lib/Config/DEV_Config.h"
#include "Raspberry/lib/GUI/GUI_Paint.h"
#include "example.h"
#include "../lib/GUI/GUI_BMPfile.h"

#include <math.h>

#include <stdlib.h>     //exit()
#include <signal.h>     //signal()

#define Enhance false

#define USE_Factory_Test false

#define USE_Normal_Demo true

#define USE_Touch_Panel false

UWORD VCOM = 2510;

IT8951_Dev_Info Dev_Info = {0, 0};
UWORD Panel_Width;
UWORD Panel_Height;
UDOUBLE Init_Target_Memory_Addr;
int epd_mode = 0;	//0: no rotate, no mirror
					//1: no rotate, horizontal mirror, for 10.3inch
					//2: no totate, horizontal mirror, for 5.17inch
					//3: no rotate, no mirror, isColor, for 6inch color

UBYTE Display_Black(UWORD Panel_Width, UWORD Panel_Height,
		    UDOUBLE Init_Target_Memory_Addr);

void  Handler(int signo){
    Debug("\r\nHandler:exit\r\n");
    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Debug("free Refresh_Frame_Buf\r\n");
        Refresh_Frame_Buf = NULL;
    }
    if(Panel_Frame_Buf != NULL){
        free(Panel_Frame_Buf);
        Debug("free Panel_Frame_Buf\r\n");
        Panel_Frame_Buf = NULL;
    }
    if(Panel_Area_Frame_Buf != NULL){
        free(Panel_Area_Frame_Buf);
        Debug("free Panel_Area_Frame_Buf\r\n");
        Panel_Area_Frame_Buf = NULL;
    }
    if(bmp_src_buf != NULL){
        free(bmp_src_buf);
        Debug("free bmp_src_buf\r\n");
        bmp_src_buf = NULL;
    }
    if(bmp_dst_buf != NULL){
        free(bmp_dst_buf);
        Debug("free bmp_dst_buf\r\n");
        bmp_dst_buf = NULL;
    }
	if(Dev_Info.Panel_W != 0){
		Debug("Going to sleep\r\n");
		EPD_IT8951_Sleep();
	}
    DEV_Module_Exit();
    exit(0);
}


int main(int argc, char *argv[])
{
    //Exception handling:ctrl + c
    signal(SIGINT, Handler);

    if (argc < 2){
        Debug("Please input VCOM value on FPC cable!\r\n");
        Debug("Example: sudo ./epd -2.51\r\n");
        exit(1);
    }
	if (argc != 3){
		Debug("Please input e-Paper display mode!\r\n");
		Debug("Example: sudo ./epd -2.51 0 or sudo ./epd -2.51 1\r\n");
		Debug("Now, 10.3 inch glass panle is mode1, else is mode0\r\n");
		Debug("If you don't know what to type in just type 0 \r\n");
		exit(1);
    }

    //Init the BCM2835 Device
    if(DEV_Module_Init()!=0){
        return -1;
    }

    double temp;
    sscanf(argv[1],"%lf",&temp);
    VCOM = (UWORD)(fabs(temp)*1000);
    Debug("VCOM value:%d\r\n", VCOM);
	sscanf(argv[2],"%d",&epd_mode);
    Debug("Display mode:%d\r\n", epd_mode);
    Dev_Info = EPD_IT8951_Init(VCOM);

#if(Enhance)
    Debug("Attention! Enhanced driving ability, only used when the screen is blurred\r\n");
    Enhance_Driving_Capability();
#endif

    //get some important info from Dev_Info structure
    Panel_Width = Dev_Info.Panel_W;
    Panel_Height = Dev_Info.Panel_H;
    Init_Target_Memory_Addr = Dev_Info.Memory_Addr_L | (Dev_Info.Memory_Addr_H << 16);
    char* LUT_Version = (char*)Dev_Info.LUT_Version;
    if( strcmp(LUT_Version, "M641") == 0 ){
        //6inch e-Paper HAT(800,600), 6inch HD e-Paper HAT(1448,1072), 6inch HD touch e-Paper HAT(1448,1072)
        A2_Mode = 4;
        Four_Byte_Align = true;
    }else if( strcmp(LUT_Version, "M841_TFAB512") == 0 ){
        //Another firmware version for 6inch HD e-Paper HAT(1448,1072), 6inch HD touch e-Paper HAT(1448,1072)
        A2_Mode = 6;
        Four_Byte_Align = true;
    }else if( strcmp(LUT_Version, "M841") == 0 ){
        //9.7inch e-Paper HAT(1200,825)
        A2_Mode = 6;
    }else if( strcmp(LUT_Version, "M841_TFA2812") == 0 ){
        //7.8inch e-Paper HAT(1872,1404)
        A2_Mode = 6;
    }else if( strcmp(LUT_Version, "M841_TFA5210") == 0 ){
        //10.3inch e-Paper HAT(1872,1404)
        A2_Mode = 6;
    }else{
        //default set to 6 as A2 Mode
        A2_Mode = 6;
    }
    Debug("A2 Mode:%d\r\n", A2_Mode);

    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode);

    Display_Black(Panel_Width, Panel_Height, Init_Target_Memory_Addr);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode);
    
    //Show 16 grayscale
    /* Display_ColorPalette_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr); */
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode); */

    //Show some character and pattern
    /* Display_CharacterPattern_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_4); */
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode); */

    // Show a bmp file
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, A2_Mode);      */
    //1bp use A2 mode by default, before used it, refresh the screen with WHITE
    /* Display_BMP_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_1); */
    /* Display_BMP_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_2); */
    /* Display_BMP_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_4); */
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode); */
    
    //Show A2 mode refresh effect
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, A2_Mode); */
    /* Dynamic_Refresh_Example(Dev_Info,Init_Target_Memory_Addr); */
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, A2_Mode); */
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode); */
	
    //We recommended refresh the panel to white color before storing in the warehouse.
    /* EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode); */

    //EPD_IT8951_Standby();
    EPD_IT8951_Sleep();

    //In case RPI is transmitting image in no hold mode, which requires at most 10s
    DEV_Delay_ms(5000);

    DEV_Module_Exit();
    return 0;
}


UBYTE Display_Black(UWORD Panel_Width, UWORD Panel_Height,
		    UDOUBLE Init_Target_Memory_Addr) {
  puts(__func__);  
    UWORD In_4bp_Refresh_Area_Width;
    if(Four_Byte_Align == true){
        In_4bp_Refresh_Area_Width = Panel_Width - (Panel_Width % 32);
    }else{
        In_4bp_Refresh_Area_Width = Panel_Width;
    }
    UWORD In_4bp_Refresh_Area_Height = Panel_Height / 16;

    UDOUBLE Imagesize;

    clock_t In_4bp_Refresh_Start, In_4bp_Refresh_Finish;
    double In_4bp_Refresh_Duration;

    Imagesize  = ((In_4bp_Refresh_Area_Width*4 % 8 == 0)? (In_4bp_Refresh_Area_Width*4 / 8 ): (In_4bp_Refresh_Area_Width*4 / 8 + 1)) * In_4bp_Refresh_Area_Height;

    if((Refresh_Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }

    Debug("Start to demostrate 4bpp palette example\r\n");
    In_4bp_Refresh_Start = clock();

    /* UBYTE SixteenColorPattern[16] = {0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00}; */

    /* Paint_NewImage(Refresh_Frame_Buf, In_4bp_Refresh_Area_Width, In_4bp_Refresh_Area_Height, 0, BLACK); */
    /* Paint_SelectImage(Refresh_Frame_Buf); */
    /* /\* Epd_Mode(epd_mode); *\/ */
    /* Paint_SetBitsPerPixel(4); */
    /* Paint_Clear(BLACK); */

        for(int i=0; i < 16; i++){
        memset(Refresh_Frame_Buf, 0x00, Imagesize);
        EPD_IT8951_4bp_Refresh(Refresh_Frame_Buf, 0, i * In_4bp_Refresh_Area_Height, In_4bp_Refresh_Area_Width, In_4bp_Refresh_Area_Height, false, Init_Target_Memory_Addr, true);
    }

    /* memset(Refresh_Frame_Buf, 0x00, Imagesize); */
    /* EPD_IT8951_4bp_Refresh(Refresh_Frame_Buf, 0, 0, In_4bp_Refresh_Area_Width, In_4bp_Refresh_Area_Height, false, Init_Target_Memory_Addr, true); */
    /* EPD_IT8951_ */
    
    In_4bp_Refresh_Finish = clock();
    In_4bp_Refresh_Duration = (double)(In_4bp_Refresh_Finish - In_4bp_Refresh_Start) / CLOCKS_PER_SEC;
    Debug( "Write and Show 4bp occupy %f second\n", In_4bp_Refresh_Duration );

    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Refresh_Frame_Buf = NULL;
    }

    DEV_Delay_ms(5000);
    
    return 0;
}
