## go_static_server
a simple demo of implements static server with c


### CLONE

run this command to clone this  repository.


```bash
git clone git@github.com:dengjiawen8955/c_static_server.git
```

### CONFIG

you must config the base_path in `static_server.c`  or `static_server_epoll.c`

```c
// basePath is where your static files' folder. 
// you should change it in a property path.
char *base_path = "/root/CLionProjects/c_static_server";
//if your port already in used, you also can change it.
int port = 9000;
```


### COMPILE AND RUN

```bash
cd c_satatic_server
# build static_server.c 
gcc  -o main static_server.c
# or you can build 
# run
./main
```


### BROWSER TEST

if you run localhost and test also on localhost


```url
# 
[ip]:[port]/index
# like view a html
localhost:9000/index.html
# like view a image.
localhost:9000/bmft.png
```
