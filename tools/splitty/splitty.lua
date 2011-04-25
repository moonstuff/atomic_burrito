#!/usr/bin/env luajit

-- Split a giant input file into lots of day files.
-- It'll even do some gzip for you :)

require "stringy"
gzip = require "gzip"
files = {}

for line in io.lines() do
  local event = stringy.split(line, "\t")
  local date = stringy.split(event[4], " ")[1]

  if not files[date] then
    files[date] = gzip.open("events-" .. date .. ".gz", "a+")
  end
  files[date]:write(line)
  files[date]:write("\n")
end

for date, file in pairs(files) do
  file:flush()
  file:close()
end
