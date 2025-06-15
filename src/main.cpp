#include "data.h"
#include <cstdlib>
#include <json/value.h>
#include <iostream>
#include <optional>

std::optional<Location> getLocation() {
  // Create the request handle
  cURL::Handle handle;
  // Form the request
  std::string requestURL{ "http://ip-api.com/json" };
  // Request the data
  auto [result, data, headers] = cURL::getData(requestURL, handle);

  // Check for request success
  if(result != cURL::Result::SUCCESS) {
    std::cerr << "ERROR: cURL request failed.\n";
    return std::nullopt;
  }
  if(data.empty()) {
    std::cerr << "ERROR: Retrieved data string is empty.\n";
    return std::nullopt;
  }
  
  // Extract the "Content-Type" header
  std::string contentType = cURL::getContentType(headers);
  // Check for valid JSON, XML, or HTML response and parse
  if(contentType.find("application/json") != std::string::npos) {
    // Parse the JSON
    Json::Value parsedData = JSON::parseData(data);
    // Process data
    return Location(parsedData);
  } else {
    std::cerr << "ERROR: API returned unexpected format (" << contentType << ").\n";
    return std::nullopt;
  }
}

int main() {

  std::optional<Location> locationRet = getLocation();
  if(!locationRet.has_value()) {
    std::cerr << "ERROR: Unable to retrieve location.";
    return EXIT_FAILURE;
  }
  Location location = *locationRet;
  std::cout << location;

  // TODO:
  // Get the currrent weather
    // Research open weather APIs for an option that gives both current conditions and hourly forecast
  // Get the upcoming forecast
  // Build json string and write to cache file

  return EXIT_SUCCESS;
}







