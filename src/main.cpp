#include "data.h"
#include "location.h"
#include <cstdlib>
#include <json/value.h>
#include <iostream>
#include <optional>

int main() {
  std::optional<Location> locationRet = getLocation();
  if(!locationRet.has_value()) {
    std::cerr << "ERROR: Unable to retrieve location.";
    return EXIT_FAILURE;
  }
  Location location = *locationRet;

  // TODO:
  // Get the currrent weather
    // Research open weather APIs for an option that gives both current conditions and hourly forecast
  // Get the upcoming forecast
  // Build json string and write to cache file

  std::cout << location;
  return EXIT_SUCCESS;
}







