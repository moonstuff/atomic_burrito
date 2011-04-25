#include "nom1.h"

struct stats stats = {0, 0, 0, 0, 0, 0};

int main(int argc, char *argv[]) {
  char *url_reports_filename = argv[1];
  char *derpderpderp = ""; // Why is this required? WHY WHY WHY?!?!?!?!?
  struct event event;
  char line_buffer[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr, "usage: %s <url reports database>\n", argv[0]);
    return 1;
  }

  appearances = kcdbnew();
  if (!kcdbopen(appearances, "+", KCOWRITER | KCOCREATE)) {
    fprintf(stderr, "appearances database open error: %s\n", kcecodename(kcdbecode(appearances)));
  }

  urls = kcdbnew();
  if (!kcdbopen(urls, "+", KCOWRITER | KCOCREATE)) {
    fprintf(stderr, "url database open error: %s\n", kcecodename(kcdbecode(urls)));
  }

  while (fgets(line_buffer, BUFSIZE-1, stdin)) {
    stats.lines++;
    
    if (parse_event(line_buffer, &event) != 0)
      continue;

    if (aggregate_event(&event) != 0)
      continue;
  }
  fprintf(stderr, "EOF reached: %ld records: \n", stats.lines);
  fprintf(stderr, "%ld searches, %ld appearances, %ld clicks, %ld urls\n", stats.searches, stats.appearances, stats.clicks, stats.urls);

  generate_url_reports(url_reports_filename);
 
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



