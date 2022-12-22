import socket
import ssl
import baseServer
from threading import Thread
from jeu import *
from hashlib import sha256,sha1
from base64 import b64encode
class UrlError(Exception):
    pass

def create_game_code(auto):
    prefix="auto" if auto else ""
    lenght=4
    while 1:
        for i in range(10):
            result=prefix+generate_code(lenght)
            if not (result in parties or result in parties_pec):
                return result
        lenght+=1

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

def valid_color(c):
    if color_diff(c,(0,0,0))<=20:
        return False
    if color_diff(c,(0,96,2))<=20:
        return False
    if color_diff(c,(255,0,0))<=20:
        return False
    return True

def index(params):
    if b"partie" in params and b"player" in params:
        if params[b"partie"] in parties:
            with open("./WEB/index.html","rb")as f:
                resp=f.read().replace(b"##partie##",params[b"partie"])
                resp=resp.replace(b"##player##",params[b"player"])
            return 200,resp
        else:
            return 404,b"Cette partie n'existe pas"
    elif b"partie" in params and b"pseudo" in params and b"color" in params:
        player=Joueur(params[b"pseudo"].decode("latin-1"),"#"+params[b"color"].decode())
        if params[b"partie"]==b"auto":
            if b"auto" in parties_pec:
                parties_pec[b"auto"].add_player(player)
                if len(parties_pec[b"auto"])==4:
                    name=create_game_code(True).encode()
                    parties_pec[b"auto"].name=name
                    parties[name]=parties_pec[b"auto"].make()
                    del parties_pec[b"auto"]
                    return "redirect",b"/?partie="+name+b"&player="+player.id
                with open("./waiting.html","rb") as f:
                    resp=f.read()
                resp=resp.replace(b"##player##",player.id)
                resp=resp.replace(b"##partie##",b"auto")
                return 200,resp
            else:
                parties_pec[b"auto"]=Ppec()
                parties_pec[b"auto"].add_player(player)
                with open("./waiting.html","rb") as f:
                    resp=f.read().replace(b"##player##",player.id.encode()).replace(b"##partie##",b"auto")
                return 200,resp
        elif params[b"partie"]==b"new":
            name=create_game_code(False).encode()
            parties_pec[name]=Ppec()
            parties_pec[name].name=name
            parties_pec[name].add_player(player)
            if b"map" in params:
                if params[b"map"] in maps:
                    map=maps.pop(params[b"map"])
                    parties_pec[name].map=map
                    parties_pec[name].max_players=get_map_nb_players(map)
                else:
                    del parties_pec[name]
                    return 404,b"Nom de map invalide"
            with open("./waiting.html","rb")as f:
                ctn=f.read()
            ctn=ctn.replace(b"##partie##",name)
            ctn=ctn.replace(b"##player##",player.id.encode())
            return 200,ctn
        elif params[b"partie"].startswith(b"rej_"):
            name=params[b"partie"][4:]
            if not name in parties_pec:
                if name in parties:
                    return 409,b"<html><body>Cette partie est d&eacute;j&agrave; commenc&eaccute;e.</body></html>"
                else:
                    return 409,b"<html><body>Cette partie n'existe pas.</body></html>"
                return
            if len(parties_pec[name])==parties_pec[name].max_players:
                return 409,b"<html><body>Cette partie est compl&egrave;te.</body></html>"
            parties_pec[name].add_player(player)
            with open("./waiting.html","rb") as f:
                ctn=f.read()
            ctn=ctn.replace(b"##partie##",name)
            ctn=ctn.replace(b"##player##",player.id.encode())
            return 200,ctn
        else:
            return 404,b"Nom de partie invalide"
    else:
        with open("./login.html","rb") as f:
            ctn=f.read()
        return 200,ctn
