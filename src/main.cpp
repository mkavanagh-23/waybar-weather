#include "location.h"
#include "weather.h"
#include <cstdlib>
#include <json/value.h>
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <utility>

const std::string TEMP_FILE_PATH{ "/tmp/waybar-weather.json" };

bool writeToFile(const std::string& contents) {
  std::ofstream file(TEMP_FILE_PATH);
  if(!file) {
    std::cerr << "ERROR: Failed to open output JSON for writing.\n";
    return false;
  }
  file << contents;
  file.close();
  std::cout << "Weather data successfully written to tmp file: '" << TEMP_FILE_PATH << "'\n";
  return true;
}

int main() {
  std::optional<Location> locationRet = getLocation();
  if(!locationRet.has_value()) {
    std::cerr << "ERROR: Unable to retrieve location.";
    return EXIT_FAILURE;
  }
  Location location = *locationRet;
  auto coordinates = std::make_pair(location.latitude, location.longitude);
  auto weatherRet = getWeather(coordinates);
  if(!weatherRet.has_value()){
    std::cerr << "Error: Failed to retrieve weather data.\n";
    return EXIT_FAILURE;
  }
  std::string weatherStr = *weatherRet;
  writeToFile(weatherStr);
  return EXIT_SUCCESS;
}







