#include "../header/httpd.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
//#include <sys/stat.h>
//#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_CONNECTIONS 1000
#define CHUNK_SIZE 2048

static int listenfd;
int *clients;
static void error(char *);
static void start_server(const char *);
static void respond(int);

static int clientfd;

static char *buf;

int recv_timeout(int s, int timeout, char *buf)
{
  int size_recv, total_size = 0;
  struct timeval begin, now;
  char chunk[CHUNK_SIZE];
  double timediff;

  // make socket non blocking
  fcntl(s, F_SETFL, O_NONBLOCK);

  // beginning time
  gettimeofday(&begin, NULL);
  int cnt = 0;
  while (1)
  {
    gettimeofday(&now, NULL);

    // time elapsed in seconds
    timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);

    // if you got some data, then break after timeout
    if (total_size > 0 && timediff > timeout)
    {
      break;
    }

    // if you got no data at all, wait a little longer, twice the timeout
    else if (timediff > timeout * 2)
    {
      break;
    }
    // strcpy(buf + cnt * CHUNK_SIZE, chunk);
    // fprintf(stderr, "%s",chunk);
    memset(chunk, '\0', CHUNK_SIZE); // clear the variable
    if ((size_recv = recv(s, chunk, CHUNK_SIZE, 0)) < 0)
    {
      // if nothing was received then we want to wait a little before trying again, 0.1 seconds
      usleep(100000);
    }
    else
    {

      total_size += size_recv;
      
      // fprintf(stderr, "%d\n", cnt);
      // fprintf(stderr, "%s", chunk);
      strcpy(buf + cnt * CHUNK_SIZE, chunk);
      fprintf(stderr, "%s", buf + cnt * CHUNK_SIZE);
      cnt++;
      // printf("%s", chunk);
      // reset beginning time
      gettimeofday(&begin, NULL);
    }
  }

  return total_size;
}

void serve_forever(const char *PORT)
{
  struct sockaddr_in clientaddr;
  socklen_t addrlen;
  //  char c;

  int slot = 0;

  printf("Server started %sport:%s%s\n", "\033[92m", PORT, "\033[0m");

  // create shared memory for client slot array
  clients = mmap(NULL, sizeof(*clients) * MAX_CONNECTIONS,
                 PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

  // Setting all elements to -1: signifies there is no client connected
  int i;
  for (i = 0; i < MAX_CONNECTIONS; i++)
    clients[i] = -1;
  start_server(PORT);

  // Ignore SIGCHLD to avoid zombie threads
  signal(SIGCHLD, SIG_IGN);

  // ACCEPT connections
  while (1)
  {
    addrlen = sizeof(clientaddr);
    clients[slot] = accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);

    if (clients[slot] < 0)
    {
      perror("accept() error");
      exit(1);
    }
    else
    {
      if (fork() == 0)
      {
        close(listenfd);
        respond(slot);
        close(clients[slot]);
        exit(0);
      }
      else
      {
        close(clients[slot]);
      }
    }

    while (clients[slot] != -1)
      slot = (slot + 1) % MAX_CONNECTIONS;
  }
}
// -----------------------------------------------------------------------------
// start server
void start_server(const char *port)
{
  struct addrinfo hints, *res, *p;

  // getaddrinfo for host
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo(NULL, port, &hints, &res) != 0)
  {
    perror("getaddrinfo() error");
    exit(1);
  }
  // socket and bind
  for (p = res; p != NULL; p = p->ai_next)
  {
    int option = 1;
    listenfd = socket(p->ai_family, p->ai_socktype, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (listenfd == -1)
      continue;
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
      break;
  }
  if (p == NULL)
  {
    perror("socket() or bind()");
    exit(1);
  }

  freeaddrinfo(res);

  // listen for incoming connections
  if (listen(listenfd, 1000000) != 0)
  {
    perror("listen() error");
    exit(1);
  }
}
// -----------------------------------------------------------------------------
// get request header by name
char *request_header(const char *name)
{
  header_t *h = reqhdr;
  while (h->name)
  {
    if (strcmp(h->name, name) == 0)
      return h->value;
    h++;
  }
  return NULL;
}
// -----------------------------------------------------------------------------
// get all request headers
header_t *request_headers(void)
{
  return reqhdr;
}
// -----------------------------------------------------------------------------
// client connection
void respond(int n)
{
  int rcvd;
  //  fd;
  //  , bytes_read;
  //  char *ptr;
  int buf_size = 10000000;

  buf = malloc(buf_size);
  memset(buf, '\0', buf_size);
  // rcvd = recv(clients[n], buf, buf_size, 0);

  rcvd = recv_timeout(clients[n], 1, buf);
  // fprintf(stderr, "%s", buf);

  if (rcvd < 0) // receive error
  {
    fprintf(stderr, ("recv() error\n"));
  }
  else
  {
    if (rcvd == 0)
    { // receive socket closed
      fprintf(stderr, "Client disconnected upexpectedly.\n");
    }
    else // message received
    {
      buf[rcvd] = '\0';

      method = strtok(buf, " \t\r\n");
      uri = strtok(NULL, " \t");
      prot = strtok(NULL, " \t\r\n");

      fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);

      qs = strchr(uri, '?');

      if (qs)
      {
        *qs++ = '\0'; // split URI
      }
      else
      {
        qs = uri - 1; // use an empty string
      }

      header_t *h = reqhdr;
      char *t, *t2;
      while (h < reqhdr + 16)
      {
        char *k, *v;
        //      *t;

        k = strtok(NULL, "\r\n: \t");
        if (!k)
          break;

        v = strtok(NULL, "\r\n");
        while (*v && *v == ' ')
          v++;

        h->name = k;
        h->value = v;
        h++;
        //      fprintf(stderr, "[H] %s: %s\n", k, v);
        t = v + 1 + strlen(v);
        if (t[1] == '\r' && t[2] == '\n')
          break;
      }
      t = strtok(NULL, "\r\n");              // now the *t shall be the beginning of user payload
      t2 = request_header("Content-Length"); // and the related header if there is
      payload = t;

      payload_size = t2 ? atol(t2) : (rcvd - (t - buf));

      // bind clientfd to stdout, making it easier to write
      clientfd = clients[n];
      dup2(clientfd, STDOUT_FILENO);
      close(clientfd);

      // call router
      route();

      // tidy up
      fflush(stdout);
      shutdown(STDOUT_FILENO, SHUT_WR);
      close(STDOUT_FILENO);
    }
  }
  // Closing SOCKET
  shutdown(
      clientfd,
      SHUT_RDWR); // All further send and recieve operations are DISABLED...
  close(clientfd);
  clients[n] = -1;

  free(buf);
}