class Server(baseServer.Server):
    def request(self,method,path,parameters,headers,body):
        if method==b"GET":
            if path==b"/index.html":
                code,resp=index(parameters)
                if code=="redirect":
                    self.redirect(resp)
                    return
                self.response(code,{},resp)
                return
            elif path==b"/script.js":
                if b"partie" in parameters and b"player" in parameters:
                    with open("./WEB/script.js","rb")as f:
                        resp=f.read().replace(b"##partie##",parameters[b"partie"])
                        self.response(200,{},resp.replace(b"##player##",parameters[b"player"]))
                        return
            elif path==b"/wait":
                if b"partie" in parameters and b"player" in parameters and b"checksum" in parameters and parameters[b"partie"] in parties_pec and parameters[b"player"].decode() in [x.id for x in parties_pec[parameters[b"partie"]]]:
                    cond=parameters[b"checksum"]
                    while cond==parameters[b"checksum"]:
                        wait(0.1)
                        try:
                            cond=checksum(parties_pec[parameters[b"partie"]])
                        except TypeError as e:
                            if parties_pec[parameters[b"partie"]][-2]=="started":
                                name=parties_pec[parameters[b"partie"]][-3]
                                self.response(200,{b"Type":b"started"},name)
                                parties_pec[parameters[b"partie"]][-1]+=1
                                if parties_pec[parameters[b"partie"]][-1]==len(parties_pec[parameters[b"partie"]])-3:
                                    del parties_pec[parameters[b"partie"]]
                                return
                    self.response(200,{b"Type":b"changelist"},b"%~&1%~&!".join([(x.pseudo+"%~&2%~&!"+x.color).encode("latin-1") for x in parties_pec[parameters[b"partie"]]])+b"%~&0%~&!"+cond)
                    return
                else:
                    self.response(404,{})
                    return
            elif path==b"/game":
                if b"Sec-WebSocket-Key" in headers:
                    ws_accept=headers[b"Sec-WebSocket-Key"]
                    ws_accept+=b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
                    ws_accept=b64encode(sha1(ws_accept).digest())
                    if b"partie" in parameters and b"player" in parameters and parameters[b"partie"] in parties:
                        player=parties[parameters[b"partie"]].get_player_by_id(parameters[b"player"].decode())
                        if player == None:
                            self.response(404,{})
                        else:
                            self.response(101,{b"Connection":b"Upgrade",b"Upgrade":b"websocket",b"Sec-WebSocket-Accept":ws_accept},close=False)
                            player.reset_cnx()
                            player.cnx.cnx=self.cnx
                            player.cnx.send(b"u"+parties[parameters[b"partie"]].last_event.encode())
            elif path==b"/pec":
                if b"Sec-WebSocket-Key" in headers:
                    ws_accept=headers[b"Sec-WebSocket-Key"]
                    ws_accept+=b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
                    ws_accept=b64encode(sha1(ws_accept).digest())
                    if b"partie" in parameters and b"player" in parameters and parameters[b"partie"] in parties_pec:
                        player=parties_pec[parameters[b"partie"]].get_player_by_id(parameters[b"player"].decode())
                        if player == None:
                            self.response(404,{})
                        else:
                            self.response(101,{b"Connection":b"Upgrade",b"Upgrade":b"websocket",b"Sec-WebSocket-Accept":ws_accept},close=False)
                            player.cnx.cnx=self.cnx
                            parties_pec[parameters[b"partie"]].send_info()
            elif path==b"/start":
                if b"partie" in parameters and b"player" in parameters and parameters[b"partie"] in parties_pec and parties_pec[parameters[b"partie"]].check_first(parameters[b"player"].decode()):
                    parties[parameters[b"partie"]]=parties_pec[parameters[b"partie"]].make()
                    del parties_pec[parameters[b"partie"]]
                    self.response(204,{})
                    return
            elif path==b"/check_color":
                if b"color" in parameters:
                    valid=False
                    unused=True
                    try:
                        color=parse_color(parameters[b"color"])
                    except:
                        pass
                    else:
                        if b"partie" in parameters and parameters[b"partie"] in parties_pec:
                            unused=parties_pec[parameters[b"partie"]].check_color(color)
                        valid=valid_color(color)
                    self.response(204,{b"Result":(b"Ok" if unused else b"Used") if valid else b"Invalid"})
                else:
                    self.response(422,{})
            elif path==b"/check_pseudo":
                if b"pseudo" in parameters and b"partie" in parameters:
                    if parameters[b"partie"] in parties_pec:
                        unused=parties_pec[parameters[b"partie"]].check_pseudo(parameters[b"pseudo"].decode())
                        self.response(204,{b"Result":b"Ok" if unused else b"Used"})
                    elif parameters[b"partie"] == b"auto": # only if the game is creating
                        self.response(204,{b"result":b"Ok"})
                    else:
                        self.response(422,{})
                else:
                    self.response(422,{})
            else:
                try:
                    with open("./WEB/"+path.decode(),"rb") as f:
                        self.response(200,{},f.read())
                        return
                except FileNotFoundError:
                    self.response(404,{})
                    return
        elif method==b"POST":
            if path==b"/perso_map":
                map=body.decode()
                if is_valide_map(map):
                    name=generate_code(16).encode()
                    while name in maps:
                        name=generate_code(16).encode()
                    maps[name]=map
                    self.response(200,{},name)
                    return
                else:
                    self.response(422,{})
                    return
            else:
                self.response(404,{})
                return
        else:
            self.response(405,{})
            return

class DeleteOldParties(Thread):
    def run(self):
        while 1:
            wait(3600)
            to_del=[]
            for partie in parties:
                if not parties[partie].active:
                    to_del.append(partie)
            for x in to_del:
                del parties[x]

parties={}
parties_pec={}
maps={}
Server().start()
a=socket.socket(socket.AF_INET6)
a.setsockopt(socket.IPPROTO_IPV6,socket.IPV6_V6ONLY,0)
a.bind(("",443))
a.listen(50)
a=ssl.wrap_socket(a,
                  keyfile="lasers.key",
                  certfile="lasers.crt",
                  server_side=True)
Server(a).start() #https
DeleteOldParties().start()
while 1:
    pass
