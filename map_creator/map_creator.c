#include <SDL2/SDL.h>
# ifdef _WIN32
#include <windows.h>
# endif // _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
SDL_Rect full_window;
SDL_Renderer* renderer;
int main(int argc,char* argv[]){
    HWND console_handle=GetConsoleWindow();
    ShowWindow(console_handle,0);
    # ifdef _WIN32
    chdir(getenv("USERPROFILE"));
    chdir("Documents");
    #else
    chdir("~/Documents");
    # endif // _WIN32
    if(chdir("Lasers_maps")){
        mkdir("Lasers_maps");
        chdir("Lasers_maps");
    }
    if(SDL_Init(SDL_INIT_VIDEO)==-1){
        SDL_Quit();
        return -1;
    }
    SDL_Window* window=SDL_CreateWindow("Lasers map creator",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,600,480,SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window,300,300);
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
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
    full_window=(SDL_Rect){0,0,600,480};
    SDL_Rect rotl={160,30,30,30};
    SDL_Rect rotr={100,60,30,30};
    SDL_Rect rotbg={160,30,1,1};
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
        map=Map_Create("1111111#1111111#1111111#1114111#1111111#1111111#1111111!1");
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
                            } else if(full_window.h-event.button.y<100 && event.button.x>=50 && event.button.x<150){ //boutons de rotation
                                if(selectedx<map.width && selectedy<map.height && selectedx != -1){
                                    UINT8 sens=0;
                                    if(event.button.x<100){ //rotation a gauche
                                        sens=2;
                                    }
                                switch(Map_GetAt(map,selectedx,selectedy)){
                                    case 2:
                                        Map_SetAt(map,selectedx,selectedy,3);
                                        break;
                                    case 3:
                                        Map_SetAt(map,selectedx,selectedy,2);
                                        break;
                                    case 5:
                                        Map_SetAt(map,selectedx,selectedy,6+sens);
                                        break;
                                    case 6:
                                        Map_SetAt(map,selectedx,selectedy,7^sens);
                                        break;
                                    case 7:
                                        Map_SetAt(map,selectedx,selectedy,8-sens);
                                        break;
                                    case 8:
                                        Map_SetAt(map,selectedx,selectedy,5^sens);
                                        break;
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
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_s: //enregistrer
                            if(filename==NULL){
                                goto AskSaveFileName;
                            }
                        Saving:;
                            char* map_txt=Map_export(map);
                            FILE* map_file=fopen(filename,"w");
                            for(int i=0;map_txt[i];i++){
                                fwrite(map_txt+i,1,1,map_file);
                            }
                            fclose(map_file);
                            break;
                        AskSaveFileName:
                        case SDLK_F12:; //enregistrer sous
                            char* new_filename;
                            ShowWindow(console_handle,5);
                            printf("Enterez un nom pour cette map:\n\n");
                            new_filename=malloc(50);
                            gets(new_filename);
                            int i=0;
                            for(;new_filename[i];i++);
                            strcpy(new_filename+i,".txt");
                            system("cls");
                            ShowWindow(console_handle,0);
                            if(new_filename[0]){
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
            if(value>1 && value!=4){ //boutons de rotation
                dest.x=50;
                dest.y=full_window.h-100;
                SDL_RenderCopy(renderer,texture,&rotbg,&dest);
                dest.x=60;
                dest.y+=10;
                dest.w=dest.h=30;
                SDL_RenderCopy(renderer,texture,&rotl,&dest);
                dest.x=100;
                dest.y-=10;
                dest.w=dest.h=50;
                SDL_RenderCopy(renderer,texture,&rotbg,&dest);
                dest.x=110;
                dest.y+=10;
                dest.w=dest.h=30;
                SDL_RenderCopy(renderer,texture,&rotr,&dest);
            }
        }
        SDL_RenderPresent(renderer);
    }
    return 0;
}
