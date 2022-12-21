let map=document.location.hash.slice(1) || "511116#111111#114111#111211#111111#811117!1";
let default_tile=0;
let mode=0;
let undo_stack=[];
let redo_stack=[];
let unsaved=false;
/***********************************
 *             MODES :             *
 * 0:  Free                        *
 * 1:  Wood                        *
 * 2:  Empty                       *
 * 3:  Mirror /                    *
 * 4:  Mirror \                    *
 * 5:  Cible                       *
 * 6:  Joueur laser g              *
 * 7:  Joueur laser h              *
 * 8:  Joueur laser d              *
 * 9:  Joueur laser b              *
 * 10: Pivoter horraire            *
 * 11: Pivoter trigonométrique     *
 * 12: Supprimer lignes/colones    *
 **********************************/
function load_text(map_text,onFail=function(){
    default_tile=1;
    map=[[5,1,1,1,1,6],[1,1,1,1,1,1],[1,1,4,1,1,1],[1,1,1,2,1,1],[1,1,1,1,1,1],[8,1,1,1,1,7]];
}){
    try{
        default_tile=Number(map_text.split("!")[1]);
        if(default_tile>8 || default_tile<0 || default_tile===NaN){
            throw Error;
        }
        map=map_text.split("!")[0];
        map=map.split("#");
        let line_len=-1;
        map.forEach(line => {
            if(line_len===-1){
                line_len=line.length;
            }else if(line_len!==line.length){
                throw Error;
            }
        });
        for(let i = 0;i<map.length;i++){
            map[i]=map[i].split("");
            for(let j=0;j<map[i].length;j++){
                map[i][j]=Number(map[i][j]);
            }
        }
        map.forEach(line => {
            line.forEach(case_ => {
                if(case_>8 || case_<0 || case_===NaN){
                    throw Error;
                }
            });
        });
        unsaved=false;
    }catch{
        onFail();
    }
}
load_text(map);
function update(){
    document.getElementById("editor").style.width=(map[0].length*100+45)+"px";
    document.getElementById("editor").style.height=(map.length*100+45)+"px";
    document.getElementById("map").innerHTML="";
    for(let i=0;i<map.length;i++){
        for(let j=0;j<map[i].length;j++){
            let case_=document.createElement("div");
            case_.className="case case"+map[i][j];
            case_.style.top=(i*100)+"px";
            case_.style.left=(j*100)+"px";
            document.getElementById("map").appendChild(case_);
        }
    }
    update_menu();
}
update();
let free_mode_coordinates=null;
function ask_free_mode(x,y){
    open_popup();
    document.getElementById("free_mode_popup").style.display="block";
    free_mode_coordinates=[x,y];
}
function clean_cible(){
    for(let i=0;i<map.length;i++){
        for(let j=0;j<map[i].length;j++){
            if(map[i][j]==4) map[i][j]=default_tile;
        }
    }
}
document.getElementById("free_selector").addEventListener("click",click=>{
    let cn=click.srcElement.className;
    let value=Number(cn[cn.length-1]);
    let x=free_mode_coordinates[0];
    let y=free_mode_coordinates[1];
    undo_stack.push(JSON.parse(JSON.stringify(map)));
    redo_stack=[];
    unsaved=true;
    if(value==4) clean_cible();
    map[y][x]=value;
    close_popup();
    document.getElementById("free_mode_popup").removeAttribute("style");
    update();
});
document.getElementById("free_cancel").addEventListener("click",click=>{
    close_popup();
    document.getElementById("free_mode_popup").removeAttribute("style");
});
function apply(case_){
    if(mode>=1 && mode<=9){
        return mode-1;
    }else if(mode===10){
        switch(case_){
            case 2:
            case 3:
                return case_^1;
            case 5:
            case 6:
            case 7:
                return case_+1;
            case 8:
                return 5;
            default:
                return case_;
        }
    }else if(mode===11){
        switch(case_){
            case 2:
            case 3:
                return case_^1;
            case 5:
                return 8;
            case 6:
            case 7:
            case 8:
                return case_-1;
            default:
                return case_;
        }
    }
}
document.getElementById("editor").addEventListener("click",click=>{
    x=click.pageX+document.getElementsByTagName("main")[0].scrollLeft;
    y=click.pageY+document.getElementsByTagName("main")[0].scrollTop-30;
    let modified=false;
    let old_map=JSON.parse(JSON.stringify(map));
    if(x<30 && (x<y || x<15)){
        if(y<15){
            if(mode===5){
                alert("impossible de mettre plusieurs cibles");
            }else for(let i=0;i<map.length;i++){
                for(let j=0;j<map[i].length;j++){
                    switch(mode){
                        case 0:
                        case 12:
                            break;
                        default:
                            modified=true;
                            map[i][j]=apply(map[i][j]);
                            break;
                    }
                }
            }
        }else if((y-15)%100<30){
            modified=true;
            let pos=Math.floor(y/100);
            let new_line=Array(map[0].length);
            new_line.fill(default_tile);
            map=map.slice(0,pos).concat([new_line]).concat(map.slice(pos,map.length));
        }else{
            let pos=Math.floor((y-30)/100);
            if(mode===5){
                alert("impossible de mettre plusieurs cibles");
            }else if(mode===12){
                if(map.length===1){
                    alert("impossible de supprimer la ligne car elle est seule");
                }else{
                    modified=true;
                    map=map.slice(0,pos).concat(map.slice(pos+1,map.length));
                }
            }else for(let j=0;j<map[pos].length;j++){
                switch(mode){
                    case 0:
                        break;
                    default:
                        modified=true;
                        map[pos][j]=apply(map[pos][j]);
                        break;
                }
            }
        }
    }else if(y<30){
        if((x-15)%100<30){
            let pos=Math.floor(x/100);
            modified=true;
            for(let i=0;i<map.length;i++){
                map[i]=map[i].slice(0,pos).concat([default_tile]).concat(map[i].slice(pos,map[i].length));
            }
        }else{
            let pos=Math.floor((x-30)/100);
            if(mode===5){
                alert("impossible de mettre plusieurs cibles");
            }else if(mode===12){
                if(map.length===1){
                    alert("impossible de supprimer la ligne car elle est seule");
                }else for(let i=0;i<map.length;i++){
                    modified=true;
                    map[i]=map[i].slice(0,pos).concat(map[i].slice(pos+1,map[i].length));
                }
            }else for(let i=0;i<map.length;i++){
                switch(mode){
                    case 0:
                        break;
                    default:
                        modified=true;
                        map[i][pos]=apply(map[i][pos]);
                        break;
                }
            }
        }
    }else{
        y=Math.floor((y-30)/100);
        x=Math.floor((x-30)/100);
        if(y>=map.length || x>=map[0].length){
            return;
        }
        switch(mode){
            case 0:
                ask_free_mode(x,y);
                break;
            case 5:
                clean_cible();
            default:
                modified=true;
                map[y][x]=apply(map[y][x]);
                break;
        }
    }
    if(modified){
        undo_stack.push(old_map);
        redo_stack=[];
        unsaved=true;
    }
    update();
})
function undo(){
    if(undo_stack.length==0){
        return;
    }
    redo_stack.push(map);
    map=undo_stack.pop();
    update();
}
function redo(){
    if(redo_stack.length==0){
        return;
    }
    undo_stack.push(map);
    map=redo_stack.pop();
    update();
}
document.addEventListener("keydown",event => {
    if(event.key=="Escape"){
        close_menu();
        if(freezeClick){
            close_popup();
            Array.from(document.getElementById("popup").children).forEach(popup=>{
                popup.removeAttribute("style");
            })
        }
    }else if(!freezeClick && event.ctrlKey && !event.shiftKey && !event.altKey){
        switch(event.key){
            case 'e':
                event.preventDefault();
                document.getElementById("export").click();
                break;
            case 'i':
                event.preventDefault();
                document.getElementById("import").click();
                break;
            case 'j':
                event.preventDefault();
                document.getElementById("play").click();
                break;
            case 'o':
                event.preventDefault();
                document.getElementById("open").click();
                break;
            case 's':
                event.preventDefault();
                document.getElementById("save").click();
                break;
            case 'u':
                event.preventDefault();
                document.getElementById("url_export").click();
            case 'y':
                redo();
                break;
            case 'z':
                undo();
                break;
        }
    }
})
window.onbeforeunload=function(){
    if(unsaved){
        return "Votre map n'est pas enregistrée, voulez-vous vraiement quitter?"
    }
}