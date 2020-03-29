#include <SDL2/SDL.h>
#include <stdio.h>
#include <Windows.h>
#include "map.h"
char* pixels="###360000a###";
SDL_Rect images[]={{0,0,100,100},
                   {100,0,50,50},
                   {200,0,100,100},
                   {0,100,100,100},
                   {100,100,100,100},
                   {200,100,100,100},
                   {0,200,100,100},
                   {100,200,100,100},
                   {200,200,100,100}
};
int main(void){
    if(SDL_Init(SDL_INIT_VIDEO)==-1){
        SDL_Quit();
        return -1;
    }
    SDL_Window* window=SDL_CreateWindow("Lasers map creator",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,600,480,SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window,300,300);
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface* surface=SDL_CreateRGBSurface(0,300,300,32,0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
    surface->pixels=pixels;
    SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,surface);
    SDL_Rect selected={150,50,50,50};
    SDL_Rect black={150,0,50,50};
    SDL_Rect gray={100,50,50,50};
    SDL_Rect full_window={0,0,600,480};
    SDL_Rect full={0,0,300,300};//pour faire des tests
    int continuer=1;
    int camerax=0;
    int cameray=0;
    int selectedx=-1;
    int selectedy=-1;
    Map map=Map_Create("506#243#718!1");
    printf("%d %d %d %d\n",map.width,map.height,map.data,map.default_);
    SDL_Event event;
    while(continuer){
        if(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    continuer=0;
                    break;
                case SDL_WINDOWEVENT:
                    switch(event.window.event){
                        case SDL_WINDOWEVENT_RESIZED:
                            printf("Resized: %dx%d\n",event.window.data1,event.window.data2);
                            full_window.w=event.window.data1;
                            full_window.h=event.window.data2;
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON_MMASK){
                        camerax-=event.motion.xrel;
                        cameray-=event.motion.yrel;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button==SDL_BUTTON_LEFT){
                        if((event.button.x+camerax)/100==selectedx && (event.button.y+cameray)/100==selectedy){
                            selectedx=-1;
                            selectedy=-1;
                        } else {
                            selectedx=(event.button.x+camerax)/100;
                            selectedy=(event.button.y+cameray)/100;
                        }
                    }
                    break;
            }
        }
        if(map.width*100-camerax+100<full_window.w){
            camerax=map.width*100-full_window.w+100;
        }
        if(camerax<0){
            camerax=0;
        }
        if(map.height*100-cameray+100<full_window.h){
            cameray=map.height*100-full_window.h+100;
        }
        if(cameray<0){
            cameray=0;
        }
        SDL_RenderCopy(renderer,texture,&black,&full_window);
        for(int i=0;i<map.height;i++){
            for(int j=0;j<map.width;j++){
                SDL_Rect dest={j*100-camerax,i*100-cameray,100,100};
                SDL_RenderCopy(renderer,texture,images+Map_GetAt(map,j,i),&dest);
            }
        }
        SDL_Rect dest={selectedx*100-camerax,selectedy*100-cameray,100,100};
        SDL_RenderCopy(renderer,texture,&selected,&dest);
        dest.h=100;
        dest.y=full_window.h-100;
        dest.x=0;
        dest.w=full_window.w;
        SDL_RenderCopy(renderer,texture,&gray,&dest);
        dest.h=full_window.h;
        dest.y=0;
        dest.x=full_window.w-100;
        dest.w=100;
        SDL_RenderCopy(renderer,texture,&gray,&dest);
        SDL_RenderPresent(renderer);
    }
    return 0;
}
