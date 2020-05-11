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
void Map_SetAt(Map map,int x,int y,UINT8 value){
    if(x>=map.width || y>=map.height || (value>8)){
        return;
    }
    UINT8* value_to_change=(map.data+((map.width+1)>>1)*y+(x>>1));
    UINT8 mask=0xf<<((x&1)<<2);
    value=value<<(((x^1)&1)<<2);
    *value_to_change=((*value_to_change&mask)|value);
    return;
}
void Map_delete(Map map){
    free(map.data);
}
void Map_insert_col(Map *map,int x){
    if(map->width==255){
        return;
    }
    map->width++;
    if(map->width&1){
        UINT8* new_data=malloc(((map->width+1)>>1)*map->height);
        UINT8* old_data=map->data;
        map->data=new_data;
        for(int i=0;i<map->height;i++){
            for(int j=0;j<map->width;j++){
                if(j>x){
                    map->data=old_data;
                    map->width--;
                    UINT8 value=Map_GetAt(*map,j-1,i);
                    map->data=new_data;
                    map->width++;
                    Map_SetAt(*map,j,i,value);
                } else if(j==x){
                    Map_SetAt(*map,j,i,map->default_);
                } else {
                    map->data=old_data;
                    map->width--;
                    UINT8 value=Map_GetAt(*map,j,i);
                    map->data=new_data;
                    map->width++;
                    Map_SetAt(*map,j,i,value);
                }
            }
        }
        free(old_data);
    } else {
        for(int i=0;i<map->height;i++){
            for(int j=map->width-1;j>x;j--){
                Map_SetAt(*map,j,i,Map_GetAt(*map,j-1,i));
            }
            Map_SetAt(*map,x,i,map->default_);
        }
    }
}
void Map_delete_col(Map *map,int x){
    if(map->width==1){
        return;
    }
    map->width--;
    if(!(map->width&1)){
        UINT8* new_data=malloc(((map->width+1)>>1)*map->height);
        UINT8* old_data=map->data;
        map->data=new_data;
        for(int i=0;i<map->height;i++){
            for(int j=0;j<map->width;j++){
                if(j>=x){
                    map->data=old_data;
                    map->width++;
                    UINT8 value=Map_GetAt(*map,j+1,i);
                    map->data=new_data;
                    map->width--;
                    Map_SetAt(*map,j,i,value);
                } else {
                    map->data=old_data;
                    map->width++;
                    UINT8 value=Map_GetAt(*map,j,i);
                    map->data=new_data;
                    map->width--;
                    Map_SetAt(*map,j,i,value);
                }
            }
        }
        free(old_data);
    } else {
        map->width++;
        for(int i=0;i<map->height;i++){
            for(int j=x;j<map->width;j++){
                Map_SetAt(*map,j,i,Map_GetAt(*map,j+1,i));
            }
        }
        map->width--;
    }
}
void Map_insert_line(Map *map,int y){
    if(map->height==255){
        return;
    }
    map->height++;
    int bwidth=(map->width+1)>>1;
    UINT8*new_data=malloc(bwidth*map->height);
    UINT8*old_data=map->data;
    for(int i=0;i<y*bwidth;i++){
        new_data[i]=old_data[i];
    }
    for(int i=0;i<bwidth;i++){
        new_data[i+bwidth*y]=0x11*map->default_;
    }
    for(int i=0;i<(map->height-y-1)*bwidth;i++){
        new_data[i+bwidth*(y+1)]=old_data[i+bwidth*y];
    }
    map->data=new_data;
    free(old_data);
}
void Map_delete_line(Map *map,int y){
    if(map->height==1){
        return;
    }
    map->height--;
    int bwidth=(map->width+1)>>1;
    UINT8*new_data=malloc(bwidth*map->height);
    UINT8*old_data=map->data;
    for(int i=0;i<y*bwidth;i++){
        new_data[i]=old_data[i];
    }
    for(int i=0;i<(map->height-y)*bwidth;i++){
        new_data[i+y*bwidth]=old_data[i+(y+1)*bwidth];
    }
    map->data=new_data;
    free(old_data);
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
    result.data=malloc(((result.width+1)>>1)*result.height);
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
char* Map_export(Map map){
    char* result=malloc((map.width+1)*map.height+2);
    for(int y=0;y<map.height;y++){
        for(int x=0;x<map.width;x++){
            result[x+y*(map.width+1)]=0x30|Map_GetAt(map,x,y);
        }
        result[(y+1)*(map.width+1)-1]='#';
    }
    result[(map.width+1)*map.height-1]='!';
    result[(map.width+1)*map.height]=0x30|map.default_;
    result[(map.width+1)*map.height+1]=0;
    return result;
}
