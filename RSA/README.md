# !Important¡

Must change stack size on 

```
ESP System Settings
    - Main task stack size to 5120
```

## Data

if you want to store the private key and public key into the flash memory you can read it with the follow commands 

```
public key
   mbedtls_pk_parse_public_key(&pk, (const unsigned char *)publicKey, publicKeySize)
private key
   mbedtls_pk_parse_key(&pk,(const unsignet char*)privatekey, privatekeySize, "","")    

```
