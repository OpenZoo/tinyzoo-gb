/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stdint.h>
#include <stdbool.h>
#include <gbdk/platform.h>
#include "config.h"

#if defined(NINTENDO)
#define MULTIPLE_RENDERERS
#endif

#define RENDER_MODE_PLAYFIELD 0
#define RENDER_MODE_TITLE 1
#define RENDER_MODE_TXTWIND 2
#define RENDER_MODE_MENU 3
#define RENDER_MODE_NONE 4

#ifdef MULTIPLE_RENDERERS
typedef struct {
	void (*init)(uint8_t mode);
	void (*sync_hblank_safe)(void);
	void (*undraw)(uint8_t x, uint8_t y);
	void (*draw)(uint8_t x, uint8_t y, uint8_t chr, uint8_t col);
	void (*free_line)(uint8_t y);
	void (*scroll)(int8_t dx, int8_t dy);
	void (*update)(void);
} renderer_t;
#endif

extern uint8_t renderer_id;
extern uint8_t renderer_mode;
extern uint8_t renderer_scrolling; // if 1, do not remove colors
extern uint8_t draw_offset_x;
extern uint8_t draw_offset_y;

#ifdef NINTENDO
extern uint8_t hblank_isr_jp;
extern uint16_t hblank_isr_ip;

extern uint8_t lcdc_shadow_reg;
extern uint8_t scx_shadow_reg;
extern uint8_t scy_shadow_reg;

#ifndef __POCKET__
extern const renderer_t renderer_dmg;
#endif
extern const renderer_t renderer_gbc;

void safe_vmemcpy(uint8_t *dest, const uint8_t *src, uint8_t blocks); // renderer_gbc.c
#endif

#ifdef MULTIPLE_RENDERERS
extern renderer_t active_renderer;

void text_init(uint8_t mode, const renderer_t *renderer);
#define text_reinit(mode) text_init((mode), NULL)

#define text_sync_hblank_safe() active_renderer.sync_hblank_safe()
#define text_undraw(a, b) active_renderer.undraw(a, b)
#define text_draw(a, b, c, d) active_renderer.draw(a, b, c, d)
#define text_free_line(a) active_renderer.free_line(a)
#define text_scroll(a, b) active_renderer.scroll(a, b)
#define text_update() active_renderer.update()
#else
void text_init(uint8_t mode);
void text_reinit(uint8_t mode);

void text_sync_hblank_safe(void);
void text_undraw(uint8_t x, uint8_t y);
void text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col);
void text_free_line(uint8_t y);
void text_scroll(int8_t dx, int8_t dy);
void text_update(void);
#endif

#endif /* __RENDERER_H__ */
