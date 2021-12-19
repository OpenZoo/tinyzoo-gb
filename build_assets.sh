#!/bin/sh
echo "[ Compiling 8x8 font ]"
python3 tools/font2raw.py res/font_default.png 8 8 a res/font_default.bin
python3 tools/bin2c.py --bank 3 res/font_default.c res/font_default.h res/font_default.bin
echo "[ Compiling 4x8 font ]"
python3 tools/font2raw.py res/font_small.png 4 8 b res/font_small.bin
python3 tools/bin2c.py --bank 3 res/font_small.c res/font_small.h res/font_small.bin
echo "[ Compiling message constants ]"
python3 tools/gen_message_consts.py res/message_consts.properties res/message_consts.c res/message_consts.h
echo "[ Compiling transition tables ]"
python3 tools/gen_transition_table.py res/game_transition_table.inc 20 17
