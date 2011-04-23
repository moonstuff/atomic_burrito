#!/usr/bin/env luajit

require "stringy"
kc = require "kyotocabinet"

if #arg ~= 3 then
  print("usage: nom2 <appearances database> <urls database> <url reports database>")
  return 1
end

appearances_filename = arg[1]
appearances = kc.DB:new()
if not appearances:open(appearances_filename, kc.DB.OWRITER + kc.DB.OCREATE) then
   print("appearances database open error: " .. tostring(appearances:error()))
end

urls_filename = arg[2]
urls = kc.DB:new()
if not urls:open(urls_filename, kc.DB.OWRITER + kc.DB.OCREATE) then
   print("urls database open error: " .. tostring(urls:error()))
end

url_reports_filename = arg[3]
url_reports = kc.DB:new()
if not url_reports:open(url_reports_filename, kc.DB.OWRITER + kc.DB.OCREATE) then
   print("url_reports database open error: " .. tostring(url_reports:error()))
end

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
    date = stringy.split(event[3], " ")[1]
    appearances:increment(string.format("%s-%s",url,date), 1)
    urls:increment(url, 1)
  elseif event_type == "click" then
    stats.clicks = stats.clicks + 1
    urls:increment(event[5], 1)
  elseif event_type == "search" then
    stats.searches = stats.searches + 1  
  end
  stats.lines = stats.lines + 1
end
stats.urls = urls:count()

print(string.format("EOF reached: %d records", stats.lines))
print(string.format("%d searches, %d appearances, %d clicks, %d urls", stats.searches, stats.appearances, stats.clicks, stats.urls))
print(string.format("generating url reports for %d urls", stats.urls))

-- Now print the reports!
for url, url_count in urls:pairs() do
   local appearance_aggregates = appearances:match_prefix(url)
   if #appearance_aggregates > 0 then
     local url_report = url .. "\n"
     for index, key in pairs(appearance_aggregates) do
       url_report = url_report .. string.format("%s: %s\n", key:sub(#url + 2), appearances:increment(key, 0))
     end
     url_reports[url] = url_report
   end
end

