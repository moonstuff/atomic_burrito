#!/usr/bin/env luajit

require "stringy"
local md5 = require "md5"

for line in io.lines() do
  local event = stringy.split(line, "\t")
  event[1] = md5.sumhexa(event[1]) -- session
  -- event[2] -- userid, can be left as-is
  -- event[3] -- date, can be left as-is
  local event_type = event[4]
  if "appearance" == event_type or "click" == event_type then
    event[5] = md5.sumhexa(event[5])
  else
    local terms = stringy.split(event[5], " ")
    local hashed_terms = {}
    for i, term in pairs(terms) do
      table.insert(hashed_terms, md5.sumhexa(term))
    end
    event[5] = table.concat(hashed_terms," ")
  end
  print(table.concat(event,"\t"))
end
