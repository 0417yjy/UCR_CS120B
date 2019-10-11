# UCR_CS120B_Lab02

compile command : avr-gcc -mmcu=atmega1284 -I/usr/csshare/pkgs/simavr/ -Iheader/ -D_SIMULATE_ -Wall -g -Og -Wl,--undefined=_mmcu,--section-start=.mmcu=910000 -o build/objects/main.elf source/main.c

simulate command : simavr -v -v -v -v -g -m atmega1284 -f 8000000 build/objects/main.elf

gtkwave command : gtkwave build/results/Lab2_introToAVR_trace.vcd
