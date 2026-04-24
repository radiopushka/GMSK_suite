/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : Ivan Nikitin, Deepseek
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* GMSK modulator top
* CH32V307VCT6 micro controller
* IQ modulator board
* initiates parallel DMA transfer to DAC, calls DSP synthesis code
* sets up si5351 PLL with two outputs 90deg out of phase
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/
/**
 * CH32V307 + Si5351: 140 MHz with 72бу phase shift on CLK1
 * SDA = PA2, SCL = PA0
 * Uses bit-banged I2C (open-drain, external pull-ups required)
 */

/**
 * CH32V307 + Si5351: 140 MHz with 72бу phase shift on CLK1
 * SDA = PA2, SCL = PA0
 * Uses bit-banged I2C with proper timing (approx 100 kHz)
 */

#include "ch32v30x.h"
#include <stdint.h>
#include "DAC.c"

#include <math.h>


// I2C pin definitions
#define SCL_PIN     GPIO_Pin_3
#define SDA_PIN     GPIO_Pin_2
#define I2C_PORT    GPIOA

// Si5351 I2C address (write)
#define SI5351_ADDR 0x60   // 0x60 << 1
void DAC_DMA_Init(void)
{
    // Enable DMA2 clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->RD12BDHR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)packet_buffer_flush;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;      // Memory б· peripheral
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;          // Continuous waveform
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    // Use DMA2 Channel3 (dedicated to DAC1 requests)
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
     DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);
    DMA_SetCurrDataCounter(DMA2_Channel3, BUFFER_SIZE);

    NVIC_EnableIRQ(DMA2_Channel3_IRQn);
    NVIC_SetPriority(DMA2_Channel3_IRQn, 2);

    DMA_Cmd(DMA2_Channel3, ENABLE);
}
void DAC_Config(void)
{




    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // enable both channels TIM2 trigger
     DAC->CTLR=1|(1<<16) | (1<<2)|(1<<18) | (4<<19)|(4<<3) | (1<<28)|(1<<12);

    //DAC->R12BDHR1; is the input register and expects a 12 bit value
    DAC->R12BDHR1=2047;
	DAC->R12BDHR2=2047;
     DAC_DMA_Init();

}

// ------------------------------------------------------------------
// TIM2 configuration for 500 kHz interrupt
// ------------------------------------------------------------------
void TIM2_Init(void) {
    RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

    uint32_t system_clock = 144000000; // 144MHz


    uint32_t target_freq = 48000;

    uint32_t prescaler = 0;
    uint32_t period = (system_clock / target_freq)-1;

    TIM2->PSC = prescaler;
    TIM2->ATRLR = period;


    TIM2->SWEVGR = TIM_PSCReloadMode_Immediate;
    TIM2->CTLR2 = 2<<4;
    TIM2->DMAINTENR |= TIM_UIE;

    TIM2->CTLR1 |= TIM_CEN;
    //NVIC_EnableIRQ(TIM2_IRQn);
    //NVIC_SetPriority(TIM2_IRQn, 1);
}
volatile int done=0;
__attribute__((interrupt()))
void DMA2_Channel3_IRQHandler(void)
{

    if (DMA_GetITStatus(DMA2_IT_TC3)) {

        DMA_ClearITPendingBit(DMA2_IT_TC3);


        DMA_Cmd(DMA2_Channel3, DISABLE);


        done = 1;
    }
}
// ------------------------------------------------------------------
// Bit?banged I2C functions (open?drain, ~100 kHz)
// ------------------------------------------------------------------
static void I2C_Delay(void) {
    // ~10u
	Delay_Us(100);
}

static void I2C_Start(void) {
    GPIO_SetBits(I2C_PORT, SDA_PIN);
    GPIO_SetBits(I2C_PORT, SCL_PIN);
    I2C_Delay();
    GPIO_ResetBits(I2C_PORT, SDA_PIN);
    I2C_Delay();
    GPIO_ResetBits(I2C_PORT, SCL_PIN);
    I2C_Delay();
}

static void I2C_Stop(void) {
    GPIO_ResetBits(I2C_PORT, SDA_PIN);
    GPIO_SetBits(I2C_PORT, SCL_PIN);
    I2C_Delay();
    GPIO_SetBits(I2C_PORT, SDA_PIN);
    I2C_Delay();
}

/**
 * Write one byte, return ACK status (0 = ACK, 1 = NACK)
 */
static uint8_t I2C_WriteByte(uint8_t data) {
    // Send 8 bits, MSB first
    for (int i = 0; i < 8; i++) {
        if (data & 0x80)
            GPIO_SetBits(I2C_PORT, SDA_PIN);
        else
            GPIO_ResetBits(I2C_PORT, SDA_PIN);
        data = data<<1;
        I2C_Delay();
        GPIO_SetBits(I2C_PORT, SCL_PIN);
        I2C_Delay();
        GPIO_ResetBits(I2C_PORT, SCL_PIN);
        I2C_Delay();
    }

    // Release SDA for ACK
    GPIO_SetBits(I2C_PORT, SDA_PIN);
    I2C_Delay();
    GPIO_SetBits(I2C_PORT, SCL_PIN);
    I2C_Delay();
    uint8_t ack = GPIO_ReadInputDataBit(I2C_PORT, SDA_PIN);
    I2C_Delay();
    GPIO_ResetBits(I2C_PORT, SCL_PIN);
    I2C_Delay();
    return ack;
}

