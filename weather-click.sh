#!/bin/bash

ip_response=$(curl -v "http://ip-api.com/json" | jq)

lat=$(echo "$ip_response" | grep "lat" | awk -F': ' '{print $2}' | tr -d ',')
long=$(echo "$ip_response" | grep "lon" | awk -F': ' '{print $2}' | tr -d ',')
url="https://forecast.weather.gov/MapClick.php?lat=$lat&lon=$long"
xdg-open "$url"
