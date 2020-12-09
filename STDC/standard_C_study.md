如何在引号里面引用宏_水晶鞋-CSDN博客_sas引号里调用宏变量 - https://blog.csdn.net/mos2046/article/details/25741223

```
#define SKYBELL_FWVER	8850
#define __SYKBELL_VERSION(ver) "SKYBELL_APP_VER:"#ver""
#define SYKBELL_VERSION(ver)  __SYKBELL_VERSION(ver)
const char* SKYBELL_FWVER_STR=SYKBELL_VERSION(SKYBELL_FWVER);
```

```

```

