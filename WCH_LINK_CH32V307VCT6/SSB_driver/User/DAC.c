/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025-03-01
* Description        : Dual sine wave DDS using DAC (PA4, PA5) at 500 kHz update rate
********************************************************************************/

#include "ch32v30x.h"
#include <math.h>
#include "./GFSK/modulator.h"
//#define BUFFER_SIZE 7398
#define BUFFER_SIZE 3702

#define PHASE_CORRECTION M_PI/3.8

uint32_t packet_buffer[BUFFER_SIZE];
uint32_t packet_buffer_flush[BUFFER_SIZE];
//max GFSK string length: 73

 struct gfsk_mod* mod;
 
void gfsk_init(short amplitude){
 mod = create_gfsk_mod(8000.0,8000,48000,amplitude);
}

void gfsk_set(char* data, int length,int USB_LSB){
   set_gfsk_data(mod,(unsigned char*)data,length);
  
   run_gfsk_mod_IQ(mod,packet_buffer,BUFFER_SIZE,PHASE_CORRECTION,USB_LSB);

}
void clear_gfsk(){
     free_gfsk_mod(mod);
}
void flush_gfsk(){
   memcpy(packet_buffer_flush,packet_buffer,BUFFER_SIZE<<2);
}

