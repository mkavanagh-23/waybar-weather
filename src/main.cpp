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
  // Parse the JSON
  Json::Value parsedData = JSON::parseData(data);
  // Process data
  std::cout << data;
  return 0;
}
