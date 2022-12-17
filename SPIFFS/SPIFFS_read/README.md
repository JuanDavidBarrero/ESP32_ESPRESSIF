# How to use 

to generate de key files use the follow commands

```
openssl genrsa -out private.pem
openssl rsa -in .\private.pem -pubout -out public.pub
```
then create a folder called "data" to store the files and create a new partition for the 
spiffs memory