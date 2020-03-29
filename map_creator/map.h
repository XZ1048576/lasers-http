typedef struct{
    UINT8 width,height, *data,default_;
} Map;
UINT8 Map_GetAt(Map map,int x,int y){
    if(x>map.width || y>map.height){
        return 9;
    }
    UINT8 result=*(map.data+((map.width+1)>>1)*y+(x>>1));
    UINT8 mask=0xf0>>((x&1)<<2);
    return (result&mask)>>(((x^1)&1)<<2);
}
UINT8 Map_SetAt(Map map,int x,int y,UINT8 value){
    if(x>=map.width || y>=map.height || (value>8)){
        return 9;
    }
    UINT8* value_to_change=(map.data+((map.width+1)>>1)*y+(x>>1));
    UINT8 mask=0xf<<((x&1)<<2);
    value=value<<(((x^1)&1)<<2);
    *value_to_change=((*value_to_change&mask)|value);
    return 0;
}
Map Map_Create(char* map){
    Map result={0,0,NULL,0};
    UINT32 nb=0;
    for(int i=0;map[i];i++){
        if(map[i]=='!'){
            nb++;
        }
    }
    if(nb!=1){
        printf("! %d\n",nb);
        return (Map){0,0,NULL,0};
    }
    nb=0;
    for(int i=0;map[i];i++){
        if(nb&0x1000){
            nb++;
            nb+=(map[i])<<16;
        } else if(map[i]=='!'){
            nb=0x1000;
        }
    }
    if((nb&0xfffcffff)!=0x301001){
        printf("after ! %x\n",nb);
        return (Map){0,0,NULL,0};
    }
    result.default_=(nb&0x30000)>>16;
    for(int i=0;map[i]!='!';i++){
        if((map[i]&0xf8)!=0x30 && map[i]!='8' && map[i]!='#'){
            printf("invalid char in map %c\n",map[i]);
            return (Map){0,0,NULL,0};
        }
    }
    nb=0;
    for(int i=0;map[i]!='!';i++){
        if(map[i]=='#'){
            if(nb&0x3ff00000){
                if((nb&0xffc00)>>10!=(nb&0x3ff)){
                    printf("lines not same lenght %x\n",nb);
                    return (Map){0,0,NULL,0};
                } else {
                    nb=nb&0xbffffc00;
                }
            } else {
                nb=nb<<10;
            }
            nb+=0x100000;
        } else {
            nb++;
        }
    }
    if(nb&0x3ff00000){
        if((nb&0xffc00)>>10!=(nb&0x3ff)){
            printf("lines not same lenght %x\n",nb);
            return (Map){0,0,NULL,0};
        } else {
            nb=(nb&0x3ffffc00)>>10;
        }
    }
    nb+=0x400;
    result.width=nb&0x3ff;
    result.height=(nb&0xffc00)>>10;
    result.data=malloc(((result.height+1)>1)*result.height);
    for(int i=0,j=0,k=0;map[i]!='!';i++){
        if(map[i]=='#'){
            j=0;
            k++;
        } else {
            Map_SetAt(result,j,k,map[i]&0xf);
            j++;
        }
    }
    return result;
}
