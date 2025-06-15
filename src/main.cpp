#include "data.h"
#include <json/value.h>
#include <iostream>

int main() {
  // Create the request handle
  cURL::Handle handle;
  // Form the request
  std::string requestURL{ "http://ip-api.com/json" };
  // Request the data
  auto [result, data, headers] = cURL::getData(requestURL, handle);

  // Check for request success
  if(result == cURL::Result::SUCCESS) {
    if(!data.empty()) {
      // Extract the "Content-Type" header
      std::string contentType = cURL::getContentType(headers);
      // Check for valid JSON, XML, or HTML response and parse
      if(contentType.find("application/json") != std::string::npos) {
        // Parse the JSON
        Json::Value parsedData = JSON::parseData(data);
        // Process data
        Location currentLocation = parseLocation(parsedData);
       
        // TODO:
        // Research open weather APIs for an option that gives both current conditions and hourly forecast
        // Request weather data using parsed location from above
        // Serialize weather data into a conditional JSON string to be written to a cache file and read by waybar

        return 0;
      } else {
        std::cerr << "ERROR: API returned unexpected format (" << contentType << ")\n";
        return 1;
      }
    } else {
      std::cerr << "ERROR: Retrieved data string is empty.\n";
      return 1;
    }
  } else {
    std::cerr << "ERROR: cURL request failed.\n";
    return 1;
  }
}
