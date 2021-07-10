#ifndef __RENDERER_H__
#define __RENDERER_H__

#define VIEWPORT_WIDTH 20
#define VIEWPORT_HEIGHT 17

typedef struct {
	void (*init)(void);
	void (*draw)(uint8_t x, uint8_t y, uint8_t chr, uint8_t col);
	void (*mark_redraw)(void);
	void (*scroll)(int8_t dx, int8_t dy);
	void (*update)(void);
} renderer_t;

extern uint8_t hblank_isr_jp;
extern uint16_t hblank_isr_ip;

extern uint8_t draw_offset_x;
extern uint8_t draw_offset_y;
extern uint8_t scx_shadow_reg;
extern uint8_t scy_shadow_reg;
extern const renderer_t renderer_dmg;
extern const renderer_t renderer_gbc;

extern renderer_t active_renderer;

void text_init(const renderer_t *renderer);

#define text_draw(a, b, c, d) active_renderer.draw(a, b, c, d)
#define text_mark_redraw() active_renderer.mark_redraw()
#define text_scroll(a, b) active_renderer.scroll(a, b)
#define text_update() active_renderer.update()

#endif /* __RENDERER_H__ */
