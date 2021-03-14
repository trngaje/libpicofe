/*
 * (C) Gražvydas "notaz" Ignotas, 2012
 *
 * This work is licensed under the terms of any of these licenses
 * (at your option):
 *  - GNU GPL, version 2 or later.
 *  - GNU LGPL, version 2.1 or later.
 *  - MAME license.
 * See the COPYING file in the top-level directory.
 */

#include <stdio.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include "menu.h"
#include "plat.h"
#include "gl.h"
#include "plat_sdl.h"

#if 1
#include <go2/display.h>
#include <drm/drm_fourcc.h>
#endif
// XXX: maybe determine this instead..
#define WM_DECORATION_H 32

#if 1
SDL_Window *plat_sdl_screen;
#else
SDL_Surface *plat_sdl_screen;
SDL_Overlay *plat_sdl_overlay;
#endif

int plat_sdl_gl_active;
void (*plat_sdl_resize_cb)(int w, int h);
void (*plat_sdl_quit_cb)(void);

static char vid_drv_name[32];
#if 1
int window_w, window_h;
#else
static int window_w, window_h;
#endif
static int fs_w, fs_h;
static int old_fullscreen;
static int vout_mode_overlay = -1, vout_mode_gl = -1;

#if 1
go2_display_t* display = NULL;
go2_presenter_t* presenter = NULL;
go2_context_t* context = NULL;
go2_surface_t* surface = NULL;
int go2_w,go2_h;
#else
static void *display, *window;
#endif
static int gl_quirks;

