Sessionup
=============

Given an AOL search file sorted by user, calculate a psuedo-random session id and prepend to the record. Each user gets 30 minutes before their session times out and a new one begins.

Sort by user with the following:
    $ cat user-ct-* | sort > user-ct-test-collection-all.txt

Then you can add sessions
    $ cat sorted-by-user | sessionup > user-ct-test-collection-all.txt.sessions

You might like to pipe through gzip to increase throughput and save disk space.

