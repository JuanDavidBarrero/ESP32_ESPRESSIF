# Important

In order to use a global mqtts server, we chose to use the [mosquitto](https://test.mosquitto.org/)  broker which also provides an encryption system,, to be able to use it it is necessary to create a private key and an unsigned certificate with the following commands

```
openssl genrsa -out private.key
openssl req -out client.csr -key private.key -new
```

in order to validate the certificate you must go to the mosquitto validator [ssl](https://test.mosquitto.org/ssl/) and upload the certificate, now you only need to download the mosquitto certificate from the previous page.

## Tree Folder 

```
    - client.crt
    - client.csr
    - mosquitto.org.crt
    - private.key
```