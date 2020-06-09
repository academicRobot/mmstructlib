#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(__sgi) /* most probably only IRIX 5.2 needs this */
#include <sys/uio.h>
#include <sys/time.h>
#endif
#include <sys/types.h>
#include <unistd.h>


#include "util.h"


static const char *cmd_path = "/usr/local/bin/sginfo";
static const char *cmd_name =                "sginfo";


static void chr2html(int c)
{
  switch (c)
  {
    case '&': fprintf(stdout, "&amp;");  break;
    case '<': fprintf(stdout, "&lt;");   break;
    case '>': fprintf(stdout, "&gt;");   break;
    case '"': fprintf(stdout, "&quot;"); break;
    default:
      putc(c, stdout); break;
  }
}


static void str2html(const char *s)
{
  while (*s) chr2html(*s++);
}


static void htmlHead(void)
{
  fprintf(stdout, "Content-type: text/html\n\n");

  fprintf(stdout, "<html>\n");
  fprintf(stdout, "<head><title>sginfo gateway</title></head>\n");
  fprintf(stdout, "<body><h2>SgInfo - Space group Info</h2><hr>\n");
}


static void htmlTail(void)
{
  fprintf(stdout, "<hr>\n");
  fprintf(stdout, "<address>\n");
  fprintf(stdout, "<a href=\"http://xplor.csb.yale.edu/~rwgk\"\n");
  fprintf(stdout, ">Ralf W. Grosse-Kunstleve</a>\n");
  fprintf(stdout, "&lt;<a href=\"mailto:rwgk@laplace.csb.yale.edu\"\n");
  fprintf(stdout, "                   >rwgk@laplace.csb.yale.edu</a>&gt;\n");
  fprintf(stdout, "</address>\n");
  fprintf(stdout, "</body>\n");
  fprintf(stdout, "</html>\n");
}


static void SendError(const char *msg)
{
  fprintf(stdout, "<h2>");
  str2html(msg);
  fprintf(stdout, "</h2>\n");
  htmlTail();
  exit(1);
}


static const char *copyarg(const char *s, char **arg)
{
  int         n, b, q;
  char        *a;
  const char  *p;


  if (arg) *arg = NULL;

  a = NULL;

  for (;;)
  {
    p = s;
    n = 0;
    b = 0;
    q = '\0';

    for (;;)
    {
      if (b)
      {
        if (   isspace(*p) == 0
            && *p != '"'
            && *p != '\''
            && *p != '\\')
        {
          if (a) *a++ = '\\';
          n++;
        }

        if (a) *a++ = *p;
        n++;

        b = 0;
      }
      else if (*p == '\\')
        b = 1;

      else if (q)
      {
        if (*p == q)
          q = '\0';
        else
        {
          if (a) *a++ = *p;
          n++;
        }
      }
      else if (   *p == '"'
               || *p == '\'')
        q = *p;

      else if (isspace(*p) == 0)
      {
        if (a) *a++ = *p;
        n++;
      }
      else
        break;

      p++;

      if (*p == '\0')
      {
        if (q)
          SendError("Unmatched quote.");

        if (b)
        {
          if (a) *a++ = '\\';
          n++;
        }

        break;
      }
    }

    if (a) *a = '\0';
    n++;

    if (arg == NULL || *arg) break;

        (*arg) = malloc(n * sizeof (**arg));
    if ((*arg) == NULL)
      SendError("Server Error: Not enough core");

    a = (*arg);
  }

  return p;
}


static char **mkargv(const char *s)
{
  int         argc;
  char        **argv;
  const char  *p;


  argv = NULL;

  for (;;)
  {
    argc = 1;

    p = s;

    for (;;)
    {
      while (*p && isspace(*p)) p++;

      if (*p == '\0') break;

      if (argv == NULL)
        p = copyarg(p, NULL);
      else
        p = copyarg(p, &argv[argc]);

      argc++;
    }

    if (argv) break;

        argv = malloc((argc + 1) * sizeof (*argv));
    if (argv == NULL)
      SendError("Server Error: Not enough core");

    argv[0] = (char *) cmd_name;
  }

  argv[argc] = NULL;

  return argv;
}


static int run_cmd(char *const *argv)
{
  int    n, i;
  char   buf[128];

  int    pio[2];
  pid_t  pid;


  if (pipe(pio) != 0) return -1;

      pid = fork();
  if (pid < 0) return -1;

  if (pid == 0)
  {
    close(pio[0]);

    close(1); i = dup(pio[1]);

    if (i != 1)
    {
      sprintf(buf, "Server Error: dup() failed\n");
      write(pio[1], buf, strlen(buf) + 1);
      close(pio[1]);
      exit(1);
    }

    close(pio[1]);

    close(2); i = dup(1);

    if (i != 2)
    {
      fprintf(stdout, "Server Error: dup() failed\n");
      fflush(stdout);
      close(1);
      exit(1);
    }

    (void) execv(cmd_path, argv);

    fprintf(stdout, "Server Error: execv() failed\n");
    fflush(stdout);
    close(1);
    exit(1);
  }
  else
  {
    close(pio[1]);

    while(n = read(pio[0], buf, sizeof buf))
      for (i = 0; i < n; i++)
        chr2html(buf[i]);

    close(pio[0]);
  }

  return 0;
}


typedef struct
  {
    char  *name;
    char  *val;
  }
  T_Entry;


int main(void)
{
  int         cl, i;
  char        *env, **run_cmd_argv;
  T_Entry     Entry;


  htmlHead();

      env = getenv("REQUEST_METHOD");
  if (env == NULL || strcmp(env,"POST"))
    SendError("This script should be referenced with a METHOD of POST.");

      env = getenv("CONTENT_TYPE");
  if (env == NULL || strcmp(env,"application/x-www-form-urlencoded"))
    SendError("This script can only be used to decode form results.");

      env = getenv("CONTENT_LENGTH");
  if (env && (cl = atoi(env)))
  {
    Entry.val = fmakeword(stdin,'&',&cl);
    plustospace(Entry.val);
    unescape_url(Entry.val);
    Entry.name = makeword(Entry.val,'=');

  }
  else
    Entry.val = "";

  fprintf(stdout, "<form method=\"post\"");
  fprintf(stdout, " action=\"/user-cgi-bin/sginfo-query\">\n");
  fprintf(stdout, "<code><strong>sginfo </strong></code><input\n");
  fprintf(stdout, "name=\"argv\" size=60\n");
  fprintf(stdout, "value=\"");

  str2html(Entry.val);

  fprintf(stdout, "\"><p>\n");
  fprintf(stdout, "<input type=\"submit\" value=\"Run sginfo\">\n");
  fprintf(stdout, "<input type=\"reset\"  value=\"Reset\">\n");
  fprintf(stdout, "<strong><a href=\"/sginfo/\">");
  fprintf(stdout, "On-line Documentation</a></strong>\n");
  fprintf(stdout, "</form>\n");
  fprintf(stdout, "<hr>\n");

  run_cmd_argv = mkargv(Entry.val);

  fprintf(stdout, "<pre>\n");
  fflush(stdout);

  i = run_cmd(run_cmd_argv);

  fprintf(stdout, "</pre>\n");

  if (i != 0)
    SendError("Server Error: Unable to execute command");

  htmlTail();

  exit(0);
  return 0;
}
