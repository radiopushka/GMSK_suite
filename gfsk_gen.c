#include "./alsa/alsa.h"
#include "GFSK/modulator.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    int bsize = 1024;
    if(setup_alsa(0, "default",bsize, 48000)<0)
        printf("Failed to setup alsa\n");

    short *frame = malloc(bsize*sizeof(short));

    struct gfsk_mod* mod = create_gfsk_mod(19000.0,1500,48000,30000);


    char* string = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 - = + []\n";
    //char* string = "Hello World!\n";
    int len_st = strlen(string);

    set_gfsk_data(mod,(unsigned char*)string,len_st);
    while(1){

        if(run_gfsk_mod(mod,frame,bsize)){
            set_gfsk_data(mod,(unsigned char*)string,len_st);
        }
        awrite(frame,bsize);

    }

    free_gfsk_mod(mod);

    free_alsa();
}
