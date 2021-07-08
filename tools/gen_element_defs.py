#!/usr/bin/python3

MAX_ELEMENT = 53
# init ElementDefs table
ElementDefType = type("ElementDef", (), {});
ElementDefs = []

def print_table_bool(b, n):
	if b:
		print("\t\ttrue, // %s" % n)
	else:
		print("\t\tfalse, // %s" % n)

def print_table_char(e_chr, n):
	if isinstance(e_chr, str):
		e_chr = ord(e_chr[0])
	if (e_chr >= 32 and e_chr < 127) and e_chr != 92:
		print("\t\t'%c', // %s" % (chr(e_chr), n))
	elif e_chr == 0:
		print("\t\t'\\0', // %s" % n)
	else:
		print("\t\t0x%02X, // %s" % (e_chr, n))

def strip_chars(s):
	s2 = ""
	for i in range(0, len(s)):
		c = ord(s[i])
		if (c >= ord('A') and c <= ord('Z')):
			s2 = s2 + chr(c)
		elif (c >= ord('0') and c <= ord('9')):
			s2 = s2 + chr(c)
		elif (c >= ord('a') and c <= ord('z')):
			s2 = s2 + chr(c - 32)
	return s2

def print_table():
	print("// Auto-generated file. Please do not edit directly.");
	print("");
	print("const zoo_element_def_t zoo_element_defs[MAX_ELEMENT + 1] = {");
	for i in range(0, MAX_ELEMENT + 1):
		if len(ElementDefs[i].Name) > 0:
			print("\t{ // %d - %s" % (i, ElementDefs[i].Name))
		else:
			print("\t{ // %d" % (i))
		##
		# Character
		print_table_char(ElementDefs[i].Character, "character")
		# Color
		if isinstance(ElementDefs[i].Color, str): # constant
			print("\t\t%s, // color" % ElementDefs[i].Color)
		else:
			print("\t\t0x%02X, // color" % ElementDefs[i].Color)
		flags = []
		if ElementDefs[i].Destructible:
			flags.append("ELEMENT_DESTRUCTIBLE")
		if ElementDefs[i].Pushable:
			flags.append("ELEMENT_PUSHABLE")
		if ElementDefs[i].VisibleInDark:
			flags.append("ELEMENT_VISIBLE_IN_DARK")
		if ElementDefs[i].PlaceableOnTop:
			flags.append("ELEMENT_PLACEABLE_ON_TOP")
		if ElementDefs[i].Walkable:
			flags.append("ELEMENT_WALKABLE")
		if ElementDefs[i].Cycle > -1:
			flags.append("ELEMENT_TYPICALLY_STATTED")
		# Bools
		if len(flags) > 0:
			print("\t\t%s," % " | ".join(flags))
		else:
			print("\t\t0,")
		if ElementDefs[i].HasDrawProc:
			print("\t\t%s, // draw func" % ElementDefs[i].DrawProc)
		else:
			print("\t\t0, // draw func")
		if ElementDefs[i].TickProc != "ElementDefaultTick":
			print("\t\t%s, // tick func" % ElementDefs[i].TickProc)
		else:
			print("\t\t0, // tick func")
		print("\t\t%s, // touch func" % ElementDefs[i].TouchProc)
		print("\t\t%d, // score value" % ElementDefs[i].ScoreValue)
		if i < MAX_ELEMENT:
			print("\t},");
		else:
			print("\t}");
	print("};");

for i in range(0, MAX_ELEMENT + 1):
	E = ElementDefType();
	E.Character = " ";
	E.Color = "COLOR_CHOICE_ON_BLACK";
	E.Destructible = False;
	E.Pushable = False;
	E.VisibleInDark = False;
	E.PlaceableOnTop = False;
	E.Walkable = False;
	E.HasDrawProc = False;
	E.Cycle = -1;
	E.TickProc = "ElementDefaultTick";
	E.DrawProc = "ElementDefaultDraw";
	E.TouchProc = "ElementDefaultTouch";
	E.EditorCategory = 0;
	E.EditorShortcut = 0;
	E.Name = "";
	E.CategoryName = "";
	E.Param1Name = "";
	E.Param2Name = "";
	E.ParamBulletTypeName = "";
	E.ParamBoardName = "";
	E.ParamDirName = "";
	E.ParamTextName = "";
	E.ScoreValue = 0;
	ElementDefs.append(E);

