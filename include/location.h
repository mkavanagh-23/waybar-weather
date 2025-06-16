#ifndef LOCATION_H
#define LOCATION_H

#include <json/value.h>
#include <string>
#include <optional>
#include <ostream>

class Location {
public:
  std::string country;
  std::string region;
  std::string city;
  std::string zip;
  std::string timezone;
  std::string ip;
  double latitude;
  double longitude;

  Location(const Json::Value& parsedLocation);
  friend std::ostream& operator<<(std::ostream& out, const Location& location);
};

Location parseLocation(const Json::Value& parsedData);

std::optional<Location> getLocation();

#endif
