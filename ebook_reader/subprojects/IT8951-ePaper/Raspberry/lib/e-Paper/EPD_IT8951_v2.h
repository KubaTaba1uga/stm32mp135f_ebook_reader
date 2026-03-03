/*****************************************************************************
* | File      	:   EPD_IT8951_v2.h
* | Author      :   Jakub Buczynski
* | Function    :   IT8951 Common driver
* | Info        :   V2 version of driver is meant to support A2 full refresh.
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
#ifndef __EPD_IT8951_V2_H_
#define __EPD_IT8951_V2_H_

#include "EPD_IT8951.h"

IT8951_Dev_Info V2_EPD_IT8951_Init(UWORD VCOM);
void V2_EPD_IT8951_Sleep(void);
void V2_EPD_IT8951_Clear_Init(IT8951_Dev_Info Dev_Info,
                              UDOUBLE Target_Memory_Addr);

#endif