ElementDefs[0].Character = " ";
ElementDefs[0].Color = 0x70;
ElementDefs[0].Pushable = True;
ElementDefs[0].Walkable = True;
ElementDefs[0].Name = "Empty";

ElementDefs[3].Character = " ";
ElementDefs[3].Color = 0x07;
ElementDefs[3].Cycle = 1;
ElementDefs[3].TickProc = "ElementMonitorTick";
ElementDefs[3].Name = "Monitor";

ElementDefs[19].Character = 176;
ElementDefs[19].Color = 0xF9;
ElementDefs[19].PlaceableOnTop = True;
ElementDefs[19].EditorCategory = "EditorCategoryTerrain";
ElementDefs[19].TouchProc = "ElementWaterTouch";
ElementDefs[19].EditorShortcut = "W";
ElementDefs[19].Name = "Water";
ElementDefs[19].CategoryName = "Terrains:";

ElementDefs[20].Character = 176;
ElementDefs[20].Color = 0x20;
ElementDefs[20].Walkable = False;
ElementDefs[20].TouchProc = "ElementForestTouch";
ElementDefs[20].EditorCategory = "EditorCategoryTerrain";
ElementDefs[20].EditorShortcut = "F";
ElementDefs[20].Name = "Forest";

ElementDefs[4].Character = 2;
ElementDefs[4].Color = 0x1F;
ElementDefs[4].Destructible = True;
ElementDefs[4].Pushable = True;
ElementDefs[4].VisibleInDark = True;
ElementDefs[4].Cycle = 1;
ElementDefs[4].TickProc = "ElementPlayerTick";
ElementDefs[4].EditorCategory = "EditorCategoryItem";
ElementDefs[4].EditorShortcut = "Z";
ElementDefs[4].Name = "Player";
ElementDefs[4].CategoryName = "Items:";

ElementDefs[41].Character = 234;
ElementDefs[41].Color = 0x0C;
ElementDefs[41].Destructible = True;
ElementDefs[41].Pushable = True;
ElementDefs[41].Cycle = 2;
ElementDefs[41].TickProc = "ElementLionTick";
ElementDefs[41].TouchProc = "ElementDamagingTouch";
ElementDefs[41].EditorCategory = "EditorCategoryCreature";
ElementDefs[41].EditorShortcut = "L";
ElementDefs[41].Name = "Lion";
ElementDefs[41].CategoryName = "Beasts:";
ElementDefs[41].Param1Name = "Intelligence?";
ElementDefs[41].ScoreValue = 1;

ElementDefs[42].Character = 227;
ElementDefs[42].Color = 0x0B;
ElementDefs[42].Destructible = True;
ElementDefs[42].Pushable = True;
ElementDefs[42].Cycle = 2;
ElementDefs[42].TickProc = "ElementTigerTick";
ElementDefs[42].TouchProc = "ElementDamagingTouch";
ElementDefs[42].EditorCategory = "EditorCategoryCreature";
ElementDefs[42].EditorShortcut = "T";
ElementDefs[42].Name = "Tiger";
ElementDefs[42].Param1Name = "Intelligence?";
ElementDefs[42].Param2Name = "Firing rate?";
ElementDefs[42].ParamBulletTypeName = "Firing type?";
ElementDefs[42].ScoreValue = 2;

