#ifndef NOM1_H
#define NOM1_H

#include <stdio.h>
#include <kclangc.h>

// Help with input. No record is more than 32KB and is tab separated.
#define BUFSIZE 32768

// Yeah this is pretty kooky but it is simple and fun.
// An "event" is just pointers to strings inside a buffer.
struct event {
  char *id;
  char *session;
  char *user_id;
  char *occurred_at;
  char *event_type;
  char *query;
  char *url;
  char *ranking;
};

// Lots of lovely stats.
struct stats {
  unsigned long lines;
  unsigned long searches;
  unsigned long clicks;
  unsigned long appearances;
  unsigned long urls;
  unsigned long reports;
};

extern struct stats stats;
extern char *event_day;

// DB access fun
KCDB *appearances;
KCDB *urls;
KCDB *url_reports;

extern int aggregate_event(struct event *event);
extern int parse_event(char *line_buffer, struct event *event);
extern int generate_url_reports(char *url_reports_filename);

#endif
