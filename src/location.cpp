#include "data.h"
#include "location.h"
#include <string>
#include <iostream>
#include <optional>

std::optional<Location> getLocation() {
  cURL::Handle handle;
  std::string requestURL{ "http://ip-api.com/json" };
  auto [result, data, headers] = cURL::getData(requestURL, handle);

  if(result != cURL::Result::SUCCESS) {
    std::cerr << "ERROR: cURL request failed.\n";
    return std::nullopt;
  }
  if(data.empty()) {
    std::cerr << "ERROR: Retrieved data string is empty.\n";
    return std::nullopt;
  }
  
  std::string contentType = cURL::getContentType(headers);
  if(contentType.find("application/json") != std::string::npos) {
    Json::Value parsedData = JSON::parseData(data);
    return Location(parsedData);
  } else {
    std::cerr << "ERROR: API returned unexpected format (" << contentType << ").\n";
    return std::nullopt;
  }
}
