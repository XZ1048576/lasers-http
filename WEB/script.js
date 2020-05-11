let waits=0;
const partie="##partie##";
const PLAYER="##player##";
window.addEventListener("resize",function(){
    document.getElementById("main-scroll").style.maxWidth=(window.innerWidth-300)+"px";
    document.getElementById("main-scroll").style.maxHeight=window.innerHeight+"px";
},true);
document.getElementById("main-scroll").style.maxWidth=(window.innerWidth-300)+"px";
document.getElementById("main-scroll").style.maxHeight=window.innerHeight+"px";
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
document.getElementById("bonus").onclick=function(target){
    let no=null;
    let y=null;
    let x=null;
    let dir=null;
    console.log("bonus");
    switch(target.target.id){
        case "add_mirror":
            document.getElementById("jeu").onmousedown=function(click){
                no=0;
                if(click.button==0){
                    x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                    y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                    dir=0;
                } else if(click.button==2){
                    x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                    y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                    dir=1;
                } else {
                    no=6;
                }
                console.log(x,y,dir,no);
                if(no!=6){
                    fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                        if(reponse.status===409){
                            reponse.text().then(error =>{
                                alert(error)
                            });
                        }
                    });
                }
                document.getElementById("jeu").onmousedown=null;
    
            }
            break;
        case "del_mirror":
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
                document.getElementById("jeu").onclick=null;
            }
            break;
        case "move_mirror":
            for(let i=0;i<document.getElementsByClassName("mirroir").length;i++){
                document.getElementsByClassName("mirroir")[i].innerHTML='<div class="move_obj"></div>';
            }
            for(let i=0;i<document.getElementsByClassName("mirroirt").length;i++){
                document.getElementsByClassName("mirroirt")[i].innerHTML='<div class="move_obj"></div>';
            }
            document.getElementById("jeu").onclick=function(click){
                no=2;
                x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                let hd=click.pageX%100>click.pageY%100;
                let hg=(click.pageX%100+click.pageY%100)<100;
                if(hd && hg){
                    dir=1;
                } else if(hd && !hg){
                    dir=2;
                } else if(!hd && !hg){
                    dir=3;
                } else if(!hd && hg){
                    dir=0;
                }
                fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                    if(reponse.status===409){
                        reponse.text().then(error =>{
                            alert(error)
                        });
                    }
                });
                for(let i=0;i<document.getElementsByClassName("mirroir").length;i++){
                    document.getElementsByClassName("mirroir")[i].innerHTML='';
                }
                for(let i=0;i<document.getElementsByClassName("mirroirt").length;i++){
                    document.getElementsByClassName("mirroir")[i].innerHTML='';
                }
                document.getElementById("jeu").onclick=null;
            }
            break;
        case "turn_mirror":
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
                document.getElementById("jeu").onclick=null;
            }
            break;
        case "move_cible":
            for(let i=0;i<document.getElementsByClassName("cible").length;i++){
                document.getElementsByClassName("cible")[i].innerHTML='<img src="move_obj.png"/>';
            }
            document.getElementById("jeu").onclick=function(click){
                no=4;
                x=Number.parseInt((click.pageX+document.getElementById("main-scroll").scrollLeft)/100)-3;
                y=Number.parseInt((click.pageY+document.getElementById("main-scroll").scrollTop)/100)-3;
                let hd=click.pageX%100>click.pageY%100;
                let hg=(click.pageX%100+click.pageY%100)<100;
                if(hd && hg){
                    dir=1;
                } else if(hd && !hg){
                    dir=2;
                } else if(!hd && !hg){
                    dir=3;
                } else if(!hd && hg){
                    dir=0;
                }
                fetch("/bonus?partie="+partie+"&player="+PLAYER+"&no="+no+"&x="+x+"&y="+y+"&dir="+dir).then(reponse => {
                    if(reponse.status===409){
                        reponse.text().then(error =>{
                            alert(error)
                        });
                    }
                });
                for(let i=0;i<document.getElementsByClassName("cible").length;i++){
                    document.getElementsByClassName("cible")[i].innerHTML='';
                }
                document.getElementById("jeu").onclick=null;
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
    }
}
function wait(){
    fetch("/wait?no="+waits+"&partie="+partie+"&player="+PLAYER).catch(wait).then(function(reponse){
        reponse.text().then(function(text){
            let lines=text.split("!")[0].split("#");
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
                            line_html+='<div class="joueur" style="transform: rotate('+Element.split("_")[1]+'deg);background-image: url(\'player.svg?color='+Element.split("_")[2]+'\')"></div>';
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
            let players=text.split("!")[1].split(":")[0].split("\\");
            document.getElementById("scores").innerHTML=""
            players.forEach(player => {
                let score_html='<div class="score" style="background-color: '+player.split("@")[2]+';">';
                score_html+='<div class="pseudo">';
                score_html+=player.split("@")[0];
                score_html+='</div><div class="cash">';
                score_html+=player.split("@")[1];
                score_html+='</div><div class="piece"></div></div>';
                document.getElementById("scores").innerHTML+=score_html;
            });
            document.getElementById("cible_price").innerText=text.split(":")[1];
            document.getElementById("player_price").innerText=text.split(":")[2];
            if(text.split("!").length>2){
                document.getElementById("main").style="display: none";
                document.getElementById("left").style="display: none";
                document.getElementById("win").style="display: block; background-color:"+text.split("!")[2].split("@")[1]+"; height: "+document.body.clientWidth+"px;";
                document.getElementById("winner_pseudo").innerText=text.split("!")[2].split("@")[0];
            } else {
                waits+=1;
                wait();
            }
        });
    });
}
wait();