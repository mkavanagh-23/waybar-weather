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
#include "location.h"
#include "weather.h"
#include <json/value.h>
#include <limits>
#include <string>
#include <utility>
#include <iostream>
#include <optional>

std::optional<std::string> getWeather(const std::pair<double, double>& coordinates) {
  cURL::Handle handle;  // This handle to be utilized for all subsequent function calls
  auto pointsRet = getPointsData(coordinates, handle);
  if(!pointsRet.has_value()) {
    std::cerr << "ERROR: Failed to retrieve data for given coordinates.\n";
    return std::nullopt;
  }
  auto [stationsURL, forecastURL] = *pointsRet;
  
  getCurrentConditions(stationsURL, coordinates, handle);
  //getForecastData(forecastURL, handle);
  return std::nullopt;
};

std::optional<std::pair<std::string, std::string>> getPointsData(const std::pair<double, double>& coordinates, cURL::Handle& curl){
  std::string url{ "https://api.weather.gov/points/" + std::to_string(coordinates.first) + ',' + std::to_string(coordinates.second) };
  auto [result, data, headers] = cURL::getData(url, curl);
  
  if(result != cURL::Result::SUCCESS) {
    std::cerr << "ERROR: cURL request failed.\n";
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

void getCurrentConditions(const std::string& stationsURL, const std::pair<double,double> coordinates, cURL::Handle& curl) {
  // Get the closest station ID
  auto closestRet = getClosestStation(stationsURL, coordinates, curl);
  if(!closestRet.has_value())
    return;
  std::string stationID = *closestRet;
  std::string conditionsURL { "https://api.weather.gov/stations/" + stationID + "/observations/latest" };
  auto [result, data, headers] = cURL::getData(conditionsURL, curl);
  if(result != cURL::Result::SUCCESS) {
    std::cerr << "ERROR: cURL request failed.\n";
    return;
  }
  std::cout << data;
  // TODO:
  // Need to create a condition/state object
  // Parse JSON to extract members into object
  // Need to create a forecast object (Or possibly a hash map of conditions/states)
}

std::optional<std::string> getClosestStation(const std::string& stationsURL, const std::pair<double,double> coordinates, cURL::Handle& curl) {
  // Get stations data
  auto stationsRet = getStationsData(stationsURL, curl);
  if(!stationsRet.has_value()) {
    std::cerr << "ERROR: Stations request request failed.\n";
    return std::nullopt;
  }
  Json::Value parsedData = *stationsRet;
  // Enter into the 'features' (stations) array
  const Json::Value& stations = parsedData["features"];
  std::string closestStation { "" };
  double minDistance = std::numeric_limits<double>::max();
  // Iterate through each station
  for(const auto& station : stations) {
    std::string stationID = station["properties"]["stationIdentifier"].asString();
    const Json::Value stationCoordinates = station["geometry"]["coordinates"];
    double stationLat = stationCoordinates[1].asDouble();
    double stationLong = stationCoordinates[0].asDouble();
    // Calculate the distance from our current coordinates
    const auto& [ latitude, longitude ] = coordinates;
    double distance = haversineDistance(latitude, longitude, stationLat, stationLong);
    if(distance < minDistance) {
      minDistance = distance;
      closestStation = stationID;
    }
  }
  // Return the closest station's ID
  if(!closestStation.empty())
    return closestStation;
  else
    return std::nullopt;
}

std::optional<Json::Value> getStationsData(const std::string& stationsURL, cURL::Handle& curl) {
  auto [result, data, headers] = cURL::getData(stationsURL, curl);
  if(result != cURL::Result::SUCCESS) {
    std::cerr << "ERROR: cURL request failed.\n";
    return std::nullopt;
  }
  if(data.empty()) {
    std::cerr << "ERROR: Retrieved stations data string is empty.\n";
    return std::nullopt;
  }
  std::string contentType = cURL::getContentType(headers);
  if(contentType.find("application/geo+json") == std::string::npos) {
    std::cerr << "ERROR: API returned unexpected format.\n";
    return std::nullopt;
  }
  // Parse the stations data
  return JSON::parseData(data);
}

void getForecastData(const std::string& forecastURL, cURL::Handle& curl) {
  auto [result, data, headers] = cURL::getData(forecastURL, curl);
  if(result != cURL::Result::SUCCESS) {
    std::cerr << "ERROR: cURL request failed.\n";
    return;
  }
  if(data.empty()) {
    std::cerr << "ERROR: Retrieved forecast data string is empty.\n";
    return;
  }
  std::string contentType = cURL::getContentType(headers);
  if(contentType.find("application/geo+json") == std::string::npos) {
    std::cerr << "ERROR: API returned unexpected format.\n";
    return;
  }
  //std::cout << "Forecast data: " << data << '\n';
}