/* w, h is layer resolution */
int plat_sdl_change_video_mode(int w, int h, int force)
{
  static int prev_w, prev_h;

  if (w == 0)
    w = prev_w;
  else
    prev_w = w;
  if (h == 0)
    h = prev_h;
  else
    prev_h = h;

#if 1
	window_w = w;
	window_h = h;
	
	g_menuscreen_w = window_w;
	g_menuscreen_h = window_h;
#endif
  // invalid method might come from config..
  if (plat_target.vout_method != 0
      && plat_target.vout_method != vout_mode_overlay
      && plat_target.vout_method != vout_mode_gl)
  {
    fprintf(stderr, "invalid vout_method: %d\n", plat_target.vout_method);
    plat_target.vout_method = 0;
  }
#if 0
  // skip GL recreation if window doesn't change - avoids flicker
  if (plat_target.vout_method == vout_mode_gl && plat_sdl_gl_active
      && plat_target.vout_fullscreen == old_fullscreen && !force)
  {
    return 0;
  }

  if (plat_sdl_overlay != NULL) {
    SDL_FreeYUVOverlay(plat_sdl_overlay);
    plat_sdl_overlay = NULL;
  }
  if (plat_sdl_gl_active) {
    gl_finish();
    plat_sdl_gl_active = 0;
  }
#endif

#if 1
 if (1) {
    Uint32 flags = SDL_SWSURFACE;
    int win_w = window_w;
    int win_h = window_h;

    if (plat_target.vout_fullscreen) {
      win_w = fs_w;
      win_h = fs_h;
    }
#else
  if (plat_target.vout_method != 0) {
    Uint32 flags = SDL_RESIZABLE | SDL_SWSURFACE;
    int win_w = window_w;
    int win_h = window_h;

    if (plat_target.vout_fullscreen) {
      flags |= SDL_FULLSCREEN;
      win_w = fs_w;
      win_h = fs_h;
    }
#endif
    // XXX: workaround some occasional mysterious deadlock in SDL_SetVideoMode
    // (seen on r-pi)
    SDL_PumpEvents();

#if 1
	// 윈도우가 생성되어야 키보드 입력을 받을 수 있다. @2021.3.14 일 확인됨
	// 이슈 : 커서 표시됨 .. 제거 필요함
	// SDL_CreateWindow 는 libgo 에서 생성한 surface와 같이 사용이 안되어
	// keyboard event를 받기 위해서  SDL_Init(SDL_INIT_VIDEO); 을 사용한다.
	// for SDL2.0
	
	SDL_Init(SDL_INIT_VIDEO);
/*
	SDL_Window* sdl_window = SDL_CreateWindow("pcsx Window",
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  win_w, win_h,
							  SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
*/							  
	SDL_ShowCursor(SDL_DISABLE);
	
	if (surface!=NULL)
		go2_surface_destroy(surface);
	
	surface = go2_surface_create(display, w, h, DRM_FORMAT_RGB565);

	g_menuscreen_w = go2_surface_stride_get(surface)/2; // 201025 by trngaje
								  
#else
    plat_sdl_screen = SDL_SetVideoMode(win_w, win_h, 0, flags);
#endif
    if (plat_sdl_screen == NULL) {
      fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
      plat_target.vout_method = 0;
    }
  }
#if 0
  if (plat_target.vout_method == vout_mode_overlay) {
    plat_sdl_overlay = SDL_CreateYUVOverlay(w, h, SDL_UYVY_OVERLAY, plat_sdl_screen);
    if (plat_sdl_overlay != NULL) {
      if ((long)plat_sdl_overlay->pixels[0] & 3)
        fprintf(stderr, "warning: overlay pointer is unaligned\n");

      plat_sdl_overlay_clear();
    }
    else {
      fprintf(stderr, "warning: could not create overlay.\n");
      plat_target.vout_method = 0;
    }
  }
  else if (plat_target.vout_method == vout_mode_gl) {
    plat_sdl_gl_active = (gl_init(display, window, &gl_quirks) == 0);
    if (!plat_sdl_gl_active) {
      fprintf(stderr, "warning: could not init GL.\n");
      plat_target.vout_method = 0;
    }
  }
#endif
  if (plat_target.vout_method == 0) {
    SDL_PumpEvents();

#if 0
    plat_sdl_screen = SDL_SetVideoMode(w, h, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
#endif
    if (plat_sdl_screen == NULL) {
      fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
      return -1;
    }
  }

  old_fullscreen = plat_target.vout_fullscreen;
  if (plat_sdl_resize_cb != NULL)
#if 1
    plat_sdl_resize_cb(w, h); 
#else
    plat_sdl_resize_cb(plat_sdl_screen->w, plat_sdl_screen->h);
#endif

  return 0;
}

void plat_sdl_event_handler(void *event_)
{
  static int was_active;
  SDL_Event *event = event_;

  switch (event->type) {
#if 1
  //case SDL_VIDEORESIZE: // for SDL1.2
  case SDL_WINDOWEVENT: // for SDL2.0
	switch (event->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
		
		//printf("resize %dx%d\n", event->window.data1, event->window.data2);		
		break;
		
	//  case SDL_ACTIVEEVENT:
		case SDL_WINDOWEVENT_SHOWN:
		// 여기 호출됨
		//printf("Window %d shown: %dx%d\n", event->window.windowID, window_w, window_h);			
	    break;
	}
#else
  case SDL_VIDEORESIZE:
    //printf("resize %dx%d\n", event->resize.w, event->resize.h);
    if (plat_target.vout_method != 0
        && !plat_target.vout_fullscreen && !old_fullscreen)
    {
      window_w = event->resize.w;
      window_h = event->resize.h;
      plat_sdl_change_video_mode(0, 0, 1);
    }
    break;
  case SDL_ACTIVEEVENT:
    if (event->active.gain && !was_active) {
      if (plat_sdl_overlay != NULL) {
        SDL_Rect dstrect = { 0, 0, plat_sdl_screen->w, plat_sdl_screen->h };
        SDL_DisplayYUVOverlay(plat_sdl_overlay, &dstrect);
      }
      else if (plat_sdl_gl_active) {
        if (gl_quirks & GL_QUIRK_ACTIVATE_RECREATE) {
          gl_finish();
          plat_sdl_gl_active = (gl_init(display, window, &gl_quirks) == 0);
        }
        gl_flip(NULL, 0, 0);
      }
      // else SDL takes care of it
    }
    was_active = event->active.gain;
    break;
#endif
  case SDL_QUIT:
    if (plat_sdl_quit_cb != NULL)
      plat_sdl_quit_cb();
    break;
  }
}

int plat_sdl_init(void)
{
#if 1
	static const char *vout_list[] = { NULL, NULL, NULL, NULL };
//	printf("[trngaje] plat_sdl_init\r\n");
	
	// added by trngaje
	display = go2_display_create();
	go2_w = go2_display_height_get(display);
	go2_h = go2_display_width_get(display);	
    presenter = go2_presenter_create(display, DRM_FORMAT_RGB565, 0xff080808);

    go2_context_attributes_t attr;
    attr.major = 2;
    attr.minor = 0;
    attr.red_bits = 8;
    attr.green_bits = 8;
    attr.blue_bits = 8;
    attr.alpha_bits = 8;
    attr.depth_bits = 24;
    attr.stencil_bits = 8;

	//g_menuscreen_w = 320;
	//g_menuscreen_h = 240;
	g_menuscreen_w = 640;
	g_menuscreen_h = 480;	
	//g_menuscreen_w = 368;
	//g_menuscreen_h = 480;
		
	if (surface!=NULL)
		go2_surface_destroy(surface);
	surface = go2_surface_create(display, g_menuscreen_w, g_menuscreen_h, DRM_FORMAT_RGB565);

//printf("[trngaje] plat_sdl_init: context = 0x%x, display = 0x%x, surface = 0x%x\r\n", 
//		context, display, surface);

	int *dst = (int *)go2_surface_map(surface);
	printf("[trngaje] plat_sdl_init: dst = 0x%x\r\n", dst);
	

	
	//int i = 0;
	//vout_list[i++] = "SDL Window";

	//plat_target.vout_method = vout_mode_overlay = i;
	//vout_list[i++] = "Video Overlay";

	//plat_target.vout_method = vout_mode_gl = i;
	//vout_list[i++] = "OpenGL";

	//plat_target.vout_methods = vout_list;	
	
	
	int ret = SDL_Init(SDL_INIT_NOPARACHUTE/* | SDL_INIT_JOYSTICK */);
	if (ret != 0) {
		printf("[trngaje: SDL_Init failed: %s\n", SDL_GetError());
	}

	SDL_ShowCursor(SDL_DISABLE); // by trngaje


	return 0;
#else
  static const char *vout_list[] = { NULL, NULL, NULL, NULL };
  const SDL_VideoInfo *info;
  SDL_SysWMinfo wminfo;
  int overlay_works = 0;
  int gl_works = 0;
  int i, ret, h;

  ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
  if (ret != 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return -1;
  }

  info = SDL_GetVideoInfo();
  if (info != NULL) {
    fs_w = info->current_w;
    fs_h = info->current_h;
    printf("plat_sdl: using %dx%d as fullscreen resolution\n", fs_w, fs_h);
  }

  g_menuscreen_w = 640;
/*
  if (fs_w != 0 && g_menuscreen_w > fs_w)
    g_menuscreen_w = fs_w;
*/
  g_menuscreen_h = 480;
  /*
  if (fs_h != 0) {
    h = fs_h;
    if (info && info->wm_available && h > WM_DECORATION_H)
      h -= WM_DECORATION_H;
    if (g_menuscreen_h > h)
      g_menuscreen_h = h;
  }
*/

  ret = plat_sdl_change_video_mode(g_menuscreen_w, g_menuscreen_h, 1);
  if (ret != 0) {
    plat_sdl_screen = SDL_SetVideoMode(0, 0, 16, SDL_SWSURFACE);
    if (plat_sdl_screen == NULL) {
      fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
      goto fail;
    }

    if (plat_sdl_screen->w < 320 || plat_sdl_screen->h < 240) {
      fprintf(stderr, "resolution %dx%d is too small, sorry.\n",
              plat_sdl_screen->w, plat_sdl_screen->h);
      goto fail;
    }
  }
  /*
  g_menuscreen_w = window_w = plat_sdl_screen->w;
  g_menuscreen_h = window_h = plat_sdl_screen->h;
*/
  // overlay/gl require native bpp in some cases..
  plat_sdl_screen = SDL_SetVideoMode(g_menuscreen_w, g_menuscreen_h,
    0, plat_sdl_screen->flags);
  if (plat_sdl_screen == NULL) {
    fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
    goto fail;
  }

  plat_sdl_overlay = SDL_CreateYUVOverlay(plat_sdl_screen->w, plat_sdl_screen->h,
    SDL_UYVY_OVERLAY, plat_sdl_screen);
  if (plat_sdl_overlay != NULL) {
    printf("plat_sdl: overlay: fmt %x, planes: %d, pitch: %d, hw: %d\n",
      plat_sdl_overlay->format, plat_sdl_overlay->planes, *plat_sdl_overlay->pitches,
      plat_sdl_overlay->hw_overlay);

    if (plat_sdl_overlay->hw_overlay)
      overlay_works = 1;
    else
      fprintf(stderr, "warning: video overlay is not hardware accelerated, "
                      "not going to use it.\n");
    SDL_FreeYUVOverlay(plat_sdl_overlay);
    plat_sdl_overlay = NULL;
  }
  else
    fprintf(stderr, "overlay is not available.\n");

  // get x11 display/window for GL
  SDL_VideoDriverName(vid_drv_name, sizeof(vid_drv_name));
#ifdef SDL_VIDEO_DRIVER_X11
  if (strcmp(vid_drv_name, "x11") == 0) {
    SDL_VERSION(&wminfo.version);
    ret = SDL_GetWMInfo(&wminfo);
    if (ret > 0) {
      display = wminfo.info.x11.display;
      window = (void *)wminfo.info.x11.window;
    }
  }
#else
  (void)wminfo;
#endif

  ret = gl_init(display, window, &gl_quirks);
  if (ret == 0) {
    gl_works = 1;
    gl_finish();
  }

  i = 0;
  vout_list[i++] = "SDL Window";
  if (overlay_works) {
    plat_target.vout_method = vout_mode_overlay = i;
    vout_list[i++] = "Video Overlay";
  }
  if (gl_works) {
    plat_target.vout_method = vout_mode_gl = i;
    vout_list[i++] = "OpenGL";
  }
  plat_target.vout_methods = vout_list;

  return 0;

fail:
  SDL_Quit();
  return -1;
#endif
}

void plat_sdl_finish(void)
{
#if 1
	if (context != NULL)
	{
		go2_context_destroy(context);
		context = NULL;
	}
	
	if (presenter != NULL)
	{
		go2_presenter_destroy(presenter);
		presenter = NULL;
	}
	
	if (display != NULL)
	{
		go2_display_destroy(display);
		display = NULL;	
	}
#else
  if (plat_sdl_overlay != NULL) {
    SDL_FreeYUVOverlay(plat_sdl_overlay);
    plat_sdl_overlay = NULL;
  }
  if (plat_sdl_gl_active) {
    gl_finish();
    plat_sdl_gl_active = 0;
  }
  // restore back to initial resolution
  // resolves black screen issue on R-Pi
  if (strcmp(vid_drv_name, "x11") != 0)
    SDL_SetVideoMode(fs_w, fs_h, 16, SDL_SWSURFACE);
  SDL_Quit();
#endif
}

void plat_sdl_overlay_clear(void)
{
#if 1
  int pixels = window_w*window_h;
  int *dst = (int *)go2_surface_map(surface);
#else	
  int pixels = plat_sdl_overlay->w * plat_sdl_overlay->h;
  int *dst = (int *)plat_sdl_overlay->pixels[0];
#endif
  int v = 0x10801080;

  for (; pixels > 0; dst += 4, pixels -= 2 * 4)
    dst[0] = dst[1] = dst[2] = dst[3] = v;

  for (; pixels > 0; dst++, pixels -= 2)
    *dst = v;
}

// vim:shiftwidth=2:expandtab
