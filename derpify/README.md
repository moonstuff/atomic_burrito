derpify
=============

One of the unfortunate aspects of the AOL data set is the amount of personal information it contains. By including the AnonID *and* URL/query data it became possible to determine the identity of some users.

Obviously, we don't want that in our codebase. We don't actually need the identifying data in our application so instead we run all URLs and queries through a one-way hash. After throwing away the salt it become practically impossible (like, really really hard, end of the universe hard). This keeps the sessions and AnonID (which can pose some interesting algorithm problems later on) without all the privacy problems.

Once derpified, please delete the original AOL and interstitial data.
