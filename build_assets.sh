#!/bin/sh
python3 tools/font2raw.py res/font_default.png 8 8 a res/font_default.bin
python3 tools/bin2c.py --bank 3 res/font_default.c res/font_default.h res/font_default.bin
python3 tools/font2raw.py res/font_small.png 4 8 a res/font_small.bin
python3 tools/bin2c.py --bank 3 res/font_small.c res/font_small.h res/font_small.bin