ElementDefs[44].Character = 233;
ElementDefs[44].Destructible = True;
ElementDefs[44].Cycle = 2;
ElementDefs[44].TickProc = "ElementCentipedeHeadTick";
ElementDefs[44].TouchProc = "ElementDamagingTouch";
ElementDefs[44].EditorCategory = "EditorCategoryCreature";
ElementDefs[44].EditorShortcut = "H";
ElementDefs[44].Name = "Head";
ElementDefs[44].CategoryName = "Centipedes";
ElementDefs[44].Param1Name = "Intelligence?";
ElementDefs[44].Param2Name = "Deviance?";
ElementDefs[44].ScoreValue = 1;

ElementDefs[45].Character = "O";
ElementDefs[45].Destructible = True;
ElementDefs[45].Cycle = 2;
ElementDefs[45].TickProc = "ElementCentipedeSegmentTick";
ElementDefs[45].TouchProc = "ElementDamagingTouch";
ElementDefs[45].EditorCategory = "EditorCategoryCreature";
ElementDefs[45].EditorShortcut = "S";
ElementDefs[45].Name = "Segment";
ElementDefs[45].ScoreValue = 3;

ElementDefs[18].Character = 248;
ElementDefs[18].Color = 0x0F;
ElementDefs[18].Destructible = True;
ElementDefs[18].Cycle = 1;
ElementDefs[18].TickProc = "ElementBulletTick";
ElementDefs[18].TouchProc = "ElementDamagingTouch";
ElementDefs[18].Name = "Bullet";

ElementDefs[15].Character = "S";
ElementDefs[15].Color = 0x0F;
ElementDefs[15].Destructible = False;
ElementDefs[15].Cycle = 1;
ElementDefs[15].TickProc = "ElementStarTick";
ElementDefs[15].TouchProc = "ElementDamagingTouch";
ElementDefs[15].HasDrawProc = True;
ElementDefs[15].DrawProc = "ElementStarDraw";
ElementDefs[15].Name = "Star";

ElementDefs[8].Character = 12;
ElementDefs[8].Pushable = True;
ElementDefs[8].TouchProc = "ElementKeyTouch";
ElementDefs[8].EditorCategory = "EditorCategoryItem";
ElementDefs[8].EditorShortcut = "K";
ElementDefs[8].Name = "Key";

ElementDefs[5].Character = 132;
ElementDefs[5].Color = 0x03;
ElementDefs[5].Pushable = True;
ElementDefs[5].TouchProc = "ElementAmmoTouch";
ElementDefs[5].EditorCategory = "EditorCategoryItem";
ElementDefs[5].EditorShortcut = "A";
ElementDefs[5].Name = "Ammo";

ElementDefs[7].Character = 4;
ElementDefs[7].Pushable = True;
ElementDefs[7].TouchProc = "ElementGemTouch";
ElementDefs[7].Destructible = True;
ElementDefs[7].EditorCategory = "EditorCategoryItem";
ElementDefs[7].EditorShortcut = "G";
ElementDefs[7].Name = "Gem";

ElementDefs[11].Character = 240;
ElementDefs[11].Color = "COLOR_WHITE_ON_CHOICE";
ElementDefs[11].Cycle = 0;
ElementDefs[11].VisibleInDark = True;
ElementDefs[11].TouchProc = "ElementPassageTouch";
ElementDefs[11].EditorCategory = "EditorCategoryItem";
ElementDefs[11].EditorShortcut = "P";
ElementDefs[11].Name = "Passage";
ElementDefs[11].ParamBoardName = "Room thru passage?";

ElementDefs[9].Character = 10;
ElementDefs[9].Color = "COLOR_WHITE_ON_CHOICE";
ElementDefs[9].TouchProc = "ElementDoorTouch";
ElementDefs[9].EditorCategory = "EditorCategoryItem";
ElementDefs[9].EditorShortcut = "D";
ElementDefs[9].Name = "Door";

ElementDefs[10].Character = 232;
ElementDefs[10].Color = 0x0F;
ElementDefs[10].TouchProc = "ElementScrollTouch";
ElementDefs[10].TickProc = "ElementScrollTick";
ElementDefs[10].Pushable = True;
ElementDefs[10].Cycle = 1;
ElementDefs[10].EditorCategory = "EditorCategoryItem";
ElementDefs[10].EditorShortcut = "S";
ElementDefs[10].Name = "Scroll";
ElementDefs[10].ParamTextName = "Edit text of scroll";

