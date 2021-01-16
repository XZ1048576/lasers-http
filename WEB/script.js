let waits=0;
const partie="##partie##";
const PLAYER="##player##";
window.addEventListener("resize",function(){
    document.getElementById("main-scroll").style.maxWidth=(window.innerWidth-300)+"px";
    document.getElementById("main-scroll").style.maxHeight=window.innerHeight+"px";
    document.getElementById("left").style.maxHeight=window.innerHeight+"px";
},true);
document.getElementById("main-scroll").style.maxWidth=(window.innerWidth-300)+"px";
document.getElementById("main-scroll").style.maxHeight=window.innerHeight+"px";
document.getElementById("left").style.maxHeight=window.innerHeight+"px";
document.getElementById("control").onclick=function(target){
    let key=null;
    switch(target.target.id){
        case "left" :
            key=0;
            break;
        case "up" :
            key=1;
            break;
        case "right":
            key=2;
            break;
        case "down":
            key=3;
            break;
        case "turn right":
            key=4;
            break;
        case "turn left":
            key=5;
            break;
        case "fire":
            key=6;
    }
    if(Number.isInteger(key)){
        fetch("/play?coup="+key+"&partie="+partie+"&player="+PLAYER).then(reponse => {
            if(reponse.status===409){
                reponse.text().then(error =>{
                    alert(error)
                });
            }
        });
    }
}
function CloseBonus(){
    document.getElementById("jeu").onclick=null;
    document.getElementById("bonus-infos").style="";
    Array.from(document.getElementById("bonus-infos").children).forEach(function(child){
        if(child.localName=="div"){
            child.style="";
        }
    });
    Array.from(document.getElementsByClassName("mirroir")).forEach(function(mirror){
        mirror.innerHTML='';
    });
    Array.from(document.getElementsByClassName("mirroirt")).forEach(function(mirror){
        mirror.innerHTML='';
    });
    document.getElementsByClassName("cible")[0].innerHTML='';
}
document.getElementById("bonus").onclick=function(target){
    let no=null;
    let y=null;
    let x=null;
    let dir=null;
    switch(target.target.id){
        case "add_mirror":
            document.getElementById("bonus-infos").style="display: block";
            document.getElementById("add_mirror_info").style="display: block";
            document.getElementById("jeu").onclick=function(click){
                no=0;
                x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                if(document.getElementById("mirror_dir_/").checked){
                    dir=0;
                } else if(document.getElementById("mirror_dir_\\").checked){
                    dir=1;
                } else {
                    alert("Choisissez d'abord le sens du mirroir");
                    return;
                }
                console.log(x,y,dir,no);
                if(no!==null){
                    fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                        if(reponse.status===409){
                            reponse.text().then(error =>{
                                alert(error)
                            });
                        }
                    });
                }
                CloseBonus();
            }
            break;
        case "del_mirror":
            document.getElementById("bonus-infos").style="display: block";
            document.getElementById("del_mirror_info").style="display: block";
            document.getElementById("jeu").onclick=function(click){
                no=1;
                x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                dir=0;
                fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                    if(reponse.status===409){
                        reponse.text().then(error =>{
                            alert(error)
                        });
                    }
                });
                CloseBonus();
            }
            break;
        case "move_mirror":
            document.getElementById("bonus-infos").style="display: block";
            document.getElementById("move_mirror_info").style="display: block";
            Array.from(document.getElementsByClassName("mirroir")).forEach(function(mirror){
                mirror.innerHTML='<div class="move_obj"></div>';
            });
            Array.from(document.getElementsByClassName("mirroirt")).forEach(function(mirror){
                mirror.innerHTML='<div class="move_obj"></div>';
            });
            document.getElementById("jeu").onclick=function(click){
                no=2;
                x=click.pageX+document.getElementById("main-scroll").scrollLeft;
                y=click.pageY+document.getElementById("main-scroll").scrollTop;
                let hd=x%100>y%100;
                let hg=(x%100+y%100)<100;
                if(hd && hg){
                    dir=1;
                } else if(hd && !hg){
                    dir=2;
                } else if(!hd && !hg){
                    dir=3;
                } else if(!hd && hg){
                    dir=0;
                }
                x=Number.parseInt(x/100)-3;
                y=Number.parseInt(y/100)-3;
                fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                    if(reponse.status===409){
                        reponse.text().then(error =>{
                            alert(error)
                        });
                    }
                });
                CloseBonus();
            }
            break;
        case "turn_mirror":
            document.getElementById("bonus-infos").style="display: block";
            document.getElementById("turn_mirror_info").style="display: block";
            document.getElementById("jeu").onclick=function(click){
                no=3;
                x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                dir=0;
                fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                    if(reponse.status===409){
                        reponse.text().then(error =>{
                            alert(error)
                        });
                    }
                });
                CloseBonus();
            }
            break;
        case "move_cible":
            document.getElementById("bonus-infos").style="display: block";
            document.getElementById("move_cible_info").style="display: block";
            document.getElementsByClassName("cible")[0].innerHTML='<div class="move_obj"></div>';
            document.getElementById("jeu").onclick=function(click){
                no=4;
                x=click.pageX+document.getElementById("main-scroll").scrollLeft;
                y=click.pageY+document.getElementById("main-scroll").scrollTop;
                let hd=x%100>y%100;
                let hg=(x%100+y%100)<100;
                if(hd && hg){
                    dir=1;
                } else if(hd && !hg){
                    dir=2;
                } else if(!hd && !hg){
                    dir=3;
                } else if(!hd && hg){
                    dir=0;
                }
                x=Number.parseInt(x/100)-3;
                y=Number.parseInt(y/100)-3;
                fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                    if(reponse.status===409){
                        reponse.text().then(error =>{
                            alert(error)
                        });
                    }
                });
                CloseBonus();
            }
            break;
        case "2play":
            fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no=5&x=0&y=0&dir=0").then(reponse => {
                if(reponse.status===409){
                    reponse.text().then(error =>{
                        alert(error)
                    });
                }
            });
            break;
        case "add_wood":
            document.getElementById("bonus-infos").style="display: block";
            document.getElementById("add_wood_info").style="display: block";
            document.getElementById("jeu").onclick=function(click){
                no=6;
                x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                dir=0;
                fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                    if(reponse.status===409){
                        reponse.text().then(error =>{
                            alert(error)
                        });
                    }
                });
                CloseBonus();
            }
            break;
    }
}
document.getElementById("cancel_bonus").onclick=CloseBonus;
function wait(){
    fetch("/wait?no="+waits+"&partie="+partie+"&player="+PLAYER).catch(wait).then(function(reponse){
        reponse.text().then(function(text){
            let lines=text.split("%~&0%~&!")[0].split("#");
            let height=lines.length;
            let width=lines[0].split("@").length;
            document.getElementById("jeu").style="height:"+(100*height)+"px;width:"+(100*width)+"px;";
            document.getElementById("main").style="height:"+(100*height+600)+"px;width:"+(100*width+600)+"px;";
            document.getElementById("jeu").innerHTML="";
            lines.forEach(line => {
                let line_html='<div class="line">';
                line.split("@").forEach(Case => {
                    line_html+='<div class="case">';
                    Case.split("&").forEach(Element => {
                        if(Element.startsWith("joueur")){
                            img_url="data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhLS0gQ3JlYXRlZCB3aXRoIElua3NjYXBlIChodHRwOi8vd3d3Lmlua3NjYXBlLm9yZy8pIC0tPgoKPHN2ZwogICB4bWxuczpkYz0iaHR0cDovL3B1cmwub3JnL2RjL2VsZW1lbnRzLzEuMS8iCiAgIHhtbG5zOmNjPSJodHRwOi8vY3JlYXRpdmVjb21tb25zLm9yZy9ucyMiCiAgIHhtbG5zOnJkZj0iaHR0cDovL3d3dy53My5vcmcvMTk5OS8wMi8yMi1yZGYtc3ludGF4LW5zIyIKICAgeG1sbnM6c3ZnPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIKICAgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIgogICB4bWxuczpzb2RpcG9kaT0iaHR0cDovL3NvZGlwb2RpLnNvdXJjZWZvcmdlLm5ldC9EVEQvc29kaXBvZGktMC5kdGQiCiAgIHhtbG5zOmlua3NjYXBlPSJodHRwOi8vd3d3Lmlua3NjYXBlLm9yZy9uYW1lc3BhY2VzL2lua3NjYXBlIgogICB3aWR0aD0iMTAwbW0iCiAgIGhlaWdodD0iMTAwbW0iCiAgIHZpZXdCb3g9IjAgMCAxMDAgMTAwIgogICB2ZXJzaW9uPSIxLjEiCiAgIGlkPSJzdmc4IgogICBpbmtzY2FwZTp2ZXJzaW9uPSIwLjkyLjIgKDVjM2U4MGQsIDIwMTctMDgtMDYpIgogICBzb2RpcG9kaTpkb2NuYW1lPSJwbGF5ZXIuc3ZnIj4KICA8ZGVmcwogICAgIGlkPSJkZWZzMiIgLz4KICA8c29kaXBvZGk6bmFtZWR2aWV3CiAgICAgaWQ9ImJhc2UiCiAgICAgcGFnZWNvbG9yPSIjZmZmZmZmIgogICAgIGJvcmRlcmNvbG9yPSIjNjY2NjY2IgogICAgIGJvcmRlcm9wYWNpdHk9IjEuMCIKICAgICBpbmtzY2FwZTpwYWdlb3BhY2l0eT0iMC4wIgogICAgIGlua3NjYXBlOnBhZ2VzaGFkb3c9IjIiCiAgICAgaW5rc2NhcGU6em9vbT0iMS45Nzk4OTkiCiAgICAgaW5rc2NhcGU6Y3g9IjI0Mi4xNTQ5MSIKICAgICBpbmtzY2FwZTpjeT0iNjIuNDYzMzI5IgogICAgIGlua3NjYXBlOmRvY3VtZW50LXVuaXRzPSJtbSIKICAgICBpbmtzY2FwZTpjdXJyZW50LWxheWVyPSJsYXllcjEiCiAgICAgc2hvd2dyaWQ9ImZhbHNlIgogICAgIGlua3NjYXBlOndpbmRvdy13aWR0aD0iMTkyMCIKICAgICBpbmtzY2FwZTp3aW5kb3ctaGVpZ2h0PSIxMDE3IgogICAgIGlua3NjYXBlOndpbmRvdy14PSItOCIKICAgICBpbmtzY2FwZTp3aW5kb3cteT0iLTgiCiAgICAgaW5rc2NhcGU6d2luZG93LW1heGltaXplZD0iMSIgLz4KICA8bWV0YWRhdGEKICAgICBpZD0ibWV0YWRhdGE1Ij4KICAgIDxyZGY6UkRGPgogICAgICA8Y2M6V29yawogICAgICAgICByZGY6YWJvdXQ9IiI+CiAgICAgICAgPGRjOmZvcm1hdD5pbWFnZS9zdmcreG1sPC9kYzpmb3JtYXQ+CiAgICAgICAgPGRjOnR5cGUKICAgICAgICAgICByZGY6cmVzb3VyY2U9Imh0dHA6Ly9wdXJsLm9yZy9kYy9kY21pdHlwZS9TdGlsbEltYWdlIiAvPgogICAgICAgIDxkYzp0aXRsZSAvPgogICAgICA8L2NjOldvcms+CiAgICA8L3JkZjpSREY+CiAgPC9tZXRhZGF0YT4KICA8ZwogICAgIGlua3NjYXBlOmxhYmVsPSJDYWxxdWUgMSIKICAgICBpbmtzY2FwZTpncm91cG1vZGU9ImxheWVyIgogICAgIGlkPSJsYXllcjEiPgogICAgPHBhdGgKICAgICAgIGlua3NjYXBlOmNvbm5lY3Rvci1jdXJ2YXR1cmU9IjAiCiAgICAgICBpZD0icGF0aDQ1MjEiCiAgICAgICBkPSJNIDEwMCwwIDEwMCwxMDAgSCA1OCBWIDg0IEggNDIgViAxMDAgSCAwIFoiCiAgICAgICBzdHlsZT0iZmlsbDoj";
                            img_url+=btoa(Element.split("_")[2]);
                            img_url+="O2ZpbGwtb3BhY2l0eToxO3N0cm9rZS13aWR0aDowIgogICAgICAgc29kaXBvZGk6bm9kZXR5cGVzPSJjY2NjY2NjYyIgLz4KICAgIDxwYXRoCiAgICAgICBzb2RpcG9kaTpub2RldHlwZXM9InpzY2NjY3oiCiAgICAgICBpbmtzY2FwZTpjb25uZWN0b3ItY3VydmF0dXJlPSIwIgogICAgICAgaWQ9InBhdGg0NTE5IgogICAgICAgZD0iTSA4OCw1MCBDIDg4LDQ2IDkyLDQyIDk2LDQyIEggMTAwIFYgNTggSCA5NiBDIDkyLDU4IDg4LDU0IDg4LDUwIFoiCiAgICAgICBzdHlsZT0iZmlsbDojZmYwMDAwO2ZpbGwtb3BhY2l0eToxO3N0cm9rZS13aWR0aDowIiAvPgogIDwvZz4KPC9zdmc+Cg==";
                            line_html+='<div class="joueur" style="transform: rotate('+Element.split("_")[1]+'deg);background-image: url(\''+img_url+'\')"></div>';
                        } else if(Element.startsWith("killed")){
                            line_html+='<div class="killed" style="transform: rotate('+Element.split("_")[1]+'deg)"></div>';
                        } else {
                            line_html+='<div class="'+Element+'"></div>';
                        }                    
                    });
                    line_html+="</div>";
                });
                line_html+="</div>";
                document.getElementById("jeu").innerHTML+=line_html;
            });
            let players=text.split("%~&0%~&!")[1].split("%~&2%~&!")[0].split("%~&1%~&!");
            document.getElementById("scores").innerHTML=""
            players.forEach(player => {
                let score_html='<div class="score" style="background-color: '+player.split("%~&3%~&!")[2]+';">';
                score_html+='<div class="pseudo">';
                score_html+=player.split("%~&3%~&!")[0];
                score_html+='</div><div class="cash">';
                score_html+=player.split("%~&3%~&!")[1];
                score_html+='</div><div class="piece"></div>';
                switch(player.split("%~&3%~&!")[3]){
                    case '1':
                        score_html+='<div class="actual_player"></div>';
                        break;
                    case '2':
                        score_html+='<div class="actual_player_2x"></div>';
                }
                score_html+='</div>';
                document.getElementById("scores").innerHTML+=score_html;
            });
            document.getElementById("cible_price").innerText=text.split("%~&2%~&!")[1];
            document.getElementById("player_price").innerText=text.split("%~&2%~&!")[2];
            if(text.split("%~&0%~&!").length>2){
                document.getElementById("main-scroll").style="display: none";
                document.getElementById("left").style="display: none";
                document.getElementById("win").style="display: block;";
                document.body.parentElement.style="background-color:"+text.split("%~&0%~&!")[2].split("%~&3%~&!")[1]+";";
                document.getElementById("winner_pseudo").innerText=text.split("%~&0%~&!")[2].split("%~&3%~&!")[0];
            } else {
                waits+=1;
                wait();
            }
        });
    });
}
wait();