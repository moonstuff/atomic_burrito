#include "nom1.h"

struct stats stats;

int main(int argc, char *argv[]) {
  char *appearances_filename = argv[1];
  char *urls_filename = argv[2];
  struct event event;
  char line_buffer[BUFSIZE];

  if (argc != 3) {
    fprintf(stderr, "usage: %s <appearances database> <urls database>\n", argv[0]);
    return 1;
  }

  appearances = kcdbnew();
  if (!kcdbopen(appearances, appearances_filename, KCOWRITER | KCOCREATE)) {
    fprintf(stderr, "appearances database open error: %s\n", kcecodename(kcdbecode(appearances)));
  }

  urls = kcdbnew();
  if (!kcdbopen(urls, urls_filename, KCOWRITER | KCOCREATE)) {
    fprintf(stderr, "url database open error: %s\n", kcecodename(kcdbecode(urls)));
  }

  while (fgets(line_buffer, BUFSIZE-1, stdin)) {
    stats.lines++;
    
    if (parse_event(line_buffer, &event) != 0)
      continue;

    if (aggregate_event(&event) != 0)
      continue;
 }

  fprintf(stderr, "EOF reached: %ld records\n", stats.lines);
  fprintf(stderr, "%ld searches, %ld appearances, %ld clicks\n", stats.searches, stats.appearances, stats.clicks);
 
  if (!kcdbclose(appearances)) {
    fprintf(stderr, "appearances database close error: %s\n", kcecodename(kcdbecode(appearances)));
  }

  if (!kcdbclose(urls)) {
    fprintf(stderr, "url database close error: %s\n", kcecodename(kcdbecode(urls)));
  }

  kcdbdel(appearances);
  kcdbdel(urls);

  return 0;
}



