#include "nom1.h"

#define DELIMITER "\t"

int parse_event(char *line_buffer, struct event *event) {
  event->id = strtok(line_buffer, DELIMITER);
  if (!event->session) {
    fprintf(stderr, "No id provided in line %ld\n", stats.lines);
    return 1;
  }

  event->session = strtok(NULL, DELIMITER);
  if (!event->session) {
    fprintf(stderr, "No session provided in line %ld\n", stats.lines);
    return 1;
  }

  event->user_id = strtok(NULL, DELIMITER);
  if (!event->user_id) {
    fprintf(stderr, "No user_id provided in line %ld\n", stats.lines);
    return 1;
  }
  
  event->occurred_at = strtok(NULL, DELIMITER);
  if (!event->occurred_at) {
    fprintf(stderr, "No occurred_at provided in line %ld\n", stats.lines);
    return 1;
  }

  event->event_type = strtok(NULL, DELIMITER);
  if (!event->event_type) {
    fprintf(stderr, "No event_type provided in line %ld\n", stats.lines);
    return 1;
  }

  // Continue parsing based on eventType
  if (strcmp("appearance", event->event_type) == 0 || strcmp("click", event->event_type) == 0)  {
    // Both these types have a URL and ranking
    event->url = strtok(NULL, DELIMITER);
    if (!event->url) {
      fprintf(stderr, "No url provided in line %ld\n", stats.lines);
      return 1;
    }

    event->ranking = strtok(NULL, DELIMITER);
    if (!event->ranking) {
      fprintf(stderr, "No ranking provided in line %ld\n", stats.lines);
      return 1;
    }
  } else if (strcmp("search", event->event_type) == 0) {
    // Search contains a set of search terms. 
    event->query = strtok(NULL, DELIMITER);
    if (!event->query) {
      fprintf(stderr, "No query provided in line %ld\n", stats.lines);
      return 1;
    }
  } else {
    fprintf(stderr, "Invalid event_type provided in line %ld: '%s'\n", stats.lines, event->event_type);
    return 1;
  }

  return 0;
}


