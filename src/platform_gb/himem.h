#ifndef __HIMEM_H__
#define __HIMEM_H__

#include <gbdk/platform.h>

static volatile SFR AT(0xA0) temp1; // used in VBLANK
static volatile SFR AT(0xA1) temp2; // used in VBLANK
static volatile SFR AT(0xA2) temp3; // used in VBLANK
static volatile SFR AT(0xA3) temp4; // used in VBLANK
static volatile SFR AT(0xA4) temp5; // used in VBLANK
static volatile SFR AT(0xA5) ly_bank_switch_mirror;
static volatile SFR AT(0xA6) zoo_stat_count;
static volatile SFR AT(0xA7) temp6; // NOT used in VBLANK
static volatile SFR AT(0xA8) ly_offset_mirror;

#endif
