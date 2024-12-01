from threading import Thread
from socket import timeout
from random import randrange
from time import sleep as wait
from math import inf
class Websocket(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.cnx=None
        self.event=None
        self.onclose=None
        self.onconnect=None
        self.ondisconnect=None
        self.continuer=True
        self.closing=False
        self.offline_time=0

    def run(self):
        while self.continuer:
            while self.cnx==None:
                wait(1)
                self.offline_time+=1
            self.cnx.settimeout(5)
            recieving=None
            self.offline_time=0
            if self.onconnect!=None:
                self.onconnect()
            while self.cnx!=None:
                try:
                    h=self.cnx.recv(2)
                except timeout:
                    ping=bytes([randrange(256) for _ in range(10)])
                    self._send(ping,9)
                    try:
                        h=self.cnx.recv(2)
                    except timeout: #didn't recive pong
                        h=b""
                except ConnectionResetError: #connection lost
                    self.cnx=None
                    if self.onclose!=None:
                        self.onclose((1006,b""),self)
                    break
                if h==b"": #connection lost too
                    self.cnx=None
                    if self.onclose!=None:
                        self.onclose((1006,b""),self)
                    break
                if h[1]&0x7f<126:
                    data_len=h[1]&0x7f
                else:
                    data_len=cnx.recv(2 if h[1]&0x7f==126 else 8)
                    if data_len==b"": #connection lost
                        self.cnx=None
                        if self.onclose!=None:
                            self.onclose((1006,b""),self)
                        break
                    data_len=int.from_bytes(data_len,"big")
                if h[1]&0x80: #mask
                    mask=self.cnx.recv(4)
                else:
                    mask=bytes(4)
                data=b""
                while len(data)<data_len:
                    if data_len-len(data)>1024:
                        recv_data=self.cnx.recv(1024)
                    else:
                        recv_data=self.cnx.recv(data_len-len(data))
                    if recv_data==b"": #connection lost
                        self.cnx=None
                        if self.onclose!=None:
                            self.onclose((1006,b""),self)
                        break
                    data+=recv_data
                if len(data)<data_len:
                    break
                data=list(data)
                for i in range(data_len):
                    data[i]^=mask[i%4]
                data=bytes(data)
                cmd=h[0]&0x0f
                if cmd==0:
                    if recieving==None:
                        self.cnx.close()
                        self.cnx=None
                        if self.onclose!=None:
                            self.onclose((1006,b""),self)
                        break
                    else:
                        recieving[1]+=data
                        if h[0]&0x80:
                            cmd=recieving[0]
                            data=recieving[1]
                            recieving=None
                elif h[0]&0x80==0:
                    recieving=[cmd,data]
                    continue
                if cmd==1 or cmd==2:
                    if self.event:
                        self.event(data,self)
                elif cmd==8:
                    if not self.closing:
                        self._send(data,8)
                    self.cnx.close()
                    self.cnx=None
                    if self.onclose:
                        if len(data)>=2:
                            code=int.from_bytes(data[:2],"big")
                            reason=data[2:]
                        else:
                            code=1005
                            reason=b""
                        self.onclose((code,reason),self)
                elif cmd==9:
                    self._send(data,10)
                elif cmd==10 and data==ping:
                    ping=None
                elif cmd!=0: #unknown
                    self.cnx.close()
                    self.cnx=None
                    if self.onclose!=None:
                        self.onclose((1006,b""),self)
            self.offline_time=1
            if self.ondisconnect!=None:
                self.ondisconnect()
        self.offline_time=inf

    def _send(self,data,cmd):
        if self.cnx==None:
            return
        payload=bytes([0x80+cmd])
        if len(data)<=125:
            payload+=bytes([len(data)])
        elif len(data)<(1<<16):
            payload+=b"\x7e"
            payload+=len(data).to_bytes(2,"big")
        elif len(data)<(1<<63):
            payload+=b"\x7f"
            payload+=len(data).to_bytes(8,"big")
        payload+=data
        self.cnx.send(payload)

    def send(self,data):
        self._send(data,1)

    def close(self,code=None,reason=b""):
        if type(code)==int:
            code=code.to_bytes(2,"big")
        elif code==None:
            code=b""
        if type(reason)==str:
            reason=reason.encode()
        self._send(code+reason,8)
        self.closing=True

    def stop(self):
        self.continuer=0
