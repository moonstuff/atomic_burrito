#!/usr/bin/env luajit

require "stringy"
  
-- No string splitter? *sigh*
-- This is from the lua users wiki where *everyone* goes to get this function.
function string:split(sep)
  local sep, fields = sep or ":", {}
  local pattern = string.format("([^%s]+)", sep)
  self:gsub(pattern, function(c) fields[#fields+1] = c end)
  return fields
end

function store_url(url)
  if urls[url] == nil then
    urls[url] = 1
    stats.urls = stats.urls + 1
  else
    urls[url] = urls[url] + 1
  end
end

function store_appearance(url, date)
  appearances_for_url = appearances[url] or {}
  appearances_for_url[date] = (appearances_for_url[date] or 0) + 1
  appearances[url] = appearances_for_url
end

-- No need for KyotoCabinet for the moment, Lua is hashes all the way down :)
urls = {}
appearances = {}
reports_filename = arg[1]

stats = {
  lines = 0,
  searches = 0,
  appearances = 0,
  clicks = 0,
  urls = 0,
}

for line in io.lines() do
  local event = stringy.split(line, "\t")
  local event_type = event[4]

  if event_type == "appearance" then
    stats.appearances = stats.appearances + 1
    url = event[5]
    date = string.split(event[3], " ")[1]
    store_appearance(url, date)
    store_url(url)
  elseif event_type == "click" then
    stats.clicks = stats.clicks + 1
    store_url(event[5])
  elseif event_type == "search" then
    stats.searches = stats.searches + 1  
  end
  stats.lines = stats.lines + 1
end

print(string.format("EOF reached: %d records", stats.lines))
print(string.format("%d searches, %d appearances, %d clicks, %d urls", stats.searches, stats.appearances, stats.clicks, stats.urls))
print(string.format("generating url reports for %d urls", stats.urls))

-- Now print the reports!
reports = io.open(reports_filename, "w+")
for url in pairs(urls) do
  if not (appearances[url] == nil) then
    reports:write(url, "\n")
    for date, appearance_count in pairs(appearances[url]) do
      reports:write(date, ": ", appearance_count, "\n")
    end
  end
end

