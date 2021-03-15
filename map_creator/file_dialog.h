#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

void fd_init(void){

}
#ifdef _WIN32
void make_ofn(char* types,int maxLenght,char* initial_dir,char* window_title,LPOPENFILENAMEA ofn){
    ofn->lStructSize=sizeof(*ofn);
    ofn->hwndOwner=NULL;
    ofn->hInstance=NULL;
    ofn->lpstrFilter=types;
    ofn->lpstrCustomFilter=NULL;
    ofn->nMaxCustFilter=0;
    ofn->nFilterIndex=1;
    ofn->lpstrFile=malloc(maxLenght);
    ofn->nMaxFile=maxLenght;
    ofn->lpstrFileTitle=NULL;
    ofn->nMaxFileTitle=0;
    ofn->lpstrInitialDir=initial_dir;
    ofn->lpstrTitle=window_title;
    ofn->Flags=0;
    ofn->nFileOffset=0;
    ofn->nFileExtension=0;
    ofn->lpstrDefExt="lama";
    ofn->lCustData=0;
    ofn->lpfnHook=NULL;
    ofn->lpTemplateName=NULL;
    ofn->pvReserved=NULL;
    ofn->dwReserved=0;
    ofn->FlagsEx=0;
    for(int i=0;i<maxLenght;i++){
        ofn->lpstrFile[i]=0;
    }
}
#endif // _WIN32
char* AskOpenFileName(char* types,int maxLenght,char* initial_dir,char* window_title){
    #ifdef _WIN32
    struct tagOFNA ofn;
    make_ofn(types,maxLenght,initial_dir,window_title,&ofn);
    GetOpenFileNameA(&ofn);
    return ofn.lpstrFile;
    #endif // _WIN32
}
char* AskSaveFileName(char* types,int maxLenght,char* initial_dir,char* window_title){
    #ifdef _WIN32
    struct tagOFNA ofn;
    make_ofn(types,maxLenght,initial_dir,window_title,&ofn);
    GetSaveFileNameA(&ofn);
    return ofn.lpstrFile;
    #endif // _WIN32
}
