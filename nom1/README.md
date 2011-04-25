nom1
=============

A basic implementation of an aggregator. It is designed to aggregate a single day of events into RAM, then appending the result into a simple key-value store.

The input format is the stock record format, but be sure that it is only the records for that day. For example:

    $ zcat events-2006-03-01.gz | ./nom1 2006-03-01 reports.kch

The output, _reports.kch_ is a Kyoto Cabinet database. The parameter "2006-03-01" tells _nom1_ which day to put the aggregated count under in the database. Because it's just a Kyoto Cabinet database, you can change and tune the database by simply changing the file name, like so:

    $ zcat events-2006-03-01.gz | ./nom1 2006-03-01 reports.kch#apow=8

This changes the record alignment in the database, which prevents a lot of fragmentation in the reports database when you start appending more and more days.

Aggregating a date range
----------

Considering that _nom1_ appends to any existing reports database, performing aggregations for a month is a simple matter of mapping _nom1_ over each day in the range. For example, aggregating for the month can be done in bash like this:

    $ for x in `seq -w 01 31`
    do
        zcat events-2006-03-$x | ./nom1 2006-03-$x reports.kch#apow=8
    done

This will execute _nom1_ in sequence over the set, folding the day aggregates.

Parallel execution
-----------

Kyoto Cabinet has built-in locking (depending on database type), so parallel execution on a single machine becomes trivial. GNU Parallel is a simple tool for this task:

    $ parallel -i sh -c "zcat appearances-2006-03-{}.gz | ~/dev/atomic_burrito/nom1/nom1 2006-03-{} url_reports.kch#apow=8" -- `seq -w 01 31`

This will execute N parallel instances of _nom1_, where N is the number of cores on your machine. I hope you weren't planning on doing anything on that machine for a while.

Performance
-----------

I would say that performance is acceptable. I have a machine with the following specs:

* Intel Core i7 860 (quad-core 2.80GHz), 8GB RAM
* A single 7200 RPM spinny-rust hard drive
* Ubuntu 10.10

Even without compiler optimisations, or storing the data in a column oriented format, the month of March is aggregated in just under 30 minutes. That's 652,698,576 records at around 360,000 records per second.
