import socket
import baseServer
from threading import Thread
from jeu import *
from hashlib import sha256
from http.server import HTTPStatus
codes={}
for x in HTTPStatus:
    codes[x.value]=x.name.encode()
class UrlError(Exception):
    pass

def url_decoder(bin):
    text=list(bin)
    i=0
    while i<len(text):
        if text[i]==37:
            try:
                text[i]=int(chr(text[i+1])+chr(text[i+2]),16)
            except ValueError:
                raise UrlError
            del text[i+1]
            del text[i+1]
        i+=1
    return bytes(text)

def checksum(players):
    for player in players:
        if type(player)!=Joueur:
            raise TypeError("ce ne sont pas que des joueurs")
    return sha256("".join([x.id for x in players]).encode()).hexdigest().encode()
def index(params):
    if b"partie" in params and b"player" in params:
        with open("./WEB/index.html","rb")as f:
            resp=f.read().replace(b"##partie##",params[b"partie"])
            resp=resp.replace(b"##player##",params[b"player"])
        return 200,resp
    elif b"partie" in params and b"pseudo" in params and b"color" in params:
        if params[b"partie"]==b"auto":
            if b"auto" in parties_pec:
                parties_pec[b"auto"].append(Joueur(params[b"pseudo"].decode("latin-1"),"#"+params[b"color"].decode()))
                if len(parties_pec[b"auto"])==4:
                    name=b"auto"+generate_code(20).encode()
                    parties[name]=Jeu(*parties_pec[b"auto"])
                    parties_pec[b"auto"].append(name)
                    parties_pec[b"auto"].append("started")
                    parties_pec[b"auto"].append(1)
                    return "redirect",b"/?partie="+name+b"&player="+parties_pec[b"auto"][-4].id.encode()
                with open("./waiting_auto.html","rb") as f:
                    return 200,f.read().replace(b"##player##",parties_pec[b"auto"][-1].id.encode())
                return
            else:
                parties_pec[b"auto"]=[Joueur(params[b"pseudo"].decode("latin-1"),"#"+params[b"color"].decode())]
                with open("./waiting_auto.html","rb") as f:
                    resp=f.read().replace(b"##player##",parties_pec[b"auto"][-1].id.encode())
                return 200,resp
        elif params[b"partie"]==b"new":
            name=generate_code(24).encode()
            parties_pec[name]=[Joueur(params[b"pseudo"].decode("latin-1"),"#"+params[b"color"].decode())]
            if b"map" in params and params[b"map"] in parties_pec_info:
                map=parties_pec_info.pop(params[b"map"])
                parties_pec_info[name]=map,get_map_nb_players(map)
            with open("./waiting.html","rb")as f:
                ctn=f.read()
                ctn=ctn.replace(b"##partie##",name)
                ctn=ctn.replace(b"##player##",parties_pec[name][0].id.encode())
            return 200,ctn
        elif params[b"partie"].startswith(b"rej_"):
            name=params[b"partie"][4:]
            if not name in parties_pec:
                if name in parties:
                    return 409,b"<html><body>Cette partie est d&eacute;j&agrave; commenc&eaccute;e.</body></html>"
                else:
                    return 409,b"<html><body>Cette partie n'existe pas.</body></html>"
                return
            if len(parties_pec[name])==parties_pec_info.get(name,(0,4))[1]:
                return 409,b"<html><body>Cette partie est compl&egrave;te.</body></html>"
            parties_pec[name].append(Joueur(params[b"pseudo"].decode("latin-1"),"#"+params[b"color"].decode()))
            with open("./waiting.html","rb") as f:
                ctn=f.read()
                ctn=ctn.replace(b"##partie##",name)
                ctn=ctn.replace(b"##player##",parties_pec[name][-1].id.encode())
            return 200,ctn
        else:
            return 404,b"Nom de partie invalide"
    else:
        with open("./login.html","rb") as f:
            ctn=f.read()
        return 200,ctn
