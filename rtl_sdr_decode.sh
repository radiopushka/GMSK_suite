#!/bin/bash
rtl_fm -M usb -f 52002576 -s 192000 -g 50 -E agc -E dc | ./sdr_rx
