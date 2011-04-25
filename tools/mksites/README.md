mksites
=============

Given an AOL search file sorted by site, gather all sites and their associated terms

You can sort the AOL data with:
    $ cat user-ct-* | sort -t $'\t' -k 5

Then the output is a tab-seperated file with two records:
    link  terms

The resulting file is suitable for indexing by indexsites

