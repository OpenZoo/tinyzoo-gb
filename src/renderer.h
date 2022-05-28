#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stdint.h>
#include <stdbool.h>
#include <gbdk/platform.h>

#define VIEWPORT_WIDTH 20
#define VIEWPORT_HEIGHT 17
#define RENDER_MODE_PLAYFIELD 0
#define RENDER_MODE_TXTWIND 1
#define RENDER_ID_DMG 0
#define RENDER_ID_GBC 1

typedef struct {
	void (*init)(uint8_t mode);
	void (*sync_hblank_safe)(void);
	void (*undraw)(uint8_t x, uint8_t y);
	void (*draw)(uint8_t x, uint8_t y, uint8_t chr, uint8_t col);
	void (*free_line)(uint8_t y);
	void (*scroll)(int8_t dx, int8_t dy);
	void (*update)(void);
} renderer_t;

extern uint8_t hblank_isr_jp;
extern uint16_t hblank_isr_ip;

extern uint8_t renderer_id;
extern uint8_t renderer_mode;
extern uint8_t renderer_scrolling; // if 1, do not remove colors
extern uint8_t draw_offset_x;
extern uint8_t draw_offset_y;
extern uint8_t lcdc_shadow_reg;
extern uint8_t scx_shadow_reg;
extern uint8_t scy_shadow_reg;

#ifndef __POCKET__
extern const renderer_t renderer_dmg;
#endif

extern const renderer_t renderer_gbc;

extern renderer_t active_renderer;

void text_init(uint8_t mode, const renderer_t *renderer);
#define text_reinit(mode) text_init((mode), NULL)

#define text_sync_hblank_safe() active_renderer.sync_hblank_safe()
#define text_undraw(a, b) active_renderer.undraw(a, b)
#define text_draw(a, b, c, d) active_renderer.draw(a, b, c, d)
#define text_free_line(a) active_renderer.free_line(a)
#define text_scroll(a, b) active_renderer.scroll(a, b)
#define text_update() active_renderer.update()

void safe_vmemcpy(uint8_t *dest, const uint8_t *src, uint8_t blocks); // renderer_gbc.c

#endif /* __RENDERER_H__ */
