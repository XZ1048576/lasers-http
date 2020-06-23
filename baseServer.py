import socket
from threading import Thread
class Server(Thread):
    def __init__(self,a=None):
        if a is None:
            a=socket.socket()
            a.bind(("",80))
            a.listen(50)
        self.a=a
        Thread.__init__(self)
    def redirect(self,path,version):
        self.cnx.send(version+b" 302 Found\r\nLocation: "+path+b"\r\n\r\n")
        self.cnx.close()
    def run(self):
        self.cnx,infos=self.a.accept()
        type(self)(self.a).start()
        try:
            request=self.cnx.recv(1024)
            while not b"\r\n\r\n" in request:
                request+=self.cnx.recv(1024)
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
            self.request(method,path,parameters,headers,body,version)
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
            self.cnx.send(version+b" 500 Internal Server Error\r\n\r\n"+error.encode())
            self.cnx.close()
    def request(self,method,path,parameters,headers,body,version):
        raise NotImplementedError
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
