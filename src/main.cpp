#include "data.h"
#include "location.h"
#include "weather.h"
#include <cstdlib>
#include <json/value.h>
#include <iostream>
#include <optional>
#include <utility>

int main() {
  std::cout << "User Agent: " << std::getenv("USER_AGENT");
  std::optional<Location> locationRet = getLocation();
  if(!locationRet.has_value()) {
    std::cerr << "ERROR: Unable to retrieve location.";
    return EXIT_FAILURE;
  }
  Location location = *locationRet;
  auto coordinates = std::make_pair(location.latitude, location.longitude);
  getWeather(coordinates);

  // TODO:
  // Get the currrent weather
    // Research open weather APIs for an option that gives both current conditions and hourly forecast
  // Get the upcoming forecast
  // Build json string and write to cache file

  return EXIT_SUCCESS;
}