// ------------------------------------------------------------------
// Si5351 register write (uses I2C)
// ------------------------------------------------------------------
static int si5351_write_reg(uint8_t reg, uint8_t val) {
    I2C_Start();
    if(I2C_WriteByte((SI5351_ADDR<<1)|0))
		return 0;
    if(I2C_WriteByte(reg))
		return -1;
    if(I2C_WriteByte(val))
		return -2;
    I2C_Stop();
	return 1;
}



// ------------------------------------------------------------------
// Main configuration
// ------------------------------------------------------------------
int main(void) {
	SystemCoreClockUpdate();
	Delay_Init();

    // Enable GPIOA clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = SCL_PIN | SDA_PIN;


    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_PORT, &gpio);

    GPIO_SetBits(I2C_PORT, SCL_PIN | SDA_PIN);
	Delay_Ms(1000);
   if(si5351_write_reg(3, 0xFF) != 1) {
        Delay_Ms(10000);
        goto exit;
    }

    // --- Disable Spread Spectrum (register 149) ---
    si5351_write_reg(149, 0x00);

    // --- PLLA = 624 MHz (25 MHz * 24.96) ---
    // Multiplier = 24 + 24/25
    uint32_t a_pll = 24;
    uint32_t b_pll = 24;
    uint32_t c_pll = 25;
    uint32_t floor128 = (128 * b_pll) / c_pll;  // floor(128*24/25) = 122

    uint32_t P1_pll = 128 * a_pll + floor128 - 512;  // = 2682
    uint32_t P2_pll = 128 * b_pll - c_pll * floor128; // = 22
    uint32_t P3_pll = c_pll;                          // = 25

    si5351_write_reg(26, (P3_pll >> 8) & 0xFF);
    si5351_write_reg(27, P3_pll & 0xFF);
    si5351_write_reg(28, (P1_pll >> 16) & 0b11);
    si5351_write_reg(29, (P1_pll >> 8) & 0xFF);
    si5351_write_reg(30, P1_pll & 0xFF);
    si5351_write_reg(31, ((P2_pll >> 16) & 0b1111) | (((P3_pll >> 16) & 0b1111) << 4));
    si5351_write_reg(32, (P2_pll >> 8) & 0xFF);
    si5351_write_reg(33, P2_pll & 0xFF);

    // --- Multisynth for CLK0 and CLK1: integer divide by 12 ---
    uint32_t div = 12;
    uint32_t P1_ms = 128 * div - 512;  // = 1024
    uint32_t P2_ms = 0;
    uint32_t P3_ms = 1;

    // CLK0 (MS0) registers 42-49
    si5351_write_reg(42, (P3_ms >> 8) & 0xFF);
    si5351_write_reg(43, P3_ms & 0xFF);
    si5351_write_reg(44, (P1_ms >> 16) & 0b11);
    si5351_write_reg(45, (P1_ms >> 8) & 0xFF);
    si5351_write_reg(46, P1_ms & 0xFF);
    si5351_write_reg(47, (((P2_ms >> 16) & 0b1111) | (((P3_ms >> 16) & 0b1111) << 4)) & 0xFF);
    si5351_write_reg(48, (P2_ms >> 8) & 0xFF);
    si5351_write_reg(49, P2_ms & 0xFF);

    // CLK1 (MS1) registers 50-57
    si5351_write_reg(50, (P3_ms >> 8) & 0xFF);
    si5351_write_reg(51, P3_ms & 0xFF);
    si5351_write_reg(52, (P1_ms >> 16) & 0b11);
    si5351_write_reg(53, (P1_ms >> 8) & 0xFF);
    si5351_write_reg(54, P1_ms & 0xFF);
    si5351_write_reg(55, (((P2_ms >> 16) & 0b1111) | (((P3_ms >> 16) & 0b1111) << 4)) & 0xFF);
    si5351_write_reg(56, (P2_ms >> 8) & 0xFF);
    si5351_write_reg(57, P2_ms & 0xFF);

    // --- Phase offset: CLK1 delayed by 3 PLL cycles (90бу at divider 12) ---
    si5351_write_reg(166, 7);  // CLK1 phase register

    // --- Output control ---
    // Enable CLK0 and CLK1 (bits 0 and 1 = 0 to enable)
    si5351_write_reg(3, 0b11111100);

    // CLK0 control: PLLA, 8mA drive, powered up (0x0C)
    si5351_write_reg(16, 0x0C);
    // CLK1 control: same
    si5351_write_reg(17, 0x0C);

    // --- Reset PLLA for clean start ---
    si5351_write_reg(177, 0x20);  // set bit 5
    Delay_Ms(10);
    si5351_write_reg(177, 0x00);  // clear

    gfsk_init(2040);
    char* string = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 - = +[]\n";
    gfsk_set(string,73,1);


    flush_gfsk();


      DAC_Config();
    TIM2_Init();



exit:



    while (1) {
        if(done){
            flush_gfsk();
            DMA_SetCurrDataCounter(DMA2_Channel3, BUFFER_SIZE);
            done=0;
            DMA_Cmd(DMA2_Channel3, ENABLE);
            gfsk_set(string,73,1);
        }
        // main loop
    }

}
