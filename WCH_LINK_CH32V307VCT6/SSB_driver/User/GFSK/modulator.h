#ifndef gfskMOD
#define gfskMOD
#include "./IIR/lpf.h"
#include<stdint.h>


#define M_2PI 2 * M_PI

struct gfsk_demod{

    float am;
    float am_2;
    float am_shifter_2;
    float am_shifter;


    struct IIR* lpf_q_1;
    struct IIR* lpf_q_2;
    struct IIR* lpf_i_1;
    struct IIR* lpf_i_2;

    struct IIR* lpf_q_11;
    struct IIR* lpf_q_21;
    struct IIR* lpf_i_11;
    struct IIR* lpf_i_21;
    int dcount;
    int dcount_max;

    int prev_bval;
    int p2;

    float p_i;
    float p_q;


    uint16_t frame;

    int packet_size;

    int bit_count;

    int preamb_manchester;//0 for preamble, 1 for manchester encoding

};

struct gfsk_mod{
    float am;
    float am_shifter;

    struct IIR* lpf_mod;
    struct IIR* lpf_mod_2;

    float mod_index;
    float baseline;

    unsigned char* data;
    unsigned char* d_end;
    unsigned char* d_index;

    int bit_count;
    

    int count;
    int dcount_max;

    int DM;//direct modulation (for PLL VCOs)
           //most PLLs have capacitor decoupling so manchester encoding is a must

    float value;

    int preamb_manchester;//0 for preamble, 1 for manchester encoding
};

//gfsk demodulator
struct gfsk_demod* create_gfsk_demod(float carrier_freq,int tx_rate,int sample_rate);
struct gfsk_demod* create_gfsk_demod_FM(float carrier_freq,int tx_rate,int sample_rate);
void free_gfsk_demod(struct gfsk_demod* gfsk);
//gfsk modulator
struct gfsk_mod* create_gfsk_mod(float carrier_freq,int tx_rate,int sample_rate,float drive);
void free_gfsk_mod(struct gfsk_mod* gfsk);



int run_gfsk_demod(struct gfsk_demod* gfsk,short* in,char* obuffer,int size);//return the number of characters received
int run_gfsk_demod_FM(struct gfsk_demod* gfsk,short* in,char* obuffer,int size);//return the number of characters received
int run_gfsk_mod(struct gfsk_mod* gfsk,short* audio,int size);//return 1 if the entirity of data has been transmitter
    //for generating SSB on IQ modulators
    //if flip is set then the 90 degree and zero degree channels are flipped
    //offset is the phase offset for best rejection
int run_gfsk_mod_IQ(struct gfsk_mod* gfsk,uint32_t* audio,int size,float offset,int flip);//return 1 if the entirity of data has been transmitter

void set_gfsk_data(struct gfsk_mod* gfsk, unsigned char* data,int length);//run after gfsk_mod returns 1 to set new data
#endif
