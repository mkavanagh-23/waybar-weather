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

  std::cout << "Response headers:\n";
  for(const auto& header : headers) {
    std::cout << header;
  }

  // TODO:
  // Parse and validate headers

  // Parse the JSON
  Json::Value parsedData = JSON::parseData(data);

  // TODO:
  // Status
  // City
  // Region/State
  // Zip
  // Lat/Long
  // IP

  // Process data
  std::cout << "ResponseData:\n";
  std::cout << data;
  return 0;
}
