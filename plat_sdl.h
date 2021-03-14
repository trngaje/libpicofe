#include <SDL.h>
#if 1
#include <go2/display.h>
#include <drm/drm_fourcc.h>

extern int window_w, window_h;

extern go2_display_t* display;
extern go2_presenter_t* presenter;
extern go2_context_t* context;
extern go2_surface_t* surface;
#else

extern SDL_Surface *plat_sdl_screen;
extern SDL_Overlay *plat_sdl_overlay;
#endif
extern int plat_sdl_gl_active;
extern void (*plat_sdl_resize_cb)(int w, int h);
extern void (*plat_sdl_quit_cb)(void);

int plat_sdl_init(void);
int plat_sdl_change_video_mode(int w, int h, int force);
void plat_sdl_overlay_clear(void);
void plat_sdl_event_handler(void *event_);
void plat_sdl_finish(void);