class Server(baseServer.Server):
    def request(self,method,path,parameters,headers,body,version):
        if method==b"GET":
            if path==b"/index.html":
                code,resp=index(parameters)
                if code=="redirect":
                    self.redirect(resp,version)
                    return
                self.cnx.send(version+b" "+str(code).encode()+b" "+codes[code]+b"\r\n\r\n"+resp)
                self.cnx.close()
                return
            elif path==b"/script.js":
                if b"partie" in parameters and b"player" in parameters:
                    with open("./WEB/script.js","rb")as f:
                        resp=f.read().replace(b"##partie##",parameters[b"partie"])
                        self.cnx.send(version+b" 200 OK\r\n\r\n"+parameters[b"player"].join(resp.split(b"##player##")))
                        return
            elif path==b"/wait":
                if b"no" in parameters and b"partie" in parameters and b"player" in parameters and parameters[b"partie"] in parties:
                    self.cnx.send(version+b" 200 OK\r\n\r\n"+parties[parameters[b"partie"]].wait(parameters[b"player"].decode(),int(parameters[b"no"].decode())).encode())
                    return
                elif b"partie" in parameters and b"player" in parameters and b"checksum" in parameters and parameters[b"partie"] in parties_pec and parameters[b"player"].decode() in [x.id for x in parties_pec[parameters[b"partie"]]]:
                    cond=parameters[b"checksum"]
                    while cond==parameters[b"checksum"]:
                        wait(0.1)
                        try:
                            cond=checksum(parties_pec[parameters[b"partie"]])
                        except TypeError as e:
                            if parties_pec[parameters[b"partie"]][-2]=="started":
                                name=parties_pec[parameters[b"partie"]][-3]
                                self.cnx.send(version+b" 200 OK\r\nType: started\r\n\r\n"+name)
                                parties_pec[parameters[b"partie"]][-1]+=1
                                if parties_pec[parameters[b"partie"]][-1]==len(parties_pec[parameters[b"partie"]])-3:
                                    del parties_pec[parameters[b"partie"]]
                                return
                    self.cnx.send(version+b" 200 OK\r\nType: changelist\r\n\r\n"+b"\\".join([(x.pseudo+"@"+x.color).encode() for x in parties_pec[parameters[b"partie"]]])+b"!"+cond)
                    return
            elif path==b"/player.svg":
                color=b"#"+parameters.get(b"color",b"000000")
                if len(color)!=7:
                    color=b"#000000"
                for x in color[1:]:
                    if (x<0x30 or x>0x39) and (x<0x61 or x>0x66):
                        color="#000000"
                        break
                with open("./WEB/player.svg","rb")as f:
                    self.cnx.send(version+b" 200 OK\r\nContent-Type: image/svg+xml\r\n\r\n"+f.read().replace(b"#2677d7",color))
            elif path==b"/play":
                if b"partie" in parameters and b"player" in parameters and b"coup" in parameters and parameters[b"partie"] in parties:
                    try:
                        parties[parameters[b"partie"]].play(int(parameters[b"coup"].decode()),parameters[b"player"].decode())
                    except ValueError as e:
                        self.cnx.send(version+b" 409 Conflict\r\n\r\n"+str(e).encode())
                        return
                    self.cnx.send(version+b" 204 No Content\r\n\r\n")
                    return
                else:
                    self.cnx.send(version+b" 404 Not Found\r\n\r\n")
                    return
            elif path==b"/start":
                if b"partie" in parameters and b"player" in parameters and parameters[b"partie"] in parties_pec and parties_pec[parameters[b"partie"]][0].id.encode()==parameters[b"player"]:
                    if parameters[b"partie"] in parties_pec_info:
                        map=parties_pec_info.pop(parameters[b"partie"])[0]
                        parties[parameters[b"partie"]]=Jeu(*parties_pec[parameters[b"partie"]],map=map)
                    else:
                        parties[parameters[b"partie"]]=Jeu(*parties_pec[parameters[b"partie"]])
                    parties_pec[parameters[b"partie"]].append(b"")
                    parties_pec[parameters[b"partie"]].append("started")
                    parties_pec[parameters[b"partie"]].append(0)
                    self.cnx.send(version+b" 204 No Content\r\n\r\n")
                    return
            elif path==b"/bonus":
                if b"partie" in parameters and b"player" in parameters and b"no" in parameters and b"x" in parameters and b"y" in parameters and b"dir" in parameters and parameters[b"partie"] in parties:
                    try:
                        parties[parameters[b"partie"]].bonus(int(parameters[b"no"].decode()),parameters[b"player"].decode(),(int(parameters[b"x"].decode()),int(parameters[b"y"].decode())),int(parameters[b"dir"].decode()))
                    except ValueError as e:
                        self.cnx.send(version+b" 409 Conflict\r\n\r\n"+str(e).encode())
                        return
                    self.cnx.send(version+b" 204 No Content\r\n\r\n")
                    return
                else:
                    self.cnx.send(version+b" 404 Not Found\r\n\r\n")
                    return
            else:
                try:
                    with open("./WEB/"+path.decode(),"rb") as f:
                        self.cnx.send(version+b" 200 OK\r\n\r\n"+f.read())
                        return
                except FileNotFoundError:
                    self.cnx.send(version+b" 404 Not Found\r\n\r\n")
                    return
        elif method==b"POST":
            if path==b"/perso_map":
                map=body.decode()
                if is_valide_map(map):
                    name=generate_code(16).encode()
                    parties_pec_info[name]=map
                    self.cnx.send(version+b" 200 OK\r\n\r\n"+name)
                    return
                else:
                    self.cnx.send(version+b" 422 Unprocessable entity\r\n\r\n")
                    return
            else:
                self.cnx.send(version+b" 501 Not Implemented\r\n\r\n")
                return
        else:
            self.cnx.send(version+b" 501 Not Implemented\r\n\r\n")
            return

parties={}
parties_pec={}
parties_pec_info={}
Server().start()
