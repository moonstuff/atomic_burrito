#!/usr/bin/env ruby

require 'digest'

# Derpify the output from searchsites. This is very important because it anonymizes the data set, removing
# a lot of personal and NSFW information from the data.
#
# Once derpified, the original and interstitial data can be deleted.

salt = rand(2**64).to_s

ARGF.each do |line|
  row = line.split("\t")
  row[4] = row[4].chomp.split.map { |s| Digest::SHA2.hexdigest(s + salt) }.join(" ")
  STDOUT << row.join("\t").chomp << "\n"
end

