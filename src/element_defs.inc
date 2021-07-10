// Auto-generated file. Please do not edit directly.

const zoo_element_def_t zoo_element_defs[MAX_ELEMENT + 1] = {
	{ // 0 - Empty
		' ', // character
		0x70, // color
		ELEMENT_PUSHABLE | ELEMENT_WALKABLE,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 1
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementBoardEdgeTouch, // touch func
		0, // score value
	},
	{ // 2
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		ElementMessageTimerTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 3 - Monitor
		' ', // character
		0x07, // color
		ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementMonitorTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 4 - Player
		0x02, // character
		0x1F, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_PUSHABLE | ELEMENT_VISIBLE_IN_DARK | ELEMENT_TYPICALLY_STATTED,
		ElementPlayerDraw, // draw func
		ElementPlayerTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 5 - Ammo
		0x84, // character
		0x03, // color
		ELEMENT_PUSHABLE,
		0, // draw func
		0, // tick func
		ElementAmmoTouch, // touch func
		0, // score value
	},
	{ // 6 - Torch
		0x9D, // character
		0x06, // color
		ELEMENT_VISIBLE_IN_DARK,
		0, // draw func
		0, // tick func
		ElementTorchTouch, // touch func
		0, // score value
	},
	{ // 7 - Gem
		0x04, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_PUSHABLE,
		0, // draw func
		0, // tick func
		ElementGemTouch, // touch func
		0, // score value
	},
	{ // 8 - Key
		0x0C, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_PUSHABLE,
		0, // draw func
		0, // tick func
		ElementKeyTouch, // touch func
		0, // score value
	},
	{ // 9 - Door
		0x0A, // character
		COLOR_WHITE_ON_CHOICE, // color
		0,
		0, // draw func
		0, // tick func
		ElementDoorTouch, // touch func
		0, // score value
	},
	{ // 10 - Scroll
		0xE8, // character
		0x0F, // color
		ELEMENT_PUSHABLE | ELEMENT_TYPICALLY_STATTED | ELEMENT_TYPICALLY_TEXTED,
		0, // draw func
		ElementScrollTick, // tick func
		ElementScrollTouch, // touch func
		0, // score value
	},
	{ // 11 - Passage
		0xF0, // character
		COLOR_WHITE_ON_CHOICE, // color
		ELEMENT_VISIBLE_IN_DARK | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		0, // tick func
		ElementPassageTouch, // touch func
		0, // score value
	},
	{ // 12 - Duplicator
		0xFA, // character
		0x0F, // color
		ELEMENT_TYPICALLY_STATTED,
		ElementDuplicatorDraw, // draw func
		ElementDuplicatorTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 13 - Bomb
		0x0B, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_PUSHABLE | ELEMENT_TYPICALLY_STATTED,
		ElementBombDraw, // draw func
		ElementBombTick, // tick func
		ElementBombTouch, // touch func
		0, // score value
	},
	{ // 14 - Energizer
		0x7F, // character
		0x05, // color
		0,
		0, // draw func
		0, // tick func
		ElementEnergizerTouch, // touch func
		0, // score value
	},
	{ // 15 - Star
		'S', // character
		0x0F, // color
		ELEMENT_TYPICALLY_STATTED,
		ElementStarDraw, // draw func
		ElementStarTick, // tick func
		ElementDamagingTouch, // touch func
		0, // score value
	},
	{ // 16 - Clockwise
		'/', // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED,
		ElementConveyorCWDraw, // draw func
		ElementConveyorCWTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 17 - Counter
		0x5C, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED,
		ElementConveyorCCWDraw, // draw func
		ElementConveyorCCWTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 18 - Bullet
		0xF8, // character
		0x0F, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementBulletTick, // tick func
		ElementDamagingTouch, // touch func
		0, // score value
	},
	{ // 19 - Water
		0xB0, // character
		0xF9, // color
		ELEMENT_PLACEABLE_ON_TOP,
		0, // draw func
		0, // tick func
		ElementWaterTouch, // touch func
		0, // score value
	},
	{ // 20 - Forest
		0xB0, // character
		0x20, // color
		0,
		0, // draw func
		0, // tick func
		ElementForestTouch, // touch func
		0, // score value
	},
	{ // 21 - Solid
		0xDB, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 22 - Normal
		0xB2, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 23 - Breakable
		0xB1, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 24 - Boulder
		0xFE, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_PUSHABLE,
		0, // draw func
		0, // tick func
		ElementPushableTouch, // touch func
		0, // score value
	},
	{ // 25 - Slider (NS)
		0x12, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementPushableTouch, // touch func
		0, // score value
	},
	{ // 26 - Slider (EW)
		0x1D, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementPushableTouch, // touch func
		0, // score value
	},
	{ // 27 - Fake
		0xB2, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_PLACEABLE_ON_TOP | ELEMENT_WALKABLE,
		0, // draw func
		0, // tick func
		ElementFakeTouch, // touch func
		0, // score value
	},
	{ // 28 - Invisible
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementInvisibleTouch, // touch func
		0, // score value
	},
	{ // 29 - Blink wall
		0xCE, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementBlinkWallTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 30 - Transporter
		0xC5, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED,
		ElementTransporterDraw, // draw func
		ElementTransporterTick, // tick func
		ElementTransporterTouch, // touch func
		0, // score value
	},
	{ // 31 - Line
		0xCE, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		ElementLineDraw, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 32 - Ricochet
		'*', // character
		0x0A, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 33
		0xCD, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 34 - Bear
		0x99, // character
		0x06, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_PUSHABLE | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementBearTick, // tick func
		ElementDamagingTouch, // touch func
		1, // score value
	},
	{ // 35 - Ruffian
		0x05, // character
		0x0D, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_PUSHABLE | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementRuffianTick, // tick func
		ElementDamagingTouch, // touch func
		2, // score value
	},
	{ // 36 - Object
		0x02, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED | ELEMENT_TYPICALLY_TEXTED,
		ElementObjectDraw, // draw func
		ElementObjectTick, // tick func
		ElementObjectTouch, // touch func
		0, // score value
	},
	{ // 37 - Slime
		'*', // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementSlimeTick, // tick func
		ElementSlimeTouch, // touch func
		0, // score value
	},
	{ // 38 - Shark
		'^', // character
		0x07, // color
		ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementSharkTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 39 - Spinning gun
		0x18, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED,
		ElementSpinningGunDraw, // draw func
		ElementSpinningGunTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 40 - Pusher
		0x10, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_TYPICALLY_STATTED,
		ElementPusherDraw, // draw func
		ElementPusherTick, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 41 - Lion
		0xEA, // character
		0x0C, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_PUSHABLE | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementLionTick, // tick func
		ElementDamagingTouch, // touch func
		1, // score value
	},
	{ // 42 - Tiger
		0xE3, // character
		0x0B, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_PUSHABLE | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementTigerTick, // tick func
		ElementDamagingTouch, // touch func
		2, // score value
	},
	{ // 43
		0xBA, // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 44 - Head
		0xE9, // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementCentipedeHeadTick, // tick func
		ElementDamagingTouch, // touch func
		1, // score value
	},
	{ // 45 - Segment
		'O', // character
		COLOR_CHOICE_ON_BLACK, // color
		ELEMENT_DESTRUCTIBLE | ELEMENT_TYPICALLY_STATTED,
		0, // draw func
		ElementCentipedeSegmentTick, // tick func
		ElementDamagingTouch, // touch func
		3, // score value
	},
	{ // 46
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 47
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 48
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 49
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 50
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 51
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 52
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	},
	{ // 53
		' ', // character
		COLOR_CHOICE_ON_BLACK, // color
		0,
		0, // draw func
		0, // tick func
		ElementDefaultTouch, // touch func
		0, // score value
	}
};
