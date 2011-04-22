#include "nom1.h"

int aggregate_appearance_event(struct event *event) {
  stats.appearances++;

  // key is "url-yyyy-mm-dd". appending strings is a little ugly...
  char key[BUFSIZE];
  int keylength = snprintf(key, BUFSIZE-1, "%s-", event->url);
  strncat(key+keylength, event->occurred_at, 10);
  if (kcdbincrint(appearances, key, strlen(key), 1) == INT64_MIN) {
    fprintf(stderr, "Count not increment count on line %ld: %s\n", stats.lines, kcecodename(kcdbecode(appearances)));
    return 1;
  }
  
  return 0;
}

int aggregate_search_event(struct event *event) {
  stats.searches++;
  return 0;
}

int aggregate_click_event(struct event *event) {
  stats.clicks++;
  return 0;
}

int store_url(struct event *event) {  
  if (kcdbincrint(urls, event->url, strlen(event->url), 1) == INT64_MIN) {
    fprintf(stderr, "store_url error on line %ld: %s\n", stats.lines, kcecodename(kcdbecode(urls)));
    return 1;
  }
  stats.urls = kcdbcount(urls);
  return 0;
}

int aggregate_event(struct event *event) {
  // Now perform processing based on event_type
  if (strcmp("appearance", event->event_type) == 0) {
    return aggregate_appearance_event(event) || store_url(event);
  } else if (strcmp("search", event->event_type) == 0) {
    return aggregate_search_event(event);
  } else if (strcmp("click", event->event_type) == 0)  {
    return aggregate_click_event(event) || store_url(event);
  } else {
   return 1;
  }
}

