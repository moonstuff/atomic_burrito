#include "nom1.h"

#define LINE_WIDTH 100
#define MAX_LINES 100
#define REPORT_BUFFER_LEN (LINE_WIDTH * MAX_LINES + 1)

char report_buffer[REPORT_BUFFER_LEN];

const char *visit_url(const char *url, size_t key_size, const char *value, size_t value_size, size_t *sp, void *opq) {
  // There should be at most 92 days of aggregates for this data set.
  char *url_appearance_keys[MAX_LINES-1];
  long matching_records = kcdbmatchprefix(appearances, url, url_appearance_keys, MAX_LINES-1);
  if (matching_records == 0) return KCVISNOP;
  
  memset(report_buffer, 0, REPORT_BUFFER_LEN);

  int report_cursor = snprintf(report_buffer, LINE_WIDTH, "%s\n", url);
  for (long i=0; i<matching_records; i++) {
    char *appearance_key = url_appearance_keys[i];
    long appearance_key_value = kcdbincrint(appearances, appearance_key, strlen(appearance_key), 0);
    report_cursor += snprintf(report_buffer+report_cursor, LINE_WIDTH, "%s: %ld\n", appearance_key + strlen(url) + 1, appearance_key_value);
  }

  kcdbset(url_reports, url, key_size, report_buffer, report_cursor);
  stats.reports++;
  return KCVISNOP;
}

int generate_url_reports() {
  fprintf(stdout, "generating url reports for %ld urls\n", kcdbcount(urls));
  if (!kcdbiterate(urls, visit_url, NULL, 0)) {
    fprintf(stderr, "error iterating over url database: %s\n", kcecodename(kcdbecode(urls)));
  }
  return 0;
}

