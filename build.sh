#!/bin/sh
if [ -f obj/gbzoo.gb ]; then
	rm obj/gbzoo.gb
fi
make -j4
cat obj/gbzoo.gb TOWN.BIN > gbzoo.gb
python3 tools/fix_rom_size.py gbzoo.gb
cp gbzoo.gb obj/gbzoo.gb
rm gbzoo.gb
