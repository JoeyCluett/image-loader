#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
#include "sdl-core.h"
#include "BMPLoader.h"
#include "BmpOperations.h"

using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "usage: " << argv[0] << " <file to open>\n";
        return 1;
    }

    ifstream ifs(argv[1]);
    auto _bmp = BMPLoader.parseFile(ifs);
    //bmp.grayscale();
    //auto bmp = maximize_contrast(_bmp);
    //auto tmp_bmp = downsample(_bmp, 3);
    //auto bmp = maximize_contrast(tmp_bmp);

    auto bmp = maximize_contrast( downsample( greyscale( _bmp ), 3 ) );

    // if we get to this point, the file was successfully loaded
    SDL_Init(SDL_INIT_EVERYTHING);
    auto win = SDL_SetVideoMode(
            800, 600, 32, 
            SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN
    );

    struct {
        bool running = true;
    } vars;

    sdl_event_map_t event_map = {
        {
            SDL_KEYDOWN,
            [&](void* ptr) {
                auto* key_event = reinterpret_cast<SDL_KeyboardEvent*>(ptr);
                if(key_event->keysym.sym == SDLK_ESCAPE)
                    vars.running = false;
            }
        }
    };

    while(vars.running) {
        sdl_evaluate_events(event_map);
        
        SDL_FillRect(win, NULL, 0);
        int minx = (800 < bmp.width()  ? 800 : bmp.width());
        int miny = (600 < bmp.height() ? 600 : bmp.height());

        for(int j = 0; j < miny; j++) {
            for(int i = 0; i < minx; i++) {

                SDL_Rect r; r.x = i; r.y = j; r.w = 1; r.h = 1;
                auto color = bmp.at(j, i);

                SDL_FillRect(win, &r, SDL_MapRGB(win->format, color.r, color.g, color.b));
            }
        }

        SDL_Flip(win);
        SDL_Delay(50); // ~20 fps
    }

    SDL_Quit();

    return 0;
}
