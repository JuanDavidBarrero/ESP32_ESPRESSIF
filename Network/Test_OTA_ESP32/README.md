#Important 

In order to use the https module WITHOUT SECURITY must be selec the follow options this only should be use as example

```
ESP TLS
    - Allow potentially insecure options
    - Skip server certificate verification by default (WARNING: ONLY FOR TESTING PURPOSE, READ HELP)

PARTITION TABLE
    - Factory app, two OTA definitions

ESP HTTPS OTA 
    - Allow http for OTA (this should be use just for   testing porpuses)

```

For this example i also made a server where is stored the new firmware that you want to place in the esp32 but this can be any other page that has this service, if you want to run the index code inside the server folder with the following command

````
node index.js
```