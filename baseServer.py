import socket
from threading import Thread
from http.server import HTTPStatus
from mimetypes import guess_type
from time import strftime
codes={}
for x in HTTPStatus:
    codes[x.value]=x.name.encode()
class Server(Thread):
    def __init__(self,a=None):
        if a is None:
            a=socket.socket(socket.AF_INET6)
            a.setsockopt(socket.IPPROTO_IPV6,socket.IPV6_V6ONLY,0)
            a.bind(("",34080))
            a.listen(50)
        self.a=a
        Thread.__init__(self)
    def redirect(self,path):
        self.response(302,{b"Location":path})
    def run(self):
        try:
            self.cnx,infos=self.a.accept()
        except:
            type(self)(self.a).start()
            return
        type(self)(self.a).start()
        try:
            request=self.cnx.recv(1024)
            while not b"\r\n\r\n" in request:
                folow=self.cnx.recv(1024)
                if folow==b"":
                    return
                request+=folow
            request=request.split(b"\n")
            if len(request[0].split(b" "))!=3:
                self.default_response(b"HTTP/1.1",400)
                return
            method,url,version=request[0].split(b" ")
            if version.endswith(b"\r"):
                version=version[:-1]
            parameters={}
            if len(url.split(b"?"))==1:
                path=url_decode(url)
            else:
                path=url_decode(url.split(b"?")[0])
                for x in url.split(b"?")[1].split(b"&"):
                    if len(x.split(b"="))!=2:
                        self.default_response(version,400)
                        return
                    parameters[url_decode(x.split(b"=")[0])]=url_decode(x.split(b"=")[1])
            headers={}
            i=1
            while i<len(request):
                if request[i] in (b"",b"\r"):
                    break
                headers[request[i].split(b":")[0]]=b":".join(request[i].split(b":")[1:])
                while headers[request[i].split(b":")[0]].startswith(b" "):
                    headers[request[i].split(b":")[0]]=headers[request[i].split(b":")[0]][1:]
                while headers[request[i].split(b":")[0]].endswith(b" ") or headers[request[i].split(b":")[0]].endswith(b"\r"):
                    headers[request[i].split(b":")[0]]=headers[request[i].split(b":")[0]][:-1]
                i+=1
            request=b"\n".join(request)
            if b"Content-Lenght" in headers:
                ttl=request.find(b"\r\n\r\n")+4+int(headers[b"Content-Length"].decode())
                while len(request)<ttl:
                    request+=self.cnx.recv(1024)
            body=request.split(b"\r\n\r\n")[1]
            if path.endswith(b"/"):
                path+=b"index.html"
            self.version=version
            self.file_type=guess_type(path.decode())[0]
            self.request(method,path,parameters,headers,body)
        except Exception as e:
            error="Traceback (most recent call last):\n"
            a=e.__traceback__
            while a!=None:
                error+="  File \""
                error+=a.tb_frame.f_code.co_filename
                error+="\", line "
                error+=str(a.tb_lineno)
                error+=", in "
                error+=a.tb_frame.f_code.co_name
                error+="\n"
                a=a.tb_next
            error+=type(e).__name__
            error+=": "
            error+=str(e)
            self.cnx.send(version+b" 500 Internal Server Error\r\n\r\n<h1>Erreur 500</h1><h2>Une erreur est survenue.</h2>")
            self.cnx.close()
            with open('error.log','a') as error_log:
                error_log.write(strftime("%Y-%m-%d %H:%M:%S UTC%z")+"\n"+error+"\n\n")
    def request(self,method,path,parameters,headers,body):
        raise NotImplementedError
    def response(self,status,headers,body=b"",close=True):
        response_=self.version+b" "+str(status).encode()+b" "+codes[status]+b"\r\n"
        if body:
            if not b"Content-Length" in headers:
                headers[b"Content-Length"]=str(len(body)).encode()
            if not b"Content-Type" in headers and self.file_type:
                headers[b"Content-Type"]=self.file_type.encode()
        for header in headers:
            response_+=header
            response_+=b": "
            response_+=headers[header]
            response_+=b"\r\n"
        response_+=b"\r\n"
        response_+=body
        self.cnx.send(response_)
        if close:
            self.cnx.close()
def url_decode(url):
    url=list(url)
    i=0
    while i<len(url):
        if url[i]==37:
            try:
                url[i]=int(bytes(url[i+1:i+3]).decode(),16)
            except:
                raise ValueError("Url invalide")
            del url[i+1]
            del url[i+1]
        i+=1
    return bytes(url)
