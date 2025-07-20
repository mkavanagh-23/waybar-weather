#include "data.h"
#include "location.h"
#include "weather.h"
#include <cmath>
#include <format>
#include <iomanip>
#include <json/value.h>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <iostream>
#include <optional>

// Returns jsonText, jsonClass
std::tuple<std::string, std::string, std::string> Weather::State::barFormat() {
  std::string color{ "" };
  double roundedTemp = std::round(tempF * 10.0) / 10.0;    // Round to nearest tenths
  if(roundedTemp < 0.0) {
    color = "blue";
  } else if(roundedTemp < 32.0) {
    color = "purple";
  } else if(roundedTemp < 60.0) {
    color = "yellow";
  } else if(roundedTemp < 78.0) {
    color = "orange";
  } else if(roundedTemp < 90.0) {
    color = "pink";
  } else {
    color = "red";
  }
  std::ostringstream tempStream;    // And truncate the value for display
  tempStream << std::fixed << std::setprecision(1) << roundedTemp;
  std::string temperature = tempStream.str();
  std::string text{""};
  if(daytime){
    if(stateIconsDay.contains(description)) {
      text += stateIconsDay.at(description) + "  ";
    } else {
      text += "  ";
    }
  } else {
    if(stateIconsNight.contains(description)) {
      text += stateIconsNight.at(description) + "  ";
    } else {
      text += "  ";
    }
  }
  text += temperature + " 󰔅";
  
  // Create the tooltip
  std::ostringstream timeStream;
  timeStream << std::format("{:%T}", timeStamp);
  std::ostringstream sunriseStream;
  sunriseStream << std::format("{:%T}", timeSunrise);
  std::ostringstream sunsetStream;
  sunsetStream << std::format("{:%T}", timeSunset);
  std::string tooltip{ description };
  if(!(windChillF == INVALID_TEMP)) {
    std::ostringstream windChillStream;
    windChillStream << std::fixed << std::setprecision(1) << std::round(windChillF * 10.0) / 10;
    tooltip += "<span foreground='#92a2d5'>\\n󱩱  Feels Like: " + windChillStream.str() + " 󰔅</span>"; 
  } else if(!(heatIndexF == INVALID_TEMP)) {
    std::ostringstream heatIndexStream;
    heatIndexStream << std::fixed << std::setprecision(1) << std::round(heatIndexF * 10.0) / 10;
    tooltip += "<span foreground='#ea83a5'>\\n󱣖  Feels Like: " + heatIndexStream.str() + " 󰔅</span>";;  
  }
  std::ostringstream humidStream;
  humidStream << std::fixed << std::setprecision(2) << (std::round(humidPct * 100.0) / 100.0);
  tooltip += "\\n  Humidity: " + humidStream.str() + " 󰏰";
  std::ostringstream pressStream;
  pressStream << std::fixed << std::setprecision(2) << (std::round(barPressureIn * 100.0) / 100.0);
  tooltip += "\\n  Pressure: " + pressStream.str() + " in.";
  std::ostringstream windStream;
  windStream << std::fixed << std::setprecision(1) << (std::round(windSpeedMph * 10.0) / 10.0);
  tooltip += "\\n  Wind: " + windStream.str() + " mph  " + windDirection;
  tooltip += "\\n  Sunrise: " + sunriseStream.str();
  tooltip += "\\n󰖚  Sunset: " + sunsetStream.str();
  tooltip += "\\n󰥔  Updated: " + timeStream.str();
  tooltip += "\\n(DEBUG) State: "; 
  tooltip += (daytime) ? "Day" : "Night";
  return std::make_tuple(text, color, tooltip);
}

std::optional<std::string> getWeather(const std::pair<double, double>& coordinates) {
  cURL::Handle handle;  // This handle to be utilized for all subsequent function calls
  auto pointsRet = getPointsData(coordinates, handle);
  if(!pointsRet.has_value()) {
    std::cerr << "ERROR: Failed to retrieve data for given coordinates.\n";
    return std::nullopt;
  }
  auto [stationsURL, forecastURL] = *pointsRet;
  auto conditionsRet = getCurrentConditions(stationsURL, coordinates, handle);
  if(!conditionsRet.has_value()) {
    std::cerr << "ERROR: Failed to retrieve data for given station.\n";
    return std::nullopt;
  }
  auto [jsonText, jsonClass, jsonTooltip] = *conditionsRet;
  //tooltip = getForecastData(forecastURL, handle);
  return { "{\"text\":\"" + jsonText + "\",\"class\":\"" + jsonClass + "\",\"tooltip\":\"" + jsonTooltip + "\"}" };
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

std::optional<std::tuple<std::string, std::string, std::string>> getCurrentConditions(const std::string& stationsURL, const std::pair<double,double> coordinates, cURL::Handle& curl) {
  auto closestRet = getClosestStation(stationsURL, coordinates, curl);
  if(!closestRet.has_value())
    return std::nullopt;
  std::string stationID = *closestRet;
  std::string conditionsURL { "https://api.weather.gov/stations/" + stationID + "/observations/latest" };
  auto [result, data, headers] = cURL::getData(conditionsURL, curl);
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
  Weather::State currentState(parsedData, coordinates, curl);
  return currentState.barFormat();
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
}
