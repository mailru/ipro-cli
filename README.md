# iproto cli tools

## Build
``` sh
git clone https://github.com/mailru/ipro-cli.git
cd ipro-cli
cmake .
```

## Usage examples
``` sh
# ping command example
ipro_printf -m 65280 | socat - tcp:127.0.0.1:6500,shut-none | ipro_scanf
# output: (65280, 0, 0)

# capron profile get command example
ipro_printf -m 14 usus 1 '' 12345 Email | socat - tcp:127.0.0.1:6500,shut-none | ipro_scanf uss
# output: (14, 29, 0) 1, Email, test@test.ru
```
