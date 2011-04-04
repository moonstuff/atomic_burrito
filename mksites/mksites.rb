#!/usr/bin/env ruby
require 'set'

# Given an AOL search file sorted by site, gather all sites and their associated terms
#
# You can sort the AOL data with:
# $ cat user-ct-* | sort -t $'\t' -k 5
#
# Then the output is a tab-seperated file with two records:
#     link  terms
#
# Suitable for indexing by indexsites

current_link = nil
terms = Set.new

def write_link(link, terms)
  STDOUT << "#{link}\t#{terms.to_a.join(' ')}\n" if link and not terms.length == 0
end

ARGF.each do |line|
  row = line.split("\t")
  next if row.length < 5

  anon_id = row[0]
  next if anon_id == "AnonID"
  
  link = row[4].chomp!
  next if link.length == 0

  if current_link != link
    write_link(current_link, terms)
    current_link = link
    terms = Set.new
  end

  query = row[1]
  terms.merge(query.split)
end

write_link(current_link, terms)

