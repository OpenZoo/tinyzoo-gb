#ifndef __HIMEM_H__
#define __HIMEM_H__

#include <gb/gb.h>

static volatile SFR AT(0xA0) temp1;
static volatile SFR AT(0xA1) temp2;
static volatile SFR AT(0xA2) temp3;
static volatile SFR AT(0xA3) temp4;
static volatile SFR AT(0xA4) temp5;
static volatile SFR AT(0xA5) ly_bank_switch_mirror;
static volatile SFR AT(0xA6) zoo_stat_count;

#endif
