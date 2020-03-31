#include <SDL2/SDL.h>
#include <stdio.h>
#include <Windows.h>
#include "map.h"
char* pixels="###360000a###";
SDL_Rect images[]={{0,0,100,100},
                   {100,0,1,1},
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
    SDL_Rect selected={101,1,1,1};
    SDL_Rect black={101,0,1,1};
    SDL_Rect gray={100,1,1,1};
    SDL_Rect full_window={0,0,600,480};
    SDL_Rect full={0,0,300,300};//pour faire des tests
    int continuer=1;
    int camerax=0;
    int cameray=0;
    int selectedx=-1;
    int selectedy=-1;
    int zoom=100;
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
                        if(full_window.h-event.button.y<100){ //bande grise du bas
                            if(full_window.h-event.button.y<50 && event.button.x<200 && selectedx!=-1){ //buttons en bas a gauche
                                if(event.button.x<50){                        //1er button
                                    if(selectedx==map.width){                     //une ligne est selectionnee->insertion au dessus

                                    }else if(selectedy==map.height){              //une colonne est selectionnee->insertion a gauche

                                    }else{                                        //une case est selectionnee->on remplace par une case vide ou par du bois si elle est deja vide
                                        if(Map_GetAt(map,selectedx,selectedy)==1){
                                            Map_SetAt(map,selectedx,selectedy,0);
                                        }else{
                                            Map_SetAt(map,selectedx,selectedy,1);
                                        }
                                    }
                                }else if(event.button.x<100){                 //2e button
                                    if(selectedx==map.width){                     //une ligne est selectionnee->suppression

                                    }else if(selectedy==map.height){              //une colonne est selectionnee->suppression

                                    }else{                                        //une case est selectionnee->on replace par du bois ou un mirroir si elle c'est du bois ou une case vide
                                        if(Map_GetAt(map,selectedx,selectedy)<2){
                                            Map_SetAt(map,selectedx,selectedy,2);
                                        }else{
                                            Map_SetAt(map,selectedx,selectedy,0);
                                        }
                                    }
                                }else if(event.button.x<150){                 //3e button
                                    if(selectedx==map.width){                     //une ligne est selectionnee->insertion au dessous

                                    }else if(selectedy==map.height){              //une colonne est selectionnee->insertion a droite

                                    }else{                                        //une case est selectionnee->on remplace par la cible ou par un mirroir si c'est la cible ou un joueur
                                        if(Map_GetAt(map,selectedx,selectedy)<4){
                                            Map_SetAt(map,selectedx,selectedy,4);
                                        }else{
                                            Map_SetAt(map,selectedx,selectedy,2);
                                        }
                                    }
                                }else{                                        //4e button
                                    if(selectedx==map.width){                     //une ligne est selectionnee->nada
                                    }else if(selectedy==map.height){              //une colonne est selectionnee->nada
                                    }else{                                        //une case est selectionnee->on remplace par un joueur ou par la cible si elle est deja vide
                                        if(Map_GetAt(map,selectedx,selectedy)<5){
                                            Map_SetAt(map,selectedx,selectedy,5);
                                        }else{
                                            Map_SetAt(map,selectedx,selectedy,4);
                                        }
                                    }
                                }
                            }
                        } else if(full_window.w-event.button.x<100){ //bande gise de droite
                            //todo
                        } else{
                            int x=(event.button.x+camerax)/zoom;
                            int y=(event.button.y+cameray)/zoom;
                            if(event.button.clicks==2){
                                x=map.width;
                            }else if(event.button.clicks==3){
                                y=map.height;
                            }
                            if(x>map.width){
                                x=map.width;
                            }
                            if(y>map.height){
                                y=map.height;
                            }
                            if(x==selectedx && y==selectedy){ //objet deja selectionne -> deselection
                                selectedx=-1;
                                selectedy=-1;
                            } else {
                                selectedx=x;
                                selectedy=y;
                            }
                        }
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    zoom+=event.wheel.y*10;
                    if(zoom<30){
                        zoom=30;
                    }else if(zoom>200){
                        zoom=200;
                    }
            }
        }
        if(map.width*zoom-camerax+100<full_window.w){
            camerax=map.width*zoom-full_window.w+100;
        }
        if(camerax<0){
            camerax=0;
        }
        if(map.height*zoom-cameray+100<full_window.h){
            cameray=map.height*zoom-full_window.h+100;
        }
        if(cameray<0){
            cameray=0;
        }
        SDL_RenderCopy(renderer,texture,&black,&full_window);
        for(int i=0;i<map.height;i++){
            for(int j=0;j<map.width;j++){
                SDL_Rect dest={j*zoom-camerax,i*zoom-cameray,zoom,zoom};
                SDL_RenderCopy(renderer,texture,images+Map_GetAt(map,j,i),&dest);
            }
        }
        SDL_Rect dest;
        if(selectedx<map.width && selectedy<map.height){ // une case est slelctionnee
            dest=(SDL_Rect){selectedx*zoom-camerax,selectedy*zoom-cameray,zoom,zoom};
        } else if(selectedx<map.width && selectedy==map.height){ // une colonne est selectionnee
            dest=(SDL_Rect){selectedx*zoom-camerax,-cameray,zoom,zoom*map.height};
        } else if(selectedx==map.width && selectedy<map.height){ // une ligne est selectionnee
            dest=(SDL_Rect){-camerax,selectedy*zoom-cameray,zoom*map.width,zoom};
        }
        SDL_RenderCopy(renderer,texture,&selected,&dest);
        dest.h=100;
        dest.y=full_window.h-100;
        dest.x=0;
        dest.w=full_window.w;
        SDL_RenderCopy(renderer,texture,&gray,&dest); // bande grise inferieure
        dest.h=full_window.h;
        dest.y=0;
        dest.x=full_window.w-100;
        dest.w=100;
        SDL_RenderCopy(renderer,texture,&gray,&dest); // bande grise droite
        //affichage des boutons
        if(selectedx==-1 && selectedy==-1){
            //on ne fait rien si aucune case n'est selectionnee
        } else if(selectedx==map.width){ // une ligne est selectionnee
            //todo
        } else if(selectedy==map.height){ //une colonne est selectionnee
            //todo
        } else { //une case est selectionnee
            UINT8 value=Map_GetAt(map,selectedx,selectedy);
            dest=(SDL_Rect){0,full_window.h-50,50,50};
            if(value!=1){
                SDL_RenderCopy(renderer,texture,images+1,&dest);
                dest.x+=50;
            }
            if(value!=0){
                SDL_RenderCopy(renderer,texture,images,&dest);
                dest.x+=50;
            }
            if(value!=2 && value!=3){
                SDL_RenderCopy(renderer,texture,images+2,&dest);
                dest.x+=50;
            }
            if(value!=4){
                SDL_RenderCopy(renderer,texture,images+4,&dest);
                dest.x+=50;
            }
            if(value<5){
                SDL_RenderCopy(renderer,texture,images+5,&dest);
            }
        }
        SDL_RenderPresent(renderer);
    }
    return 0;
}
