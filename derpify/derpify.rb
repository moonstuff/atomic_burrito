#!/usr/bin/env ruby

require 'digest'

# Derpify the output from searchsites. This is very important because it anonymizes the data set, removing
# a lot of personal and NSFW information from the data.
#
# Once derpified, the original and interstitial data can be deleted.

salt = rand(2**64).to_s

ARGF.each do |line|
  row = line.split("\t")
  if row[3] == "appearance" or row[3] == "click" then
    row[4] = Digest::SHA2.hexdigest(row[4] + salt) 
  elsif row[3] == "search"
    row[4] = row[4].chomp.split.map { |s| Digest::SHA2.hexdigest(s + salt) }.join(" ")
  end
  STDOUT << row.join("\t").chomp << "\n"
end

