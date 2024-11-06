let freezeClick=false;
function handler(e){
    if(freezeClick && !document.getElementById("popup").contains(e.target)){
        e.stopPropagation();
        e.preventDefault();
    }
}
document.addEventListener("click",handler,true);
document.addEventListener("pointerup",handler,true);
document.addEventListener("pointerdown",handler,true);
function export_text(){
    let map_text="";
    map.forEach(line => {
        line.forEach(case_ => {
            map_text+=case_;
        });
        map_text+="#";
    });
    return map_text.slice(0,-1)+"!"+default_tile;
}
function update_menu(){
    document.getElementById("save").href="data:text/plain;base64,"+btoa(export_text());
    document.getElementById("play").href=document.location.origin+"/#"+export_text();
}
function open_popup(){
    document.getElementById("popup").style.display="block";
    document.getElementById("popup").style.top=(document.body.clientHeight/2-150)+"px";
    document.getElementById("popup").style.left=(document.body.clientWidth/2-225)+"px";
    freezeClick=true;
}
function close_popup(){
    document.getElementById("popup").style.display="none";
    freezeClick=false;
}
Array.from(document.getElementsByClassName("menu")).forEach(menu => {
    menu.onpointerdown=function(){
        Array.from(document.getElementsByClassName("menu_deroulant")).forEach(menu_deroulant => {
            if(menu_deroulant.parentElement===menu){
                menu_deroulant.style.display="block";
            }else{
                menu_deroulant.removeAttribute("style");
            }
        });
    };
});
function close_menu(){
    Array.from(document.getElementsByClassName("menu_deroulant")).forEach(menu_deroulant => {
        menu_deroulant.removeAttribute("style");
    })
}
close_menu();
document.getElementById("editor").addEventListener("click",close_menu);
Array.from(document.getElementsByClassName("menu_deroulant")).forEach(menu_deroulant => {
    close_menu();
    Array.from(menu_deroulant.children).forEach(button => {
        if(button.tagName==="BUTTON"){
            button.onpointerup=function(){
                this.parentElement.removeAttribute("style");
            }
        }
    });
});

/************************ File menu buttons ************************/

document.getElementById("new").addEventListener("click",function(){
    undo_stack.push(JSON.parse(JSON.stringify(map)));
    redo_stack=[];
    unsaved=false;
    default_tile=1;
    map=[[5,1,1,1,1,6],[1,1,1,1,1,1],[1,1,4,1,1,1],[1,1,1,2,1,1],[1,1,1,1,1,1],[8,1,1,1,1,7]];
    update();
});
document.getElementById("save").addEventListener("click",function(){
    unsaved=false;
})
document.getElementById("open").addEventListener("click",function(){
    document.getElementById("open_input").click();
})
document.getElementById("open_input").addEventListener("change",function(){
    var reader=new FileReader();
    reader.onload=function(e){
        let modified=true;
        load_text(e.target.result,function(){
            alert("La map est invalide");
            modified=false;
        });
        update();
        if(modified){
            undo_stack.push(JSON.parse(JSON.stringify(map)));
            redo_stack=[];
            unsaved=false;
        }
    }
    reader.readAsText(document.getElementById("open_input").files[0]);
})
document.getElementById("export").addEventListener("click",function(){
    open_popup();
    document.getElementById("export_popup").style.display="block";
    document.getElementById("export_textarea").value=export_text();
})
if(window.isSecureContext){
    document.getElementById("export_copy_button").addEventListener("click",function(){
        let copyText=document.getElementById("export_textarea").value;
        navigator.clipboard.writeText(copyText);
    })    
}else{
    document.getElementById("export_copy_button").style.display="none";
}
document.getElementById("export_close").addEventListener("click",function(){
    close_popup();
    document.getElementById("export_popup").removeAttribute("style");
})
document.getElementById("url_export").addEventListener("click",function(){
    open_popup();
    document.getElementById("url_export_popup").style.display="block";
    let link=document.location.origin+document.location.pathname+'#'+export_text();
    document.getElementById("url_export_link").href=link;
    document.getElementById("url_export_link").innerText=link;
})
if(window.isSecureContext){
    document.getElementById("url_export_copy_button").addEventListener("click",function(){
        let copyText=document.getElementById("url_export_link").href;
        navigator.clipboard.writeText(copyText);
    })    
}else{
    document.getElementById("url_export_copy_button").style.display="none";
}
document.getElementById("url_export_close").addEventListener("click",function(){
    close_popup();
    document.getElementById("url_export_popup").removeAttribute("style");
})
document.getElementById("import").addEventListener("click",function(){
    open_popup();
    document.getElementById("import_popup").style.display="block";
})
document.getElementById("import_button").addEventListener("click",function(){
    let modified=true;
    load_text(document.getElementById("import_textarea").value,function(){
        alert("La map est invalide");
        modified=false;
    });
    update();
    close_popup();
    document.getElementById("import_popup").removeAttribute("style");
    if(modified){
        undo_stack.push(JSON.parse(JSON.stringify(map)));
        redo_stack=[];
        unsaved=false;
    }
})
document.getElementById("import_close").addEventListener("click",function(){
    close_popup();
    document.getElementById("import_popup").removeAttribute("style");
})


/************************ Mode menu buttons ************************/
Array.from(document.getElementById("mode_menu").children).forEach(element=>{
    if(element.tagName=="BUTTON"){
        element.addEventListener("click",function(){
            mode=Number(element.firstChild.value);
            element.firstChild.click()
        })
    }
});
document.getElementById("mode_menu").firstElementChild.firstChild.click();
