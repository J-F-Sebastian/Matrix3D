#ifndef M3D_DISPLAY_H
#define M3D_DISPLAY_H

#include <SDL.h>
#include <cstdint>

class m3d_display
{
public:
    /** Default constructor */
    m3d_display() : window(0), screenSurface(0), renderer(0), xmax(0), ymax(0){};
    m3d_display(int xres, int yres);
    /** Default destructor */
    ~m3d_display();

    uint32_t *get_video_buffer(int16_t x0, int16_t y0);

    int get_xmax(void) { return xmax; }
    int get_ymax(void) { return ymax; }

    /** Act directly on the surface buffer */
    void fill_buffer(void);
    void show_buffer(void);
    void clear_buffer(void);

    /** Act on the renderer */
    void set_color(uint8_t red, uint8_t green, uint8_t blue);
    void draw_lines(SDL_Point pts[], unsigned ptsnum);
    void clear_renderer(void);
    void show_renderer(void);

private:
    //The window we'll be rendering to
    SDL_Window *window;
    //The surface contained by the window
    SDL_Surface *screenSurface;
    //The SDL renderer
    SDL_Renderer *renderer;
    //The window resolution
    int xmax, ymax;
};

#endif