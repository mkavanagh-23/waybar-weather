#include "data.h"
#include "location.h"
#include <cmath>
#include <string>
#include <iostream>
#include <optional>

Location::Location(const Json::Value& parsedLocation)
{
  if(parsedLocation.isMember("country")) {
    country = parsedLocation["country"].asString();
  }
  if(parsedLocation.isMember("region")) {
    region = parsedLocation["region"].asString();
  }
  if(parsedLocation.isMember("city")) {
    city = parsedLocation["city"].asString();
  }
  if(parsedLocation.isMember("zip")) {
    zip = parsedLocation["zip"].asString();
  }
  if(parsedLocation.isMember("timezone")) {
    timezone = parsedLocation["timezone"].asString();
  }
  if(parsedLocation.isMember("lat")) {
    latitude = parsedLocation["lat"].asDouble();
  }
  if(parsedLocation.isMember("lon")) {
    longitude = parsedLocation["lon"].asDouble();
  }
  if(parsedLocation.isMember("query")) {
    ip = parsedLocation["query"].asString();
  }
}

std::ostream& operator<<(std::ostream& out, const Location& location){
  out << "Location (" << location.ip << "):\n"
      << location.city << ", " << location.region << " | " << location.country << ' ' << location.zip << '\n'
      << '(' << location.latitude << ", " << location.longitude << ")\n";
  return out;
}

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

// Calculate distance between two lat/lon points using Haversine formula
// Haversine calculates distances on a sphere
double haversineDistance(double lat1, double long1, double lat2, double long2) {
  const double R = 6371.0; // Earth's radius in kilometers
  double lat1Rad = lat1 * M_PI / 180.0;
  double lat2Rad = lat2 * M_PI / 180.0;
  double deltaLat = (lat2 - lat1) * M_PI / 180.0;
  double deltaLon = (long2 - long1) * M_PI / 180.0;

  double a = sin(deltaLat / 2.0) * sin(deltaLat / 2.0) + cos(lat1Rad) * cos(lat2Rad) * sin(deltaLon / 2.0) * sin(deltaLon / 2.0);
  double c = 2.0 * atan2(sqrt(a), sqrt(1 - a));    
  
  return R * c;
}
