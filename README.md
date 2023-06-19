# Simple SHA256 distributed PoW

## SO.....

32bit 난이도를 깨는데 64bit 의 nonce를 사용한다면 대략
$10^(-10^9.270744172598489)$ 정도의 확률로 깨지 못함.
그렇게 나쁜 확률이 아님! 

## AIO

```
gcc -o bin/worker worker/main.c -lssl -lcrypto
gcc -o bin/spawner spawner/main.c -lssl -lcrypto
cd bin
`./spawner hida 8`
```

## To build


Worker`

```
gcc -o bin/worker worker/main.c -lssl -lcrypto
```

Spawner

```
gcc -o bin/spawner spawner/main.c
``` 