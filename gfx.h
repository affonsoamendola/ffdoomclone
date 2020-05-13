#ifndef GFX_H
#define GFX_H

#include "ff_stb.h"
#include "ff_point2.h"
#include "ff_rect.h"
#include "ff_vector2.h"
#include "ff_color.h"
#include "SDL2/SDL.h"

/*
typedef struct CAMERA_
{
	float * z_buffer;

	float hither_z;
	float hither_x;
	float hither_y;

	float yon_z;
	float yon_x;
	float yon_y;

	float depth_lighting_max_distance;

	float hfov;
	float vfov;

	float camera_parameters_x;
	float camera_parameters_y;
}
CAMERA;
*/


typedef struct Texture_
{
	SDL_Texture* texture;

	unsigned int w;
	unsigned int h;

	SDL_BlendMode blend_mode;
	Color modulation;
} Texture;

typedef struct TextureParam_
{
	uint32_t texture_id;
	Color color_modulation;

	bool is_parallax;

	int u_offset;
	int v_offset;

	float u_scale;
	float v_scale;

} TextureParam;

typedef struct Font_
{
	Texture texture;

	unsigned int char_w;
	unsigned int char_h;
} Font;

typedef struct FontRegistry_
{
	uint32_t size;
	Font* fonts;
} FontRegistry;

typedef struct TextureRegistry_
{
	uint32_t size;
	Texture* textures;
} TextureRegistry;

typedef struct GFX_
{
	int screen_res_x;
	int screen_res_y;

	float aspect_ratio;
	int pixel_scale;

	char* window_title;

	SDL_Window* window;
	SDL_Texture* screen_surface;
	Color* screen_pixels;

	SDL_Renderer* renderer;

	FontRegistry font_registry;
	TextureRegistry texture_registry;
} GFX;

typedef enum GFX_BLEND_MODE_
{
	GFX_NO_BLEND = 0,
	GFX_ALPHA_BLEND = 1,
	GFX_SUM_BLEND = 2,
	GFX_SUB_BLEND = 3,
	GFX_ALPHA_TINT_BLEND = 4,
	GFX_SUM_TINT_BLEND = 5,
	GFX_SUB_TINT_BLEND = 6
} GFX_BLEND_MODE;

extern GFX gfx;

//New Functions
GFX* GFX_init();
void GFX_quit();

void GFX_render_start();
void GFX_render_end();
void GFX_update_pixels();

Color GFX_get_pixel(const int x, const int y);
void GFX_set_pixel(const int x, const int y, const Color color);

void GFX_blit(	Color* src, Color* dst, 
				const Rect src_rect, const uint32_t src_pitch, 
				const Point2 dst_pos, const uint32_t dst_pitch);

void GFX_blit_alpha(	Color* src, Color* dst, 
						const Rect src_rect, const uint32_t src_pitch, 
						const Point2 dst_pos, const uint32_t dst_pitch);

void GFX_blit_blend(	Color* src, Color* dst, 
						const Rect src_rect, const uint32_t src_pitch, 
						const Rect dst_rect, const uint32_t dst_pitch,
						const GFX_BLEND_MODE blend_mode, void* blend_data);

#define GFX_BLIT_ALL rect(0,0,0,0)
#define GFX_NO_SCALE point2(1,1)

Texture* GFX_get_texture_id(uint32_t texture_id);

//Applies conversion from middle of screen being 0,0 to screen coordinates.
//Also flips y coordinates
static inline Point2 GFX_get_screen_coordinates(const Vector2f point) 
{
	return point2(	point.x + (gfx.screen_res_x/2), 
					(gfx.screen_res_y/2) - point.y);
}

void GFX_blit_texture(	Texture* src, Texture* dst, 
						Rect src_rect, Point2 dst_offset,
						Point2 scale);

void GFX_blit_texture_id( 	uint32_t texture_id_src, Texture* dst,
							Rect src_rect, Point2 dst_offset,
							Point2 scale);

int GFX_load_font_list(const char* location, FontRegistry* font_registry);
void GFX_free_font_list(FontRegistry* font_registry);

Font GFX_load_font(const char* location, uint32_t size_x, uint32_t size_y);
void GFX_free_font(Font font);

Font GFX_get_font(uint32_t font_id);

void GFX_draw_char(const Point2 position, uint32_t font_id, char character);
void GFX_draw_char_color(const Point2 position, uint32_t font_id, char character, Color color);

void GFX_draw_string(const Point2 position, uint32_t font_id, char* string);
void GFX_draw_string_color(const Point2 position, uint32_t font_id, char* string, Color color);

void GFX_draw_string_f(const Point2 position, uint32_t font_id, char* format_string, ...);
void GFX_draw_string_color_f(const Point2 position, uint32_t font_id, Color color, char* format_string, ...);

int GFX_load_texture_list(const char* location, TextureRegistry* texture_registry);
void GFX_free_texture_list(TextureRegistry* texture_registry);

Texture GFX_load_texture(const char* location);
void GFX_free_texture(Texture texture);

void GFX_texture_override_color_mod(Texture texture, const Color color);
void GFX_texture_return_color_mod(Texture texture);

void GFX_fill_screen(const Color color);

void GFX_fill_rectangle(const Rect rect, const Color color);

void GFX_draw_line(Point2 p_0, Point2 p_1, const Color color);
/*
void GFX_set_pixel_from_texture_depth_tint(	SDL_Surface *surface,
											GFX_TEXTURE_PARAM texture,
											int screen_x, int screen_y,
											int text_x, int text_y, 
											float depth, TINT tint);

void GFX_set_pixel_from_texture_tint(	SDL_Surface *surface,
										GFX_TEXTURE_PARAM texture,
										int screen_x, int screen_y,
										int text_x, int text_y, 
										TINT tint);

void GFX_set_pixel_from_texture_depth(	SDL_Surface *surface,
										GFX_TEXTURE_PARAM texture,
										int screen_x, int screen_y,
										float text_x, int text_y, float depth);

void GFX_set_pixel_from_texture(	SDL_Surface *surface,
									GFX_TEXTURE_PARAM texture,
									int screen_x, int screen_y,
									int text_x, int text_y);

void GFX_draw_wall(	int screen_x, 
					int c_height_visible, int c_height_invisible,
					int f_height_visible, int f_height_invisible,
					int x0, int x1, int u0, int u1, float z0, float z1,
					GFX_TEXTURE_PARAM texture_parameters,
					TINT tint);

void GFX_draw_visplane(	int screen_x, int visible_top, int visible_bot,
						int is_ceiling, float visplane_height, 
						GFX_TEXTURE_PARAM texture_parameters,
						TINT tint);


void GFX_Tick();

*/
#endif