ElementDefs[12].Character = 250;
ElementDefs[12].Color = 0x0F;
ElementDefs[12].Cycle = 2;
ElementDefs[12].TickProc = "ElementDuplicatorTick";
ElementDefs[12].HasDrawProc = True;
ElementDefs[12].DrawProc = "ElementDuplicatorDraw";
ElementDefs[12].EditorCategory = "EditorCategoryItem";
ElementDefs[12].EditorShortcut = "U";
ElementDefs[12].Name = "Duplicator";
ElementDefs[12].ParamDirName = "Source direction?";
ElementDefs[12].Param2Name = "Duplication rate?;SF";

ElementDefs[6].Character = 157;
ElementDefs[6].Color = 0x06;
ElementDefs[6].VisibleInDark = True;
ElementDefs[6].TouchProc = "ElementTorchTouch";
ElementDefs[6].EditorCategory = "EditorCategoryItem";
ElementDefs[6].EditorShortcut = "T";
ElementDefs[6].Name = "Torch";

ElementDefs[39].Character = 24;
ElementDefs[39].Cycle = 2;
ElementDefs[39].TickProc = "ElementSpinningGunTick";
ElementDefs[39].HasDrawProc = True;
ElementDefs[39].DrawProc = "ElementSpinningGunDraw";
ElementDefs[39].EditorCategory = "EditorCategoryCreature";
ElementDefs[39].EditorShortcut = "G";
ElementDefs[39].Name = "Spinning gun";
ElementDefs[39].Param1Name = "Intelligence?";
ElementDefs[39].Param2Name = "Firing rate?";
ElementDefs[39].ParamBulletTypeName = "Firing type?";

ElementDefs[35].Character = 5;
ElementDefs[35].Color = 0x0D;
ElementDefs[35].Destructible = True;
ElementDefs[35].Pushable = True;
ElementDefs[35].Cycle = 1;
ElementDefs[35].TickProc = "ElementRuffianTick";
ElementDefs[35].TouchProc = "ElementDamagingTouch";
ElementDefs[35].EditorCategory = "EditorCategoryCreature";
ElementDefs[35].EditorShortcut = "R";
ElementDefs[35].Name = "Ruffian";
ElementDefs[35].Param1Name = "Intelligence?";
ElementDefs[35].Param2Name = "Resting time?";
ElementDefs[35].ScoreValue = 2;

ElementDefs[34].Character = 153;
ElementDefs[34].Color = 0x06;
ElementDefs[34].Destructible = True;
ElementDefs[34].Pushable = True;
ElementDefs[34].Cycle = 3;
ElementDefs[34].TickProc = "ElementBearTick";
ElementDefs[34].TouchProc = "ElementDamagingTouch";
ElementDefs[34].EditorCategory = "EditorCategoryCreature";
ElementDefs[34].EditorShortcut = "B";
ElementDefs[34].Name = "Bear";
ElementDefs[34].CategoryName = "Creatures:";
ElementDefs[34].Param1Name = "Sensitivity?";
ElementDefs[34].ScoreValue = 1;

ElementDefs[37].Character = "*";
ElementDefs[37].Color = "COLOR_CHOICE_ON_BLACK";
ElementDefs[37].Destructible = False;
ElementDefs[37].Cycle = 3;
ElementDefs[37].TickProc = "ElementSlimeTick";
ElementDefs[37].TouchProc = "ElementSlimeTouch";
ElementDefs[37].EditorCategory = "EditorCategoryCreature";
ElementDefs[37].EditorShortcut = "V";
ElementDefs[37].Name = "Slime";
ElementDefs[37].Param2Name = "Movement speed?;FS";

