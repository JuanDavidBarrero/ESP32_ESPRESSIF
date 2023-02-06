# Important

If you wante to update the certificates, you should use the follow commands
```
openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 3650 -out cacert.crt -subj "/CN=ESP32 HTTPS server example"
```

In order to the content of the certificate use the follow command 
```
openssl x509 -in .\cacert.crt -text -noout
```