#!/bin/sh
if [ -f obj/gb/gbzoo.gb ]; then
	rm obj/gb/gbzoo.gb
fi
make -j7 gb
#cat obj/gb/gbzoo.gb TOWN.BIN > gbzoo.gb
#python3 tools/fix_rom_size.py gbzoo.gb
#cp gbzoo.gb obj/gb/gbzoo.gb
#rm gbzoo.gb
