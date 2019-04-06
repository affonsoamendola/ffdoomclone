#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "console.h"
#include "SDL.h"
#include "point2.h"

#include "gfx.h"

extern SDL_Surface * screen;

unsigned int GFX_get_pixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    
    unsigned char *p = (unsigned char *)surface->pixels + y*surface->pitch + x*bpp;

    switch(bpp) {
	    case 1:
	        return *p;
	        break;

	    case 2:
	        return *(unsigned short int *)p;
	        break;

	    case 3:
	        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
	        {
	            return p[0] << 16 | p[1] << 8 | p[2];
	        }
	        else
	        {
	            return p[0] | p[1] << 8 | p[2] << 16;
	        }
	        break;

	    case 4:
	        return *(unsigned int *)p;
	        break;

	    default:
	        return 0;
    }
}

void GFX_set_pixel(SDL_Surface *surface, int x, int y, unsigned int pixel)
{
    int bpp = surface->format->BytesPerPixel;

    unsigned char *p = (unsigned char *)surface->pixels + y*surface->pitch + x*bpp;

    switch(bpp) {
	    case 1:
	        *p = pixel;
	        break;

	    case 2:
	        *(unsigned short int *)p = pixel;
	        break;

	    case 3:
	        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	            p[0] = (pixel >> 16) & 0xff;
	            p[1] = (pixel >> 8) & 0xff;
	            p[2] = pixel & 0xff;
	        } else {
	            p[0] = pixel & 0xff;
	            p[1] = (pixel >> 8) & 0xff;
	            p[2] = (pixel >> 16) & 0xff;
	        }
	        break;

	    case 4:
	        *(unsigned int *)p = pixel;
	        break;
    }
}

void GFX_fill_rectangle(POINT2 start, POINT2 end, unsigned int pixel)
{
	for(int i = start.x; i <= end.x; i++)
	{
		for(int j = start.y; j <= end.y; j++)
		{
			GFX_set_pixel(screen, i, j, pixel);
		}
	}
}

void GFX_draw_console()
{
	GFX_fill_rectangle(point2(0, 0), point2(319, 79), SDL_MapRGB(screen->format, 40, 40, 40));
	GFX_fill_rectangle(point2(0, 80), point2(319, 87), SDL_MapRGB(screen->format, 60, 60, 60));
}

void GFX_Render()
{
	if(SDL_MUSTLOCK(screen))
	{
		if(SDL_LockSurface(screen) < 0)
		{
			printf("Couldnt lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	GFX_fill_rectangle(point2(0,0) , point2(319, 239), SDL_MapRGB(screen->format, 0, 0, 0));

	if(get_console_open())
	{
		GFX_draw_console();	
	}

	if(SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect(screen, 0, 0, 320, 240);
}