ElementDefs[38].Character = "^";
ElementDefs[38].Color = 0x07;
ElementDefs[38].Destructible = False;
ElementDefs[38].Cycle = 3;
ElementDefs[38].TickProc = "ElementSharkTick";
ElementDefs[38].EditorCategory = "EditorCategoryCreature";
ElementDefs[38].EditorShortcut = "Y";
ElementDefs[38].Name = "Shark";
ElementDefs[38].Param1Name = "Intelligence?";

ElementDefs[16].Character = "/";
ElementDefs[16].Cycle = 3;
ElementDefs[16].HasDrawProc = True;
ElementDefs[16].TickProc = "ElementConveyorCWTick";
ElementDefs[16].DrawProc = "ElementConveyorCWDraw";
ElementDefs[16].EditorCategory = "EditorCategoryItem";
ElementDefs[16].EditorShortcut = "1";
ElementDefs[16].Name = "Clockwise";
ElementDefs[16].CategoryName = "Conveyors:";

ElementDefs[17].Character = "\\";
ElementDefs[17].Cycle = 2;
ElementDefs[17].HasDrawProc = True;
ElementDefs[17].DrawProc = "ElementConveyorCCWDraw";
ElementDefs[17].TickProc = "ElementConveyorCCWTick";
ElementDefs[17].EditorCategory = "EditorCategoryItem";
ElementDefs[17].EditorShortcut = "2";
ElementDefs[17].Name = "Counter";

ElementDefs[21].Character = 219;
ElementDefs[21].EditorCategory = "EditorCategoryTerrain";
ElementDefs[21].CategoryName = "Walls:";
ElementDefs[21].EditorShortcut = "S";
ElementDefs[21].Name = "Solid";

ElementDefs[22].Character = 178;
ElementDefs[22].EditorCategory = "EditorCategoryTerrain";
ElementDefs[22].EditorShortcut = "N";
ElementDefs[22].Name = "Normal";

ElementDefs[31].Character = 206;
ElementDefs[31].HasDrawProc = True;
ElementDefs[31].DrawProc = "ElementLineDraw";
ElementDefs[31].Name = "Line";

ElementDefs[43].Character = 186;

ElementDefs[33].Character = 205;

ElementDefs[32].Character = "*";
ElementDefs[32].Color = 0x0A;
ElementDefs[32].EditorCategory = "EditorCategoryTerrain";
ElementDefs[32].EditorShortcut = "R";
ElementDefs[32].Name = "Ricochet";

ElementDefs[23].Character = 177;
ElementDefs[23].Destructible = False;
ElementDefs[23].EditorCategory = "EditorCategoryTerrain";
ElementDefs[23].EditorShortcut = "B";
ElementDefs[23].Name = "Breakable";

ElementDefs[24].Character = 254;
ElementDefs[24].Pushable = True;
ElementDefs[24].TouchProc = "ElementPushableTouch";
ElementDefs[24].EditorCategory = "EditorCategoryTerrain";
ElementDefs[24].EditorShortcut = "O";
ElementDefs[24].Name = "Boulder";

ElementDefs[25].Character = 18;
ElementDefs[25].TouchProc = "ElementPushableTouch";
ElementDefs[25].EditorCategory = "EditorCategoryTerrain";
ElementDefs[25].EditorShortcut = "1";
ElementDefs[25].Name = "Slider (NS)";

ElementDefs[26].Character = 29;
ElementDefs[26].TouchProc = "ElementPushableTouch";
ElementDefs[26].EditorCategory = "EditorCategoryTerrain";
ElementDefs[26].EditorShortcut = "2";
ElementDefs[26].Name = "Slider (EW)";

ElementDefs[30].Character = 197;
ElementDefs[30].TouchProc = "ElementTransporterTouch";
ElementDefs[30].HasDrawProc = True;
ElementDefs[30].DrawProc = "ElementTransporterDraw";
ElementDefs[30].Cycle = 2;
ElementDefs[30].TickProc = "ElementTransporterTick";
ElementDefs[30].EditorCategory = "EditorCategoryTerrain";
ElementDefs[30].EditorShortcut = "T";
ElementDefs[30].Name = "Transporter";
ElementDefs[30].ParamDirName = "Direction?";

