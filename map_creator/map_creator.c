#include <SDL2/SDL.h>
#include <sfd/sfd.h>
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char UINT8;
typedef unsigned int UINT32;
#include "map.h"
char* pixels="###360000a###";
SDL_Rect images[]={{0,0,100,100},
                   {200,0,1,1},
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
    char* filename=NULL;
    SDL_Rect selected={100,0,1,1};
    SDL_Rect black={100,100,1,1};
    SDL_Rect gray={100,1,1,1};
    SDL_Rect red={130,30,1,1};
    SDL_Rect insertL={101,0,30,30};
    SDL_Rect insertR={131,0,30,30};
    SDL_Rect insertT={161,0,30,30};
    SDL_Rect insertB={100,30,30,30};
    SDL_Rect delete_={130,30,30,30};
    SDL_Rect full_window={0,0,600,480};
    int continuer=1;
    int camerax=0;
    int cameray=0;
    int selectedx=-1;
    int selectedy=-1;
    int zoom=100;
    Map map=Map_Create("506#243#718!1");
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
                                        Map_insert_line(&map,selectedy);
                                    }else if(selectedy==map.height){              //une colonne est selectionnee->insertion a gauche
                                        Map_insert_col(&map,selectedx);
                                    }else{                                        //une case est selectionnee->on remplace par une case vide ou par du bois si elle est deja vide
                                        if(Map_GetAt(map,selectedx,selectedy)==1){
                                            Map_SetAt(map,selectedx,selectedy,0);
                                        }else{
                                            Map_SetAt(map,selectedx,selectedy,1);
                                        }
                                    }
                                }else if(event.button.x<100){                 //2e button
                                    if(selectedx==map.width){                     //une ligne est selectionnee->suppression
                                        Map_delete_line(&map,selectedy);
                                        selectedx=-1;
                                        selectedy=-1;
                                    }else if(selectedy==map.height){              //une colonne est selectionnee->suppression
                                        Map_delete_col(&map,selectedx);
                                        selectedx=-1;
                                        selectedy=-1;
                                    }else{                                        //une case est selectionnee->on replace par du bois ou un mirroir si elle c'est du bois ou une case vide
                                        if(Map_GetAt(map,selectedx,selectedy)<2){
                                            Map_SetAt(map,selectedx,selectedy,2);
                                        }else{
                                            Map_SetAt(map,selectedx,selectedy,0);
                                        }
                                    }
                                }else if(event.button.x<150){                 //3e button
                                    if(selectedx==map.width){                     //une ligne est selectionnee->insertion au dessous
                                        Map_insert_line(&map,selectedy+1);
                                    }else if(selectedy==map.height){              //une colonne est selectionnee->insertion a droite
                                        Map_insert_col(&map,selectedx+1);
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
                    break;
                case SDL_KEYDOWN:;
                    sfd_Options opt;
                    switch(event.key.keysym.sym){
                        case SDLK_o: //ouvrir un fichier
                            opt=(sfd_Options){
                                .title       = "Open map",
                                .filter_name = "Text File",
                                .filter      = "*.txt"
                            };
                            char* new_filename=sfd_open_dialog(&opt);
                            if(new_filename){
                                filename=new_filename;
                                FILE* map_file = fopen(filename,"r");
                                int length=0;
                                while(!feof(map_file)){
                                    char c;
                                    fread(&c,1,1,map_file);
                                    length++;
                                }
                                fseek(map_file,0,SEEK_SET);
                                char* map_txt=malloc(length+1);
                                map_txt[length]=0;
                                fread(map_txt,1,length,map_file);
                                fclose(map_file);
                                Map new_map=Map_Create(map_txt);
                                if(new_map.data!=NULL){
                                    Map_delete(map);
                                    map=new_map;
                                }
                            }
                            break;
                        case SDLK_s: //enregistrer
                            if(filename==NULL){
                                opt=(sfd_Options){
                                    .title       = "Save map",
                                    .filter_name = "Text File",
                                    .filter      ="*.txt"
                                };
                                filename=sfd_save_dialog(&opt);
                                if(filename==NULL){
                                    break;
                                }
                            }
                        Saving:
                            printf("saving to %s\n",filename);
                            char* map_txt=Map_export(map);
                            printf("file data is %s\n",map_txt);
                            FILE* map_file=fopen(filename,"w");
                            for(int i=0;map_txt[i];i++){
                                fwrite(map_txt+i,1,1,map_file);
                            }
                            fclose(map_file);
                            break;
                        case SDLK_F12:; //enregistrer sous
                            opt=(sfd_Options){
                                .title       = "Save map as",
                                .filter_name = "Text File",
                                .filter      = "*.txt"
                            };
                            new_filename=sfd_save_dialog(&opt);
                            if(new_filename){
                                filename=new_filename;
                                goto Saving;
                            }
                            break;
                    }
                    break;
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
            dest=(SDL_Rect){0,full_window.h-50,50,50};
            SDL_RenderCopy(renderer,texture,&selected,&dest);
            dest.x=50;
            SDL_RenderCopy(renderer,texture,&red,&dest);
            dest.x=100;
            SDL_RenderCopy(renderer,texture,&selected,&dest);
            dest=(SDL_Rect){10,full_window.h-40,30,30};
            SDL_RenderCopy(renderer,texture,&insertT,&dest);
            dest.x=60;
            SDL_RenderCopy(renderer,texture,&delete_,&dest);
            dest.x=110;
            SDL_RenderCopy(renderer,texture,&insertB,&dest);
        } else if(selectedy==map.height){ //une colonne est selectionnee
            dest=(SDL_Rect){0,full_window.h-50,50,50};
            SDL_RenderCopy(renderer,texture,&selected,&dest);
            dest.x=50;
            SDL_RenderCopy(renderer,texture,&red,&dest);
            dest.x=100;
            SDL_RenderCopy(renderer,texture,&selected,&dest);
            dest=(SDL_Rect){10,full_window.h-40,30,30};
            SDL_RenderCopy(renderer,texture,&insertL,&dest);
            dest.x=60;
            SDL_RenderCopy(renderer,texture,&delete_,&dest);
            dest.x=110;
            SDL_RenderCopy(renderer,texture,&insertR,&dest);
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
