#!/bin/sh
if [ -f obj/pocket/gbzoo.pocket ]; then
	rm obj/pocket/gbzoo.pocket
fi
make -j7 pocket
cat obj/pocket/gbzoo.pocket TOWN.BIN > gbzoo.pocket
python3 tools/fix_rom_size.py gbzoo.pocket
cp gbzoo.pocket obj/pocket/gbzoo.pocket
rm gbzoo.pocket
