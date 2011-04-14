Atomic Burrito
=============

A set of tools supporting experiments in data aggregation.

A very important notice
-------

This toolkit uses the [AOL search data](http://en.wikipedia.org/wiki/AOL_search_data_scandal) provided in 2006. Though the data used in this toolkit is anonymized, please consider the privacy implications of downloading and using this data set. I would encourage you to anonymize any data that you use (by, for example, removing the AnonID field or hashing all URLs and query strings).

In this toolkit we use the original AnonIDs, but all search terms and URLs are run through SHA with a random salt (the salt is then discarded). See the `derpify` script for details. Several tools are run 

Also, the dataset is very, very, NSFW...

The tools
------------

* `sessionup` -- add sessions to the dataset
* `mksites` -- create a set of websites from the query strings and click data (approx 1.6 million)
* `indexsites` -- create a lucene index of data from mksites
* `searchsites` -- create search, appearance and click events from dataset and site index (approx 1.6 billion)
* `derpify` -- anonymize the event data set (very very important!)

Introduction
------------

This is an experiment in data storage, aggregation and reporting. The basic premise is simple. Pretend we are running a very popular search engine and we want to record statistics for analysis. There are three different types of events that we want to keep track of:

* `search` events -- when a user performs a search.
* `appearance` events -- when a website showed in the search results.
* `click` events -- when a website link was clicked.

The data is largely dominated by appearance events. For every search there would be one search event, maybe a hundred appearance events and optionally one or more click events.

Some of the statistics we are interested in are:

* What are the most popular search terms?
* What are the most popular sites?
* For a given site, what events occurred over a given time period.

The data
------------

The AOL dataset is provided as a set of plain tab separated files:

    213M	user-ct-test-collection-01.txt
    217M	user-ct-test-collection-02.txt
    221M	user-ct-test-collection-03.txt
    219M	user-ct-test-collection-04.txt
    228M	user-ct-test-collection-05.txt
    212M	user-ct-test-collection-06.txt
    219M	user-ct-test-collection-07.txt
    217M	user-ct-test-collection-08.txt
    217M	user-ct-test-collection-09.txt
    217M	user-ct-test-collection-10.txt
    2.2G	total

Each file contains a header row and approximately 3.6 million records (for a total of around 36 million). The data spans for a period of three months. This seems like a lot, but with a modern computer it is actually pretty easy to handle. It can comfortably fit into RAM on commodity hardware, which makes simple for loops a viable prospect. What we want is a lot of data, like *ONE BILLION RECORDS* (muahahahaha). So we need to explode this dataset somehow.

To do this, we build a simple search engine using the data from `mksites` and replay the search queries using `searchsites`. The resulting event data set is approximately 1.6 billion events. Large enough for algorithms and storage to become an interesting problem.

The anonymized data set
--------------

Throughout the codebase you may see references to the mythical *events.gz*. This is the dataset that drives the entire system and it's a simple tab-seperated file:

* The first column is the session id. This is derived from the AOL data by assuming users are performing queries from the same browser and sessions have a 30 minute timeout (see `sessionup`). It is a random string of 64 bytes.
* The second column is the user id. This is preserved from the AOL data. It is an integer.
* The third column is the date of the request, e.g. `2006-03-20 03:55:57`.
* The fourth column is the event type, which is one of `search`, `click` or `appearance`.

Those columns are common to all events. However the later columns depend on the event type.

`search` events:
* The fifth column is a space seperated list of search terms. Each term is a hashed 64 byte string. Because of the one-way random-seeded hash you cannot perform a reverse operation to find out what they originally searched for. This is good because the data was very private.

`click` events:
* The fifth column is the website that the user clicked on. Like search terms the data has been put through an anonymizing hash and is a 64 byte string.

`appearance` events:
* The fifth column is the website that appeared in a search result. It is an anonymous 64 byte string.
* The sixth column is the ranking of the appearance. An appearance of "0" means it had the first rank.

