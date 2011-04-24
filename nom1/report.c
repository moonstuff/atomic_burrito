#include "nom1.h"

#define LINE_WIDTH 100
#define MAX_LINES 31
#define REPORT_BUFFER_LEN (LINE_WIDTH * MAX_LINES + 1)

char report_buffer[REPORT_BUFFER_LEN];

const char *visit_url(const char *url, size_t key_size, const char *value, size_t value_size, size_t *sp, void *opq) {
  // There should be at most 92 days of aggregates for this data set.
  char *url_appearance_keys[MAX_LINES-1];
  long matching_records = kcdbmatchprefix(appearances, url, url_appearance_keys, MAX_LINES-1);
  if (matching_records == 0) return KCVISNOP;
  
  memset(report_buffer, 0, REPORT_BUFFER_LEN);
  int report_cursor = 0;

  // there might already be a report in there, so we want to append if there is.
  size_t existing_report_size = 0;
  char *existing_report = kcdbget(url_reports, url, key_size, &existing_report_size);

  if (existing_report != NULL) {
    strncpy(report_buffer, existing_report, REPORT_BUFFER_LEN - 1);
    report_cursor = strlen(report_buffer);
    kcfree(existing_report);
  }

  for (long i=0; i<matching_records; i++) {
    char *appearance_key = url_appearance_keys[i];
    long appearance_key_value = kcdbincrint(appearances, appearance_key, strlen(appearance_key), 0);
    report_cursor += snprintf(report_buffer+report_cursor, LINE_WIDTH, "%s: %ld\n", appearance_key + strlen(url) + 1, appearance_key_value);
  }

  kcdbset(url_reports, url, key_size, report_buffer, report_cursor);
  stats.reports++;
  return KCVISNOP;
}

int generate_url_reports(char *url_reports_filename) {
  fprintf(stdout, "generating url reports for %ld urls\n", kcdbcount(urls));

  url_reports = kcdbnew();
  if (!kcdbopen(url_reports, url_reports_filename, KCOWRITER | KCOCREATE)) {
    fprintf(stderr, "url reports database open error: %s\n", kcecodename(kcdbecode(url_reports)));
  }

  if (!kcdbiterate(urls, visit_url, NULL, 0)) {
    fprintf(stderr, "error iterating over url database: %s\n", kcecodename(kcdbecode(urls)));
  }
  
  if (!kcdbclose(url_reports)) {
    fprintf(stderr, "url reports database close error: %s\n", kcecodename(kcdbecode(url_reports)));
  }

  kcdbdel(url_reports);

  return 0;
}

