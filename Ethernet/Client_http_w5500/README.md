# Important 

In order to use the https module **WITHOUT SECURITY** must be selec the follow options
this only should be use as example 

```
ESP_TLS
    - Allow potentially insecure options
    - Skip server certificate verification by default (WARNING: ONLY FOR TESTING PURPOSE, READ HELP)
```

and to use all the code must update the main **main task stack size**

```
ESP System Settings
    - Main task stack size to 6144
```

