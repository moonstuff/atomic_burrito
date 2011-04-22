#!/usr/bin/env ruby

$urls = {}
$appearances = {}
reports_filename = ARGV[0]

$stats = {
  :lines => 0,
  :searches => 0,
  :appearances => 0,
  :clicks => 0,
  :urls => 0,
}

def store_url(url)
  if $urls[url] == nil then
    $urls[url] = 1
    $stats[:urls] = $stats[:urls] + 1
  else
    $urls[url] = $urls[url] + 1
  end
end

def store_appearance(url, date)
  appearances_for_url = $appearances[url] || {}
  appearances_for_url[date] = (appearances_for_url[date] or 0) + 1
  $appearances[url] = appearances_for_url
end

STDIN.each do |line|
  event = line.split("\t")
  event_type = event[3]
  if event_type == "appearance"
    $stats[:appearances] += 1
    url = event[4]
    date = event[2].split[0]
    store_appearance(url, date)
    store_url(url)
  elsif event_type == "click" 
    $stats[:clicks] += 1
    store_url(url)
  elsif event_type == "search"
    $stats[:searches] += 1
  end
  $stats[:lines] += 1
end

puts("EOF reached: #{$stats[:lines]} records")
puts("#{$stats[:searches]} searches, #{$stats[:appearances]} appearances, #{$stats[:clicks]} clicks, #{$stats[:urls]} urls")
puts("generating url reports for #{$stats[:urls]} urls")

reports = File.open(reports_filename, "w+")
$urls.keys.each do |url|
  if $appearances.has_key? url
    reports.write("#{url}\n")
    $appearances[url].each do |date, appearance_count|
      reports.write("#{date}: #{appearance_count}\n")
    end
  end
end

