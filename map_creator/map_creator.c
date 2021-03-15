#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "map.h"
#include "file_dialog.h"
char* pixels="###360000a###";
SDL_Rect full_window;
void display_img_no(SDL_Renderer* renderer,SDL_Texture* texture,uint8_t no,SDL_Rect dest){
    static SDL_Rect images[]={{0,0,100,100},
                    {200,0,1,1},
                    {200,0,100,100},
                    {100,100,100,100},
                    {200,100,100,100}
    };
    if(no<=2){
        SDL_RenderCopy(renderer,texture,images+no,&dest);
    }else if(no==3){
        SDL_RenderCopyEx(renderer,texture,images+2,&dest,0.0,NULL,SDL_FLIP_HORIZONTAL);
    }else if(no<=5){
        SDL_RenderCopy(renderer,texture,images+no-1,&dest);
    }else{
        SDL_RenderCopyEx(renderer,texture,images+4,&dest,90.0*(no-5),NULL,SDL_FLIP_NONE);
    }
}
void button(uint8_t no,Map *map,int* selectedx,int* selectedy){
    if(*selectedx==-1) return;
    switch(no){
        case 1:
            if(*selectedx==map->width){                     //une ligne est selectionnee->insertion au dessus
                Map_insert_line(map,*selectedy);
                (*selectedy)++;
            }else if(*selectedy==map->height){              //une colonne est selectionnee->insertion a gauche
                Map_insert_col(map,*selectedx);
                (*selectedx)++;
            }else{                                        //une case est selectionnee->on remplace par une case vide ou par du bois si elle est deja vide
                if(Map_GetAt(*map,*selectedx,*selectedy)==1){
                    Map_SetAt(*map,*selectedx,*selectedy,0);
                }else{
                    Map_SetAt(*map,*selectedx,*selectedy,1);
                }
            }
            break;
        case 2:
            if(*selectedx==map->width){                     //une ligne est selectionnee->suppression
                Map_delete_line(map,*selectedy);
                *selectedx=-1;
                *selectedy=-1;
            }else if(*selectedy==map->height){              //une colonne est selectionnee->suppression
                Map_delete_col(map,*selectedx);
                *selectedx=-1;
                *selectedy=-1;
            }else{                                        //une case est selectionnee->on replace par du bois ou un mirroir si elle c'est du bois ou une case vide
                if(Map_GetAt(*map,*selectedx,*selectedy)<2){
                    Map_SetAt(*map,*selectedx,*selectedy,2);
                }else{
                    Map_SetAt(*map,*selectedx,*selectedy,0);
                }
            }
            break;
        case 3:
            if(*selectedx==map->width){                     //une ligne est selectionnee->insertion au dessous
                Map_insert_line(map,*selectedy+1);
            }else if(*selectedy==map->height){              //une colonne est selectionnee->insertion a droite
                Map_insert_col(map,*selectedx+1);
            }else{                                        //une case est selectionnee->on remplace par la cible ou par un mirroir si c'est la cible ou un joueur
                if(Map_GetAt(*map,*selectedx,*selectedy)<4){
                    Map_SetAt(*map,*selectedx,*selectedy,4);
                }else{
                    Map_SetAt(*map,*selectedx,*selectedy,2);
                }
            }
            break;
        case 4:
            if(*selectedx<map->width && *selectedy<map->height){   //une ligne ou une colonne est selectionnee->nada
                if(Map_GetAt(*map,*selectedx,*selectedy)<5){        //une case est selectionnee->on remplace par un joueur ou par la cible si c'est deja un joueur
                    Map_SetAt(*map,*selectedx,*selectedy,5);
                }else{
                    Map_SetAt(*map,*selectedx,*selectedy,4);
                }
            }
            break;
        case 8:
        case 10:
            if(*selectedx<map->width
                    && *selectedy<map->height
                    && *selectedx != -1){  //une case est selectionnee
                uint8_t sens=no&2;
                switch(Map_GetAt(*map,*selectedx,*selectedy)){
                    case 2:
                        Map_SetAt(*map,*selectedx,*selectedy,3);
                        break;
                    case 3:
                        Map_SetAt(*map,*selectedx,*selectedy,2);
                        break;
                    case 5:
                        Map_SetAt(*map,*selectedx,*selectedy,6+sens);
                        break;
                    case 6:
                        Map_SetAt(*map,*selectedx,*selectedy,7^sens);
                        break;
                    case 7:
                        Map_SetAt(*map,*selectedx,*selectedy,8-sens);
                        break;
                    case 8:
                        Map_SetAt(*map,*selectedx,*selectedy,5^sens);
                        break;
                }
            }
            break;
    }
}
int main(int argc,char* argv[]){
    if(SDL_Init(SDL_INIT_VIDEO)==-1){
        SDL_Quit();
        return -1;
    }
    SDL_Window* window=SDL_CreateWindow("Lasers map creator",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1000,700,SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window,300,300);
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface* surface=SDL_CreateRGBSurface(0,300,300,32,0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
    surface->pixels=pixels;
    SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,surface);
    char* filename=NULL;
    SDL_Rect selected={100,0,1,1};
    SDL_Rect black={100,100,1,1};
    SDL_Rect gray={100,1,1,1};
    SDL_Rect red={131,0,1,1};
    SDL_Rect insert={101,0,30,30};
    SDL_Rect delete_={131,0,30,30};
    full_window=(SDL_Rect){0,0,1000,700};
    SDL_Rect rot={161,0,30,30};
    SDL_Rect rotbg={161,0,1,1};
    SDL_Rect chiffre1={100,30,15,30};
    SDL_Rect chiffre2={115,30,15,30};
    SDL_Rect chiffre3={130,30,15,30};
    SDL_Rect chiffre4={145,30,15,30};
    SDL_Rect lettreF={160,30,15,30};
    int continuer=1;
    int camerax=0;
    int cameray=0;
    int selectedx=-1;
    int selectedy=-1;
    int zoom=100;
    Map map;
    if(argc>1){
        filename=argv[1];
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
        map=Map_Create(map_txt);
    } else {
        map=Map_Create("511116#111111#114111#111211#111111#811117!1");
    }
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
                            if(full_window.h-event.button.y<50
                               && event.button.x<200
                               && selectedx!=-1){ //buttons en bas a gauche
                                if(event.button.x<50){                        //1er button
                                    button(1,&map,&selectedx,&selectedy);
                                }else if(event.button.x<100){                 //2e button
                                    button(2,&map,&selectedx,&selectedy);
                                }else if(event.button.x<150){                 //3e button
                                    button(3,&map,&selectedx,&selectedy);
                                }else{                                        //4e button
                                    button(4,&map,&selectedx,&selectedy);
                                }
                            } else if(full_window.h-event.button.y<100
                                      && event.button.x>=50
                                      && event.button.x<150){ //boutons de rotation
                                if(event.button.x<100){ //rotation a gauche
                                    button(10,&map,&selectedx,&selectedy);
                                }else{
                                    button(8,&map,&selectedx,&selectedy);
                                }
                            }
                        } else if(full_window.w-event.button.x<100){ //bande gise de droite
                            if(event.button.y<100){
                                map.default_++;
                                map.default_&=3;
                            }
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
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_s: //enregistrer
                            if(filename==NULL){
                                goto SaveAs;
                            }
                        Saving:;
                            char* map_txt=Map_export(map);
                            FILE* map_file=fopen(filename,"w");
                            for(int i=0;map_txt[i];i++){
                                fwrite(map_txt+i,1,1,map_file);
                            }
                            fclose(map_file);
                            break;
                        SaveAs:
                        case SDLK_F12:; //enregistrer sous
                            char* new_filename;
                            char* dir=NULL;
                            if(filename){
                                int dir_len=0;
                                for(;filename[dir_len];dir_len++);
                                for(;filename[dir_len]!='\\';dir_len--);
                                dir=malloc(dir_len+1);
                                for(int i=0;i<dir_len;i++){
                                    dir[i]=filename[i];
                                }
                                dir[dir_len]=0;
                            }
                            new_filename=AskSaveFileName("Lasers maps (*.lama)\0*.lama\0All files (*.*)\0*.*\0\0",
                                                         256,dir,"Enregistrer sous...");
                            if(new_filename[0]){
                                filename=new_filename;
                                goto Saving;
                            }
                            break;
                        case SDLK_1:
                            button(1,&map,&selectedx,&selectedy);
                            break;
                        case SDLK_2:
                            button(2,&map,&selectedx,&selectedy);
                            break;
                        case SDLK_3:
                            button(3,&map,&selectedx,&selectedy);
                            break;
                        case SDLK_4:
                            button(4,&map,&selectedx,&selectedy);
                            break;
                        case SDLK_F2:
                            button(10,&map,&selectedx,&selectedy);
                            break;
                        case SDLK_F3:
                            button(8,&map,&selectedx,&selectedy);
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
                display_img_no(renderer,texture,Map_GetAt(map,j,i),dest);
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
        if(!(selectedx==-1 && selectedy==-1)){ //on ne fait rien si aucune case n'est selectionnee
            uint8_t case_selected=0;
            if(selectedx==map.width){ // une ligne est selectionnee
                dest=(SDL_Rect){0,full_window.h-50,50,50};
                SDL_RenderCopy(renderer,texture,&selected,&dest);
                dest.x=50;
                SDL_RenderCopy(renderer,texture,&red,&dest);
                dest.x=100;
                SDL_RenderCopy(renderer,texture,&selected,&dest);
                dest=(SDL_Rect){10,full_window.h-40,30,30};
                SDL_RenderCopy(renderer,texture,&insert,&dest);
                dest.x=60;
                SDL_RenderCopy(renderer,texture,&delete_,&dest);
                dest.x=110;
                SDL_RenderCopyEx(renderer,texture,&insert,&dest,0.0,NULL,SDL_FLIP_VERTICAL);
            } else if(selectedy==map.height){ //une colonne est selectionnee
                dest=(SDL_Rect){0,full_window.h-50,50,50};
                SDL_RenderCopy(renderer,texture,&selected,&dest);
                dest.x=50;
                SDL_RenderCopy(renderer,texture,&red,&dest);
                dest.x=100;
                SDL_RenderCopy(renderer,texture,&selected,&dest);
                dest=(SDL_Rect){10,full_window.h-40,30,30};
                SDL_RenderCopyEx(renderer,texture,&insert,&dest,90.0,NULL,SDL_FLIP_VERTICAL);
                dest.x=60;
                SDL_RenderCopy(renderer,texture,&delete_,&dest);
                dest.x=110;
                SDL_RenderCopyEx(renderer,texture,&insert,&dest,90.0,NULL,SDL_FLIP_NONE);
            } else { //une case est selectionnee
                uint8_t value=Map_GetAt(map,selectedx,selectedy);
                dest=(SDL_Rect){0,full_window.h-50,50,50};
                if(value!=1){
                    display_img_no(renderer,texture,1,dest);
                    dest.x+=50;
                }
                if(value!=0){
                    display_img_no(renderer,texture,0,dest);
                    dest.x+=50;
                }
                if(value!=2 && value!=3){
                    display_img_no(renderer,texture,2,dest);
                    dest.x+=50;
                }
                if(value!=4){
                    display_img_no(renderer,texture,4,dest);
                    dest.x+=50;
                }
                if(value<5){
                    display_img_no(renderer,texture,5,dest);
                }
                if(value>1 && value!=4){ //boutons de rotation
                    dest.x=50;
                    dest.y=full_window.h-100;
                    SDL_RenderCopy(renderer,texture,&rotbg,&dest);
                    dest.x=60;
                    dest.y+=10;
                    dest.w=dest.h=30;
                    SDL_RenderCopy(renderer,texture,&rot,&dest);
                    dest.x=100;
                    dest.y-=10;
                    dest.w=dest.h=50;
                    SDL_RenderCopy(renderer,texture,&rotbg,&dest);
                    dest.x=110;
                    dest.y+=10;
                    dest.w=dest.h=30;
                    SDL_RenderCopyEx(renderer,texture,&rot,&dest,0.0,NULL,SDL_FLIP_HORIZONTAL);
                    dest=(SDL_Rect){60,full_window.h-80,15,30};
                    SDL_RenderCopy(renderer,texture,&lettreF,&dest);
                    dest.x=75;
                    SDL_RenderCopy(renderer,texture,&chiffre2,&dest);
                    dest.x=110;
                    SDL_RenderCopy(renderer,texture,&lettreF,&dest);
                    dest.x=125;
                    SDL_RenderCopy(renderer,texture,&chiffre3,&dest);
                }
                case_selected=1;
            }
            dest=(SDL_Rect){17,full_window.h-30,15,30};
            SDL_RenderCopy(renderer,texture,&chiffre1,&dest);
            dest.x=67;
            SDL_RenderCopy(renderer,texture,&chiffre2,&dest);
            dest.x=117;
            SDL_RenderCopy(renderer,texture,&chiffre3,&dest);
            if(case_selected){
                dest.x=167;
                SDL_RenderCopy(renderer,texture,&chiffre4,&dest);
            }
        }
        // bande de droite
        dest=(SDL_Rect){full_window.w-100,0,100,100};
        display_img_no(renderer,texture,map.default_,dest);
        SDL_RenderPresent(renderer);
    }
    return 0;
}
