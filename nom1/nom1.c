#include "nom1.h"

struct stats stats = {0, 0, 0, 0, 0, 0};

int main(int argc, char *argv[]) {
  char *appearances_filename = argv[1];
  char *urls_filename = argv[2];
  char *url_reports_filename = argv[3];
  struct event event;
  char line_buffer[BUFSIZE];

  if (argc != 4) {
    fprintf(stderr, "usage: %s <appearances database> <urls database> <url reports database>\n", argv[0]);
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

  url_reports = kcdbnew();
  if (!kcdbopen(url_reports, url_reports_filename, KCOWRITER | KCOCREATE)) {
    fprintf(stderr, "url reports database open error: %s\n", kcecodename(kcdbecode(url_reports)));
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

  generate_url_reports();
 
  if (!kcdbclose(appearances)) {
    fprintf(stderr, "appearances database close error: %s\n", kcecodename(kcdbecode(appearances)));
  }

  if (!kcdbclose(urls)) {
    fprintf(stderr, "url database close error: %s\n", kcecodename(kcdbecode(urls)));
  }

  if (!kcdbclose(url_reports)) {
    fprintf(stderr, "url reports database close error: %s\n", kcecodename(kcdbecode(url_reports)));
  }

  kcdbdel(appearances);
  kcdbdel(urls);
  kcdbdel(url_reports);

  return 0;
}



