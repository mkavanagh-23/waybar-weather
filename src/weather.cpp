// TODO:
// getWeather should return both a current state and forecast objects
//  NEED TO CREATE A WEATHER OBJECT
// Function to parse list of stations retrieved via another GET request to observationStations URL
  // Extract each stations ID and coordinates
  // Should be stored in a local array/vector
// Function to calculate distance between two points to find closest station HAVERSINE formula
// Function to find the closest station
  // Loop through each station stored in the array, using the distance formula to find the closest
// Function to get the latest observations from the closest station
// Function to parse conditions, temp, etc from latest observations
// Function to parse hourly forecast from gridpoints object

#include "data.h"
#include "weather.h"
#include <json/value.h>
#include <string>
#include <utility>
#include <iostream>
#include <optional>

std::optional<std::string> getWeather(std::pair<double, double> coordinates) {
  cURL::Handle handle;  // This handle to be utilized for all subsequent function calls
  auto pointsRet = getPointsData(coordinates, handle);
  if(!pointsRet.has_value()) {
    std::cerr << "ERROR: Failed to retrieve data for given coordinates.\n";
    return std::nullopt;
  }
  auto [stationsURL, gridpointsURL] = *pointsRet;
  std::cout << stationsURL << '\n' << gridpointsURL << '\n';
  // TODO: 
  // GET request to stationsURL
  //    Extract each listed station's ID and Coordinates
  //    Store results in a local data structure/array
  //    Calculate distance between each point and our approximate location, return the closest ID
  //    Use the returned ID to make a request to observations/latest
  //    Parse data from current conditions
  // GET request to gridpointsURL/forecast/hourly
  return std::nullopt;
};

std::optional<std::pair<std::string, std::string>> getPointsData(std::pair<double, double> coordinates, cURL::Handle& curl){
  std::string url{ "https://api.weather.gov/points/" + std::to_string(coordinates.first) + ',' + std::to_string(coordinates.second) };
  auto [result, data, headers] = cURL::getData(url, curl);
  
  if(result != cURL::Result::SUCCESS) {
    std::cerr << "ERROR: cURL request failes.\n";
    return std::nullopt;
  }
  if(data.empty()) {
    std::cerr << "ERROR: Retrieved points data string is empty.\n";
    return std::nullopt;
  }
  std::string contentType = cURL::getContentType(headers);
  if(contentType.find("application/geo+json") == std::string::npos) {
    std::cerr << "ERROR: API returned unexpected format.\n";
    return std::nullopt;
  }

  Json::Value parsedData = JSON::parseData(data);
  if(parsedData.isMember("properties") && parsedData["properties"].isObject()) {
    const Json::Value& props = parsedData["properties"];    // Navigate to the "properties" element
    if(!props.isMember("forecastHourly") || !props.isMember("observationStations")) {
      std::cerr << "ERROR: Failed to parse forecast data URLs.\n";
      return std::nullopt;
    }
    return std::make_pair(props["observationStations"].asString(), props["forecastHourly"].asString());
  }
  std::cerr << "ERROR: Failed to parse properties element from points data.\n";
  return std::nullopt;
}
