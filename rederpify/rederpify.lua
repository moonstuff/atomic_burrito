#!/usr/bin/env luajit

require "stringy"
local md5 = require "md5"
local seed = string.format("%d", math.random(2^64))

for line in io.lines() do
  local event = stringy.split(line, "\t")
  event[1] = md5.sumhexa(event[1]) -- event[1] is the session
  local event_type = event[4]
  if "appearance" == event_type or "click" == event_type then
    event[5] = md5.sumhexa(event[5] .. seed) -- event[5] is the url
  else
    local terms = stringy.split(event[5], " ") -- event[5] is the search query
    local hashed_terms = {}
    for i, term in pairs(terms) do
      table.insert(hashed_terms, md5.sumhexa(term .. seed))
    end
    event[5] = table.concat(hashed_terms," ")
  end
  -- add a record id
  event[0] = md5.sumhexa(line .. seed)
  print(table.concat(event,"\t"))
end
