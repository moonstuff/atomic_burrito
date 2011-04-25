#!/usr/bin/env ruby

# Scan the already sessionup'd data for statistics.
#
# Useful when the first version of sessionup doesn't include stats :(
#
session = nil
longest_count = 0
longest_session = nil

requests = 0

ARGF.each do |line|
  row = line.split("\t")
  row_session = row[0]

  if row_session != session
    if requests > longest_count
      longest_count = requests
      longest_session = session
    end

    session = row_session
    requests = 0
  end
  
  requests += 1
end

STDERR << "Longest session #{longest_session} with #{longest_count} requests"

