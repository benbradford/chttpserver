Example Usage:
```
    HttpServer serv;
    server_init(&serv);
    server_createBindAndListen(&serv, 8081);
    server_registerHttpFunction(&serv, HTTP_GET, "echoRequest", echoRequest);
    server_acceptLoop(&serv);
    server_free(&serv);
```
Call with:
```
curl -v  "localhost:8081/echoRequest?p2=p2val&p1=p1val&p3=p3val" -X GET -d '{"productId": 123456, "quantity": 100}'  -H 'header1: h1val' -H 'header2:hsval' -H 'header3: h3val'
< HTTP/1.1 200 Success
< boundHttpMethod: GET
< path: echoRequest
< body: {"productId": 123456, "quantity": 100}
< p2: p2val
< p1: p1val
< p3: p3val
< Host: localhost:8081
< User-Agent: curl/8.1.2
< Accept: */*
< header1: h1val
< header2: hsval
< header3: h3val
< Content-Length: 38
< Content-Type: application/x-www-form-urlencoded
< Content-Length: 12
<
200 Success
```