ElementDefs[40].Character = 16;
ElementDefs[40].Color = "COLOR_CHOICE_ON_BLACK";
ElementDefs[40].HasDrawProc = True;
ElementDefs[40].DrawProc = "ElementPusherDraw";
ElementDefs[40].Cycle = 4;
ElementDefs[40].TickProc = "ElementPusherTick";
ElementDefs[40].EditorCategory = "EditorCategoryCreature";
ElementDefs[40].EditorShortcut = "P";
ElementDefs[40].Name = "Pusher";
ElementDefs[40].ParamDirName = "Push direction?";

ElementDefs[13].Character = 11;
ElementDefs[13].HasDrawProc = True;
ElementDefs[13].DrawProc = "ElementBombDraw";
ElementDefs[13].Pushable = True;
ElementDefs[13].Cycle = 6;
ElementDefs[13].TickProc = "ElementBombTick";
ElementDefs[13].TouchProc = "ElementBombTouch";
ElementDefs[13].EditorCategory = "EditorCategoryItem";
ElementDefs[13].EditorShortcut = "B";
ElementDefs[13].Name = "Bomb";

ElementDefs[14].Character = 127;
ElementDefs[14].Color = 0x05;
ElementDefs[14].TouchProc = "ElementEnergizerTouch";
ElementDefs[14].EditorCategory = "EditorCategoryItem";
ElementDefs[14].EditorShortcut = "E";
ElementDefs[14].Name = "Energizer";

ElementDefs[29].Character = 206;
ElementDefs[29].Cycle = 1;
ElementDefs[29].TickProc = "ElementBlinkWallTick";
ElementDefs[29].HasDrawProc = True;
ElementDefs[29].DrawProc = "ElementBlinkWallDraw";
ElementDefs[29].EditorCategory = "EditorCategoryTerrain";
ElementDefs[29].EditorShortcut = "L";
ElementDefs[29].Name = "Blink wall";
ElementDefs[29].Param1Name = "Starting time";
ElementDefs[29].Param2Name = "Period";
ElementDefs[29].ParamDirName = "Wall direction";

ElementDefs[27].Character = 178;
ElementDefs[27].EditorCategory = "EditorCategoryTerrain";
ElementDefs[27].PlaceableOnTop = True;
ElementDefs[27].Walkable = True;
ElementDefs[27].TouchProc = "ElementFakeTouch";
ElementDefs[27].EditorShortcut = "A";
ElementDefs[27].Name = "Fake";

ElementDefs[28].Character = " ";
ElementDefs[28].EditorCategory = "EditorCategoryTerrain";
ElementDefs[28].TouchProc = "ElementInvisibleTouch";
ElementDefs[28].EditorShortcut = "I";
ElementDefs[28].Name = "Invisible";

ElementDefs[36].Character = 2;
ElementDefs[36].EditorCategory = "EditorCategoryCreature";
ElementDefs[36].Cycle = 3;
ElementDefs[36].HasDrawProc = True;
ElementDefs[36].DrawProc = "ElementObjectDraw";
ElementDefs[36].TickProc = "ElementObjectTick";
ElementDefs[36].TouchProc = "ElementObjectTouch";
ElementDefs[36].EditorShortcut = "O";
ElementDefs[36].Name = "Object";
ElementDefs[36].Param1Name = "Character?";
ElementDefs[36].ParamTextName = "Edit Program";

ElementDefs[2].TickProc = "ElementMessageTimerTick";

ElementDefs[1].TouchProc = "ElementBoardEdgeTouch";

# GB/GBC port patches
# - remove unnecessary ElementBlinkWallDraw
ElementDefs[29].HasDrawProc = False;
ElementDefs[29].DrawProc = "ElementDefaultDraw";
# - add ElementPlayerDraw (to handle energizer highlights)
ElementDefs[4].HasDrawProc = True;
ElementDefs[4].DrawProc = "ElementPlayerDraw";

print_table()
