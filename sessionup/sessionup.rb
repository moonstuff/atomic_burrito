#!/usr/bin/env ruby

require 'date'

# Given an AOL search file sorted by user, calculate a psuedo-random
# session id and prepend to the record.
#
# Sort by user with the following:
# $ cat user-ct-* | sort > user-ct-test-collection-all.txt
#
# Then you can add sessions
# cat sorted-by-user | sessionup > user-ct-test-collection-all.txt.sessions
#
# You might like to pipe through gzip to increase throughput and save disk space.

SESSION_TIMEOUT = 15 * 60
SESSION_KEY_LENGTH = 64

previous_anon_id = nil
previous_time = nil

session = nil
requests = 0

rows = 0
sessions = 0
longest = nil
longest_count = 0
longest_session = nil

ARGF.each do |line|
  rows += 1
  row = line.split("\t")
  anon_id = row[0]
  next if anon_id == "AnonID"

  query_time = DateTime.strptime(row[2], "%Y-%m-%d %H:%M:%S").to_time
  
  if anon_id != previous_anon_id or previous_time == nil or (query_time - previous_time) > SESSION_TIMEOUT
    if requests > longest_count
      longest_count = requests
      longest_session = session
    end

    session = rand(36**SESSION_KEY_LENGTH).to_s(36)
    sessions += 1
    requests = 0
  end
  
  row.insert(0, session)

  STDOUT << row.join("\t")
  STDERR << '.' if rows % 100000 == 0

  previous_anon_id = anon_id
  previous_time = query_time
  requests += 1
end

STDERR << "\n\nProcessed #{rows} rows"
STDERR << "Total sessions: #{sessions}"
STDERR << "Longest session #{longest_session} with #{longest_count} requests"

