#include <stdio.h>
#include <kclangc.h>

/* main routine */

#define BUFSIZE 32768

int main(int argc, char** argv) {

  // DB access fun
  char *filename = argv[1];
  KCDB* db; // The database
  char *kbuf, *vbuf; // A buffer for keys and values
  size_t ksiz, vsiz;
  int64_t one = 1;
  char key[BUFSIZE];

  // stdin reading fun
  char line[BUFSIZE]; // The current line
  char *delimiter = "\t"; // The line delimiter

  // Lots of lovely stats.
  long lines = 0;
  long searches = 0;
  long clicks = 0;
  long appearances = 0;

  // Semantic names for event fields
  char *session; // All events contain a session
  char *userId; // All events contain a userId
  char *occurredAt; // All events contain an occurredAt date
  char *eventType; // All events contain eventType, which can be "search", "click" or "appearance"
  char *query; // search events contain a query string
  char *url; // click events and appearance events contain a url
  char *ranking; // click events and appearance events contain a ranking

  db = kcdbnew();
  if (!kcdbopen(db, filename, KCOWRITER | KCOCREATE)) {
    fprintf(stderr, "open error: %s\n", kcecodename(kcdbecode(db)));
  }

  while (fgets(line, BUFSIZE, stdin)) {
    lines++;
    
    session = strtok(line, delimiter);
    if (!session) {
      fprintf(stderr, "No session provided in line %ld\n", lines);
      continue;
    
    }

    userId = strtok(NULL, delimiter);
    if (!userId) {
      fprintf(stderr, "No userId provided in line %ld\n", lines);
      continue;
    }
    
    occurredAt = strtok(NULL, delimiter);
    if (!occurredAt) {
      fprintf(stderr, "No occurredAt provided in line %ld\n", lines);
      continue;
    }

    eventType = strtok(NULL, delimiter);
    if (!eventType) {
      fprintf(stderr, "No eventType provided in line %ld\n", lines);
      continue;
    }
 
    // Continue parsing based on eventType
    if (strcmp("appearance", eventType) == 0 || strcmp("click", eventType) == 0)  {
      // Both these types have a URL and ranking
      url = strtok(NULL, delimiter);
      if (!url) {
        fprintf(stderr, "No url provided in line %ld\n", lines);
        continue;
      }

      ranking = strtok(NULL, delimiter);
      if (!ranking) {
        fprintf(stderr, "No ranking provided in line %ld\n", lines);
        continue;
      }
    } else if (strcmp("search", eventType) == 0) {
      searches++;
      // Search contains a set of search terms. 
      query = strtok(NULL, delimiter);
      if (!query) {
        fprintf(stderr, "No query provided in line %ld\n", lines);
        continue;
      }
    } else {
      fprintf(stderr, "Invalid eventType provided in line %ld: '%s'\n", lines, eventType);
      continue;
    }

    
    // Now perform processing based on eventType
    if (strcmp("appearance", eventType) == 0) {
      appearances++;

      // key is "url-appearance-yyyy-mm-dd". appending strings is a little ugly...
      int keylength = snprintf(key, BUFSIZE-1, "%s-appearance-", url);
      strncat(key+keylength, occurredAt, 10);
      if (kcdbincrint(db, key, strlen(key), one) == INT64_MIN) {
        fprintf(stderr, "Count not increment count on line %ld\n", lines);
        continue;
      }
    } else if (strcmp("search", eventType) == 0) {
      searches++;
    } else if (strcmp("click", eventType) == 0)  {
      clicks++;
    }
  }

  fprintf(stderr, "EOF reached: %ld records\n", lines);
  fprintf(stderr, "%ld searches, %ld appearances, %ld clicks\n", searches, appearances, clicks);
 
  if (!kcdbclose(db)) {
    fprintf(stderr, "close error: %s\n", kcecodename(kcdbecode(db)));
  }

  kcdbdel(db);

  return 0;
}

