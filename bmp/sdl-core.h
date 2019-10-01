#pragma once

#include <SDL/SDL.h>
#include <map>
#include <functional>

typedef std::map<uint8_t, std::function<void(void*)>> sdl_event_map_t;

#define call_resp(elem) iter->second(reinterpret_cast<void*>(&ev.elem)); break

void sdl_evaluate_events(const sdl_event_map_t& event_map) {

    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        auto iter = event_map.find(ev.type);
        if(iter != event_map.end()) {
            // there is a legitimate callback 
            // for this event. call it
        
            switch(ev.type) {
                case SDL_ACTIVEEVENT:     call_resp(active);
                case SDL_KEYDOWN:         call_resp(key);
                case SDL_KEYUP:           call_resp(key);
                case SDL_MOUSEMOTION:     call_resp(motion);
                case SDL_MOUSEBUTTONDOWN: call_resp(button);
                case SDL_MOUSEBUTTONUP:   call_resp(button);
                case SDL_JOYAXISMOTION:   call_resp(jaxis);
                case SDL_JOYBALLMOTION:   call_resp(jball);
                case SDL_JOYHATMOTION:    call_resp(jhat);
                case SDL_JOYBUTTONDOWN:   call_resp(jbutton);
                case SDL_JOYBUTTONUP:     call_resp(jbutton);
                case SDL_QUIT:            call_resp(quit);
                case SDL_SYSWMEVENT:      call_resp(syswm);
                case SDL_VIDEORESIZE:     call_resp(resize);
                case SDL_VIDEOEXPOSE:     call_resp(expose);
                case SDL_USEREVENT:       call_resp(user);
                default:
                    throw std::runtime_error("Error in sdl_evaluate_events: unknown event type"); 
            }
        }
    }

}

#undef call_resp