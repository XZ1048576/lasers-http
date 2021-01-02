from time import sleep as wait
import random as _random
def generate_code(lenght):
    return ("{:0>"+str(lenght)+"x}").format(_random.randrange(16**lenght))
def get_map_nb_players(map):
    r=0
    for l in map:
        if l in "5678":
            r+=1
    return r
def is_valide_map(map):
    if len(map.split("!"))!=2:
        return False
    if map.count("4")!=1:
        return False
    if len(map.split("!")[1])!=1:
        return False
    if not map.split("!")[1] in "0123":
        return False
    for l in map.split("!")[0]:
        if not l in "012345678#":
            return False
    k=[len(x) for x in map.split("!")[0].split("#")]
    for x in k:
        if x!=k[0]:
            return False
    if get_map_nb_players(map)<2:
        return False
    return True
class Jeu:
    """map du jeu:
    0: bois
    1: vide
    2: mirroir /
    3: mirroir \\
    4: cible
    5: joueur laser g
    6: joueur laser h
    7: joueur laser d
    8: joueur laser b

    coups:
    0: gauche
    1: monter
    2: droite
    3: descendre
    4: tourner horraire
    5: tourner anti-horraire
    6: tirer

    bonus:
    0: ajouter mirroir (dir: 0:/ 1:\\)
    1: supprimer un mirroir
    2: déplacer un mirroir (dir: 0:g 1:h 2:d 3:b)
    3: tourner un mirroir
    4: déplacer la cible (dir: 0:g 1:h 2:d 3:b)
    5: jouer 2x
    """
    def __init__(self,*joueurs,map="511116#111111#114111#111211#111111#811117!1"):
        self.joueurs=list(joueurs)
        pos_joueurs=[]
        self.map=[]
        for y,i in enumerate(map.split("!")[0].split("#")):
            a=[]
            for x,j in enumerate(i):
                a.append(int(j))
                if j in "5678":
                    pos_joueurs.append((x,y))
            self.map.append(a)
        self.tour=0
        self.twice=False
        self.events=[]
        self.cible_price=5
        self.nb_tirs_cible=0
        self.player_price=16
        self.nb_tirs_player=0
        for i,pos in enumerate(pos_joueurs):
            if len(self.joueurs)>i:
                self.joueurs[i].init(self,pos,50)
            else:
                self.map[pos[1]][pos[0]]=int(map.split("!")[1])
        for joueur in self.joueurs:
            self.events.append([self.sort(self.mapping())])
        self.can_play=True
    def _add_loc(self,dir):
        if dir=="b":
            a=[]
            for i in range(len(self.map[0])):
                a.append(0)
            self.map.append(a)
        elif dir=="h":
            a=[]
            for i in range(len(self.map[0])):
                a.append(0)
            self.map.insert(0,a)
            for joueur in self.joueurs:
                joueur.y+=1
        elif dir=="d":
            for x in self.map:
                x.append(0)
        elif dir=="g":
            for x in self.map:
                x.insert(0,0)
            for joueur in self.joueurs:
                joueur.x+=1
    def laser_input(self,dir,casex,casey,map_images):
        if not dir in (1,2,3,4):
            raise ValueError("la direction doit etre 1 2 3 ou 4")
        if casex>=len(self.map[0]):
            self._add_loc("d")
            return False
        if casey>=len(self.map):
            self._add_loc("b")
            return False
        if casex<0:
            self._add_loc("g")
            return False
        if casey<0:
            self._add_loc("h")
            return False
        if self.map[casey][casex]==0:
            map_images[casey][casex].add("feu")
            map_images[casey][casex].add("laser"+"dbgh"[dir-1])
            self.map[casey][casex]=1
            return True
        elif self.map[casey][casex]==1:
            if dir%2:
                map_images[casey][casex].add("laserg")
                map_images[casey][casex].add("laserd")
            else:
                map_images[casey][casex].add("laserh")
                map_images[casey][casex].add("laserb")
            map_images[casey][casex].add("lasergc")
            map_images[casey][casex].add("laserdc")
            map_images[casey][casex].add("laserhc")
            map_images[casey][casex].add("laserbc")
            if dir==1:
                return self.laser_input(dir,casex-1,casey,map_images)
            elif dir==2:
                return self.laser_input(dir,casex,casey-1,map_images)
            elif dir==3:
                return self.laser_input(dir,casex+1,casey,map_images)
            else:
                return self.laser_input(dir,casex,casey+1,map_images)
        elif self.map[casey][casex]==2:
            if dir<3:
                map_images[casey][casex].add("laserd")
                map_images[casey][casex].add("laserb")
                map_images[casey][casex].add("laserdc")
                map_images[casey][casex].add("laserbc")
            else:
                map_images[casey][casex].add("laserg")
                map_images[casey][casex].add("laserh")
                map_images[casey][casex].add("lasergc")
                map_images[casey][casex].add("laserhc")
            if dir==1:
                return self.laser_input(4,casex,casey+1,map_images)
            elif dir==2:
                return self.laser_input(3,casex+1,casey,map_images)
            elif dir==3:
                return self.laser_input(2,casex,casey-1,map_images)
            else:
                return self.laser_input(1,casex-1,casey,map_images)
        elif self.map[casey][casex]==3:
            if dir in (1,4):
                map_images[casey][casex].add("laserd")
                map_images[casey][casex].add("laserh")
                map_images[casey][casex].add("laserdc")
                map_images[casey][casex].add("laserhc")
            else:
                map_images[casey][casex].add("laserg")
                map_images[casey][casex].add("laserb")
                map_images[casey][casex].add("lasergc")
                map_images[casey][casex].add("laserbc")
            if dir==1:
                return self.laser_input(2,casex,casey-1,map_images)
            elif dir==2:
                return self.laser_input(1,casex-1,casey,map_images)
            elif dir==3:
                return self.laser_input(4,casex,casey+1,map_images)
            else:
                return self.laser_input(3,casex+1,casey,map_images)
        elif self.map[casey][casex]==4:
            self.joueurs[self.tour].add_cash(self.cible_price)
            self.nb_tirs_cible+=1
            if self.nb_tirs_cible==10:
                self.nb_tirs_cible=0
                self.cible_price*=7
                self.cible_price+=3
                self.cible_price//=6
            map_images[casey][casex].add("laser"+"dbgh"[dir-1])
            return True
        elif self.map[casey][casex] in (5,6,7,8):
            if self.map[casey][casex]-dir==4:
                if dir==1:
                    map_images[casey][casex].add("laserd")
                    map_images[casey][casex].add("laserdc")
                    map_images[casey][casex].add("laserb")
                    map_images[casey][casex].add("laserbc")
                    return self.laser_input(4,casex,casey+1,map_images)
                elif dir==2:
                    map_images[casey][casex].add("laserg")
                    map_images[casey][casex].add("lasergc")
                    map_images[casey][casex].add("laserb")
                    map_images[casey][casex].add("laserbc")
                    return self.laser_input(1,casex-1,casey,map_images)
                elif dir==3:
                    map_images[casey][casex].add("laserg")
                    map_images[casey][casex].add("lasergc")
                    map_images[casey][casex].add("laserh")
                    map_images[casey][casex].add("laserhc")
                    return self.laser_input(2,casex,casey-1,map_images)
                elif dir==4:
                    map_images[casey][casex].add("laserd")
                    map_images[casey][casex].add("laserdc")
                    map_images[casey][casex].add("laserh")
                    map_images[casey][casex].add("laserhc")
                    return self.laser_input(3,casex+1,casey,map_images)
            elif self.map[casey][casex]-dir in (1,5):
                map_images[casey][casex].add("killed_"+["270","0","90","180"][dir-1])
                self.joueurs[self.tour].add_cash(self.player_price)
                self.joueur_in_case(casex,casey).add_cash(-self.player_price)
                self.nb_tirs_player+=1
                if self.nb_tirs_player==4:
                    self.nb_tirs_player=0
                    self.player_price*=4
                    self.player_price//=3
                if self.joueur_in_case(casex,casey).cash<0:
                    self.joueur_in_case(casex,casey).alive=False
                    self.map[casey][casex]=1
                map_images[casey][casex].add("laser"+"dbgh"[dir-1])
                return True
            elif self.map[casey][casex]-dir in (2,6):
                return True
            elif self.map[casey][casex]-dir in (3,7):
                if dir==1:
                    map_images[casey][casex].add("laserd")
                    map_images[casey][casex].add("laserdc")
                    map_images[casey][casex].add("laserh")
                    map_images[casey][casex].add("laserhc")
                    return self.laser_input(2,casex,casey-1,map_images)
                elif dir==2:
                    map_images[casey][casex].add("laserd")
                    map_images[casey][casex].add("laserdc")
                    map_images[casey][casex].add("laserb")
                    map_images[casey][casex].add("laserbc")
                    return self.laser_input(3,casex+1,casey,map_images)
                elif dir==3:
                    map_images[casey][casex].add("laserg")
                    map_images[casey][casex].add("lasergc")
                    map_images[casey][casex].add("laserb")
                    map_images[casey][casex].add("laserbc")
                    return self.laser_input(4,casex,casey+1,map_images)
                elif dir==4:
                    map_images[casey][casex].add("laserg")
                    map_images[casey][casex].add("lasergc")
                    map_images[casey][casex].add("laserh")
                    map_images[casey][casex].add("laserhc")
                    return self.laser_input(1,casex-1,casey,map_images)
    def joueur_in_case(self,x,y):
        if not self.map[y][x] in (5,6,7,8):
            raise ValueError("aucun joueur ne se trouve ici")
        for joueur in self.joueurs:
            if joueur.x==x and joueur.y==y and joueur.alive:
                return joueur
    def sort(self,map_images):
        r=[]
        for y in range(len(self.map)):
            res=[]
            for x in range(len(self.map[0])):
                result=[]
                case=list(map_images[y][x])
                if "feu" in case and "bois" in case:
                    case.remove("bois")
                if "killed_0" in case or "killed_90" in case or "killed_180" in case or "killed_270" in case:
                    i=0
                    while i<len(case):
                        if case[i].startswith("joueur"):
                            del case[i]
                        else:
                            i+=1
                i=0
                while i<len(case):
                    if "laser" in case[i]:
                        result.append(case.pop(i))
                    else:
                        i+=1
                while i<len(case):
                    if case[i]!="feu":
                        result.append(case.pop(i))
                    else:
                        i+=1
                if len(case)!=0:
                    result.append(case.pop(0))
                res.append("&".join(result))
            r.append("@".join(res))
        players=[]
        for x in self.joueurs:
            if x.alive:
                players.append(x.pseudo+"%~&3%~&!"+str(x.cash)+"%~&3%~&!"+x.color+"%~&3%~&!"+str(int(x==self.joueurs[self.tour])))
            else:
                color="#"+"".join(["{:0>2x}".format(int(x.color[2*y+1:2*y+3],16)//2+128) for y in range(3)])
                players.append(x.pseudo+" (mort)%~&3%~&!%~&3%~&!"+color+"%~&3%~&!0")
        return "#".join(r)+"%~&0%~&!"+"%~&1%~&!".join(players)+"%~&2%~&!"+str(self.cible_price)+"%~&2%~&!"+str(self.player_price)
    def wait(self,joueur,no):
        joueur_no=-1
        for i in range(len(self.joueurs)):
            if self.joueurs[i].id==joueur:
                joueur_no=i
        if joueur_no==-1:
            raise ValueError("Ce joueur ne fait pas partie de cette partie")
        while len(self.events[joueur_no])<=no:
            wait(0.2)
        return self.events[joueur_no][no]
    def mapping(self):
        imgs=["bois",None,"mirroir","mirroirt","cible","joueur_180","joueur_270","joueur_0","joueur_90"]
        result=[]
        for casey in range(len(self.map)):
            result.append([])
            for casex in range(len(self.map[casey])):
                value=imgs[self.map[casey][casex]]
                if type(value)==str and value.startswith("joueur"):
                    value+="_"+self.joueur_in_case(casex,casey).color[1:]
                result[-1].append([value] if value!=None else [])
        return result
    def play(self,coup,player):
        for joueur in self.joueurs:
            if joueur.id==player and not joueur.alive:
                raise ValueError("T'es mort (grosse merde)")
        if self.joueurs[self.tour].id!=player:
            raise ValueError("Ce n'est pas ton tour")
        if not self.can_play:
            raise ValueError("Impossible de jouer pour le moment")
        self.can_play=False
        if coup==0:
            if self.joueurs[self.tour].x==0 or self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x-1]!=1:
                self.can_play=True
                return
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x-1]=self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]=1
            self.joueurs[self.tour].x-=1
        elif coup==1:
            if self.joueurs[self.tour].y==0 or self.map[self.joueurs[self.tour].y-1][self.joueurs[self.tour].x]!=1:
                self.can_play=True
                return
            self.map[self.joueurs[self.tour].y-1][self.joueurs[self.tour].x]=self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]=1
            self.joueurs[self.tour].y-=1
        elif coup==2:
            if self.joueurs[self.tour].x==len(self.map[0]) or self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x+1]!=1:
                self.can_play=True
                return
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x+1]=self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]=1
            self.joueurs[self.tour].x+=1
        elif coup==3:
            if self.joueurs[self.tour].y==len(self.map) or self.map[self.joueurs[self.tour].y+1][self.joueurs[self.tour].x]!=1:
                self.can_play=True
                return
            self.map[self.joueurs[self.tour].y+1][self.joueurs[self.tour].x]=self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]=1
            self.joueurs[self.tour].y+=1
        elif coup==4:
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]+=1
            if self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]==9:
                self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]=5
        elif coup==5:
            self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]-=1
            if self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]==4:
                self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]=8
        elif coup==6:
            dir=self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]-4
            casex=self.joueurs[self.tour].x
            casey=self.joueurs[self.tour].y
            if dir==1:
                casex-=1
            elif dir==2:
                casey-=1
            elif dir==3:
                casex+=1
            elif dir==4:
                casey+=1
            map_images=[]
            for line in self.mapping():
                map_images.append([])
                for case in line:
                    map_images[-1].append(set(case))
            while not self.laser_input(dir,casex,casey,map_images):
                dir=self.map[self.joueurs[self.tour].y][self.joueurs[self.tour].x]-4
                casex=self.joueurs[self.tour].x
                casey=self.joueurs[self.tour].y
                if dir==1:
                    casex-=1
                elif dir==2:
                    casey-=1
                elif dir==3:
                    casex+=1
                elif dir==4:
                    casey+=1
                map_images=[]
                for line in self.mapping():
                    map_images.append([])
                    for case in line:
                        map_images[-1].append(set(case))
            result=self.sort(map_images)
            for evt in self.events:
                evt.append(result)
            wait(1)
        if self.twice:
            self.twice=False
        else:
            self.tour+=1
            self.tour%=len(self.joueurs)
            while not self.joueurs[self.tour].alive:
                self.tour+=1
                self.tour%=len(self.joueurs)
        new_event=self.sort(self.mapping())
        nb_alive=0
        for x in self.joueurs:
            nb_alive+=x.alive
        if nb_alive==1:
            new_event+=("%~&0%~&!"+self.joueurs[self.tour].pseudo+"%~&3%~&!"+self.joueurs[self.tour].color)
        for evt in self.events:
            evt.append(new_event)
        self.can_play=True
    def bonus(self,no,player,pos,dir):
        for joueur in self.joueurs:
            if joueur.id==player and not joueur.alive:
                raise ValueError("T'es mort (grosse merde)")
        if self.joueurs[self.tour].id!=player:
            raise ValueError("Ce n'est pas ton tour")
        if not self.can_play:
            raise ValueError("Impossible de jouer pour le moment")
        prix=[40,25,17,12,21,50,5][no]
        if self.joueurs[self.tour].cash>=prix:
            self.joueurs[self.tour].cash-=prix
        else:
            raise ValueError("Tu n'est pas assez riche")
        if no==0:
            if self.map[pos[1]][pos[0]]==1:
                self.map[pos[1]][pos[0]]=2+dir
            else:
                self.joueurs[self.tour].cash+=prix
        elif no==1:
            if self.map[pos[1]][pos[0]] in (2,3):
                self.map[pos[1]][pos[0]]=1
            else:
                self.joueurs[self.tour].cash+=prix
        elif no==2:
            x=[-1,0,1,0][dir]
            y=[0,-1,0,1][dir]
            dest=(pos[0]+x,pos[1]+y)
            if self.map[pos[1]][pos[0]] in (2,3) and self.map[dest[1]][dest[0]]==1:
                self.map[dest[1]][dest[0]]=self.map[pos[1]][pos[0]]
                self.map[pos[1]][pos[0]]=1
            else:
                self.joueurs[self.tour].cash+=prix
        elif no==3:
            if self.map[pos[1]][pos[0]]==2:
                self.map[pos[1]][pos[0]]=3
            elif self.map[pos[1]][pos[0]]==3:
                self.map[pos[1]][pos[0]]=2
            else:
                self.joueurs[self.tour].cash+=prix
        elif no==4:
            x=[-1,0,1,0][dir]
            y=[0,-1,0,1][dir]
            dest=(pos[0]+x,pos[1]+y)
            if self.map[pos[1]][pos[0]]==4 and self.map[dest[1]][dest[0]]==1:
                self.map[dest[1]][dest[0]]=4
                self.map[pos[1]][pos[0]]=1
            else:
                self.joueurs[self.tour].cash+=prix
        elif no==5:
            if self.twice:
                self.joueurs[self.tour].cash+=prix
            else:
                self.twice=True
        elif no==6:
            if self.map[pos[1]][pos[0]]==1:
                self.map[pos[1]][pos[0]]=0
            else:
                self.joueurs[self.tour].cash+=prix
        for evt in self.events:
            evt.append(self.sort(self.mapping()))
class Joueur:
    def __init__(self,pseudo,color):
        self.id=generate_code(32)
        self.alive=True
        self.pseudo=pseudo
        self.partie=None
        self.x=None
        self.y=None
        self.cash=None
        self.color=color
    def init(self,partie,pos,cash):
        self.partie=partie
        self.x,self.y=pos
        self.cash=cash
    def add_cash(self,cash):
        self.cash+=cash
