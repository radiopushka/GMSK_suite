FLAGS = -march=native -mtune=native -O3 -funroll-loops -fomit-frame-pointer -flto -fno-signed-zeros -fno-trapping-math -fno-fast-math
DSP = ./GFSK/modulator.c ./GFSK/IIR/lpf.c

all:
	$(CC) ./gfsk_rx.c $(FLAGS) $(DSP) ./alsa/alsa.c -lm -lasound -o gfsk_rx
	$(CC) ./rtlsdr_data_rx.c $(FLAGS) $(DSP) ./alsa/alsa.c -lm -o sdr_rx
	$(CC) ./gfsk_gen.c $(FLAGS) $(DSP) ./alsa/alsa.c -lm -lasound -o gfsk_gen
	$(CC) ./gfsk_gen_IQ.c $(FLAGS) $(DSP) ./alsa/alsa.c -lm -lasound -o gfsk_IQ
