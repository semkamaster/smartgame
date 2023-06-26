#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include <SDL.h>
#include <SDL_mixer.h>
#include "utils.h"
#include "smartgame.h"

int SDL_main(int argc, char* argv[]) {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music* music = Mix_LoadMUS("Imperial-March-Star-Wars.wav");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(128);
    

    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    FrameBuffer fb{ 1280, 512, std::vector<uint32_t>(1280 * 512, pack_color(255, 255, 255)) };
    GameState gs{ Map(),                                
                  {1.5, 2.345, 1.523, M_PI / 3., 0, 0}, 
                  { {8.523, 3.812, 2, 0},               
                    {1.834, 8.765, 0, 0},
                    {5.323, 5.365, 1, 0},
                    {14.32, 14.5, 3, 0},
                    {3.523, 11.85, 1, 0},
                    {5.523, 9.35, 0, 0},
                    {14.523, 8.5, 2, 0},
                    {14.523, 1.5, 3, 0},
                    {5.5, 13.85, 3, 0},
                    {8.25, 12.95, 0, 0},
                    {9.523, 2.5, 2, 0},
                    {11.523, 10.5, 3, 0},
                    {9.523, 7.5, 3, 0},
                    {11.5, 4.6, 0, 0},
                    {13.5, 5.6, 1, 0}
                  },
                  Texture("../walltext.bmp", SDL_PIXELFORMAT_ABGR8888),
                  Texture("../monsters.bmp", SDL_PIXELFORMAT_ABGR8888) }; 
    if (!gs.tex_walls.count || !gs.tex_monst.count) {
        std::cerr << "Failed to load textures" << std::endl;
        return -1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_CreateWindowAndRenderer(fb.w, fb.h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer)) {
        std::cerr << "Failed to create window and renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture* framebuffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
    if (!framebuffer_texture) {
        std::cerr << "Failed to create framebuffer texture : " << SDL_GetError() << std::endl;
        return -1;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    while (1) {
        {
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
            if (fp_ms.count() < 20) {
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
                continue;
            }
            t1 = t2;
        }

        {
            SDL_Event event;
            if (SDL_PollEvent(&event)) {
                if (SDL_QUIT == event.type || (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym)) break;
                if (SDL_KEYUP == event.type) {
                    if ('a' == event.key.keysym.sym || 'd' == event.key.keysym.sym) gs.player.turn = 0;
                    if ('w' == event.key.keysym.sym || 's' == event.key.keysym.sym) gs.player.walk = 0;
                }
                if (SDL_KEYDOWN == event.type) {
                    if ('a' == event.key.keysym.sym) gs.player.turn = -2;
                    if ('d' == event.key.keysym.sym) gs.player.turn = 2;
                    if ('w' == event.key.keysym.sym) gs.player.walk = 3;
                    if ('s' == event.key.keysym.sym) gs.player.walk = -3;
                }
            }
        }

        {
            gs.player.a += float(gs.player.turn) * .05; 
            float nx = gs.player.x + gs.player.walk * cos(gs.player.a) * .05;
            float ny = gs.player.y + gs.player.walk * sin(gs.player.a) * .05;

            if (int(nx) >= 0 && int(nx) < int(gs.map.w) && int(ny) >= 0 && int(ny) < int(gs.map.h)) {
                if (gs.map.is_empty(nx, gs.player.y)) gs.player.x = nx;
                if (gs.map.is_empty(gs.player.x, ny)) gs.player.y = ny;
            }
            for (size_t i = 0; i < gs.monsters.size(); i++) {
                gs.monsters[i].player_dist = std::sqrt(pow(gs.player.x - gs.monsters[i].x, 2) + pow(gs.player.y - gs.monsters[i].y, 2));
            }
            std::sort(gs.monsters.begin(), gs.monsters.end()); 
        }

        render(fb, gs); 

        { 
            SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void*>(fb.img.data()), fb.w * 4);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}