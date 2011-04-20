#include "nom1.h"

// Yeah, parsing input... this should do the trick...
#define BUFSIZE 32768
char line_buffer[BUFSIZE];
char *delimiter = "\t"; 

// DB access fun
KCDB *appearances;
KCDB *urls;

// Yeah this is pretty kooky but it is simple and fun.
// An "event" is just pointers to strings inside buffer.
struct {
  char *session;
  char *user_id;
  char *occurred_at;
  char *event_type;
  char *query;
  char *url;
  char *ranking;
} event;

// Lots of lovely stats.
struct {
  long lines;
  long searches;
  long clicks;
  long appearances;
} stats = { 0, 0, 0, 0 };

int aggregate_appearance_event() {
  stats.appearances++;

  // key is "url-appearance-yyyy-mm-dd". appending strings is a little ugly...
  char key[BUFSIZE];
  int keylength = snprintf(key, BUFSIZE-1, "%s-appearance-", event.url);
  strncat(key+keylength, event.occurred_at, 10);
  if (kcdbincrint(appearances, key, strlen(key), 1) == INT64_MIN) {
    fprintf(stderr, "Count not increment count on line %ld: %s\n", stats.lines, kcecodename(kcdbecode(appearances)));
    return 1;
  }
  
  return 0;
}

int aggregate_search_event() {
  stats.searches++;
  return 0;
}

int aggregate_click_event() {
  stats.clicks++;
  return 0;
}

int store_url() {  
  if (!kcdbset(urls, event.url, strlen(event.url), NULL, 0)) {
    fprintf(stderr, "store_url error on line %ld: %s\n", stats.lines, kcecodename(kcdbecode(urls)));
    return 1;
  }
  return 0;
}

int aggregate_event() {
  // Now perform processing based on event_type
  if (strcmp("appearance", event.event_type) == 0) {
    return aggregate_appearance_event() || store_url();
  } else if (strcmp("search", event.event_type) == 0) {
    return aggregate_search_event();
  } else if (strcmp("click", event.event_type) == 0)  {
    return aggregate_click_event() || store_url();
  } else {
   return 1;
  }
}


int parse_event() {
  event.session = strtok(line_buffer, delimiter);
  if (!event.session) {
    fprintf(stderr, "No session provided in line %ld\n", stats.lines);
    return 1;
  }

  event.user_id = strtok(NULL, delimiter);
  if (!event.user_id) {
    fprintf(stderr, "No user_id provided in line %ld\n", stats.lines);
    return 1;
  }
  
  event.occurred_at = strtok(NULL, delimiter);
  if (!event.occurred_at) {
    fprintf(stderr, "No occurred_at provided in line %ld\n", stats.lines);
    return 1;
  }

  event.event_type = strtok(NULL, delimiter);
  if (!event.event_type) {
    fprintf(stderr, "No event_type provided in line %ld\n", stats.lines);
    return 1;
  }

  // Continue parsing based on eventType
  if (strcmp("appearance", event.event_type) == 0 || strcmp("click", event.event_type) == 0)  {
    // Both these types have a URL and ranking
    event.url = strtok(NULL, delimiter);
    if (!event.url) {
      fprintf(stderr, "No url provided in line %ld\n", stats.lines);
      return 1;
    }

    event.ranking = strtok(NULL, delimiter);
    if (!event.ranking) {
      fprintf(stderr, "No ranking provided in line %ld\n", stats.lines);
      return 1;
    }
  } else if (strcmp("search", event.event_type) == 0) {
    // Search contains a set of search terms. 
    event.query = strtok(NULL, delimiter);
    if (!event.query) {
      fprintf(stderr, "No query provided in line %ld\n", stats.lines);
      return 1;
    }
  } else {
    fprintf(stderr, "Invalid event_type provided in line %ld: '%s'\n", stats.lines, event.event_type);
    return 1;
  }

  return 0;
}

int main(int argc, char** argv) {
  char *appearances_filename = argv[1];
  char *urls_filename = argv[2];

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

  while (fgets(line_buffer, BUFSIZE, stdin)) {
    stats.lines++;
    
    if (parse_event() != 0)
      continue;

    if (aggregate_event() != 0)
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



