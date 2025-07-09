#ifndef WEATHER_H
#define WEATHER_H

#include "data.h"
#include <chrono>
#include <json/value.h>
#include <optional>
#include <string>
#include <iostream>
#include <unordered_map>
#include <utility>

namespace Weather {

const double INVALID_TEMP { 5000 }; // Check for NULL values

// TODO: Add more values as they come up
const std::unordered_map<std::string, std::string> stateIconsDay{
  {"Clear", ""},
  {"Cloudy", ""},
  {"Heavy Rain and Fog/Mist", ""},
  {"Light Rain", ""},
  {"Light Rain and Fog/Mist", ""},
  {"Mostly Clear", ""},
  {"Mostly Cloudy", "󰅟"},
  {"Partly Cloudy", ""},
  {"Rain and Fog/Mist", ""},
  {"Thunderstorms", "󰖓"},
  {"Thunderstorms and Rain", ""},
  {"Thunderstorms and Rain and Fog/Mist", ""},
};

const std::unordered_map<std::string, std::string> stateIconsNight{
  {"Clear", "󰖔"},
  {"Cloudy", ""},
  {"Heavy Rain and Fog/Mist", ""},
  {"Light Rain", ""},
  {"Light Rain and Fog/Mist", ""},
  {"Mostly Clear", ""},
  {"Mostly Cloudy", ""},
  {"Partly Cloudy", ""},
  {"Rain and Fog/Mist", ""},
  {"Thunderstorms", ""},
  {"Thunderstorms and Rain", ""},
  {"Thunderstorms and Rain and Fog/Mist", ""},
};

class State {
public:
  std::string stationName{ "" };    // NOT NULL
  std::string description{ "" };    // NOT NULL
  double tempF{ INVALID_TEMP };
  double humidPct{ 0.0 };
  double windChillF{ INVALID_TEMP };
  double heatIndexF{ INVALID_TEMP };
  double barPressureIn{ 0.0 };
  std::string windDirection{ "" };
  double windSpeedMph{ 0.0 };
  std::chrono::zoned_time<std::chrono::seconds> timeStamp;
  bool daytime{ false };

  State(const Json::Value& stationState, const std::pair<double, double> coordinates, cURL::Handle& curl) {
    if(stationState["properties"].isMember("stationName")) {
      stationName = stationState["properties"]["stationName"].asString();
    }
    if(stationState["properties"].isMember("textDescription")) {
      description = stationState["properties"]["textDescription"].asString();
      if(description.empty()){
        description = "Clear";
      }
      std::string descPath{""};
      const char* homeDir = std::getenv("HOME");
      if(homeDir) 
        descPath = std::string(homeDir) + "/weather-descriptions.txt";
      else 
        descPath = "./weather-descriptions.txt";  // fallback to current directory
      appendToFile({description + '\n'}, descPath);
    }
    if(stationState["properties"].isMember("temperature")) {
      const auto& value = stationState["properties"]["temperature"]["value"];
      if(!value.isNull()) {
        double degC = value.asDouble();
        tempF = ctof(degC);
      }
    }
    if(stationState["properties"].isMember("relativeHumidity")) {
      const auto& value = stationState["properties"]["relativeHumidity"]["value"];
      if(!value.isNull()) {
        humidPct = value.asDouble();
      }
    }
    if(stationState["properties"].isMember("windChill")) {
      const auto& value = stationState["properties"]["windChill"]["value"];
      if(!value.isNull()) {
        double degC = value.asDouble();
        windChillF = ctof(degC);
      }
    }
    if(stationState["properties"].isMember("windDirection")) {
      const auto& value = stationState["properties"]["windDirection"]["value"];
      if(!value.isNull()) {
        double windDegree = value.asDouble();
        if (windDegree >= 345.0 || windDegree < 15.0)
          windDirection = "󰁅";
        else if (windDegree >= 15.0 && windDegree < 30.0)
          windDirection = "󱞥";
        else if (windDegree >= 30.0 && windDegree < 60.0)
          windDirection = "󰁂";
        else if (windDegree >= 60.0 && windDegree < 75.0)
          windDirection = "󱞡";
        else if (windDegree >= 75.0 && windDegree < 105.0)
          windDirection = "󰁍";
        else if (windDegree >= 105.0 && windDegree < 120.0)
          windDirection = "󱞽";
        else if (windDegree >= 120.0 && windDegree < 150.0)
          windDirection = "󰁛";
        else if (windDegree >= 150.0 && windDegree < 165.0)
          windDirection = "󱞧";
        else if (windDegree >= 165.0 && windDegree < 195.0)
          windDirection = "󰁝";
        else if (windDegree >= 195.0 && windDegree < 210.0)
          windDirection = "󱞫";
        else if (windDegree >= 210.0 && windDegree < 240.0)
          windDirection = "󰁜";
        else if (windDegree >= 240.0 && windDegree < 255.0)
          windDirection = "󱞿";
        else if (windDegree >= 255.0 && windDegree < 285.0)
          windDirection = "󰁔";
        else if (windDegree >= 285.0 && windDegree < 300.0)
          windDirection = "󱞣";
        else if (windDegree >= 300.0 && windDegree < 330.0)
          windDirection = "󰁃";
        else if (windDegree >= 330.0 && windDegree < 345.0)
          windDirection = "󱞩";
        else
          windDirection = "!!ERR!!";
      }
    }
    if(stationState["properties"].isMember("windSpeed")) {
      const auto& value = stationState["properties"]["windSpeed"]["value"];
      if(!value.isNull()) {
        double kmh = value.asDouble();
        windSpeedMph = kmtomi(kmh);
      }
    }
    if(stationState["properties"].isMember("barometricPressure")) {
      const auto& value = stationState["properties"]["barometricPressure"]["value"];
      if(!value.isNull()) {
        double barPa = value.asDouble();
        barPressureIn = patoin(barPa);
      }
    }
    if(stationState["properties"].isMember("heatIndex")) {
      const auto& value = stationState["properties"]["heatIndex"]["value"];
      if(!value.isNull()) {
        double degC = value.asDouble();
        heatIndexF = ctof(degC);
      }
    }
    if(stationState["properties"].isMember("timestamp")) {
      const auto& value = stationState["properties"]["timestamp"].asString();
      auto timeRet = utcToLocal(value);
      if(timeRet.has_value()) {
        timeStamp = *timeRet;
      }
    }

    std::string sunsetURL{ "https://api.sunrise-sunset.org/json?lat=" + std::to_string(coordinates.first) + "&lng=" + std::to_string(coordinates.second) + "&formatted=0" };
    auto [result, data, headers] = cURL::getData(sunsetURL, curl);
    if(result != cURL::Result::SUCCESS) {
      std::cerr << "ERROR: cURL request failed.\n";
    }
    if(data.empty()) {
      std::cerr << "ERROR: Sunset data string empty.\n";
    }
    Json::Value parsedData = JSON::parseData(data);
    if(parsedData.isMember("results")) {
      const Json::Value& results = parsedData["results"];
      if(results.isMember("sunrise") && results.isMember("sunset")) {
        // TODO: Parse SUNRISE time and compare to current time.
        // Create separate icon arrays for daytime and nighttime, compare times to set the current daytime state
        auto sunriseRet = utcToLocal(results["sunrise"].asString());
        auto sunsetRet = utcToLocal(results["sunset"].asString());
        if(sunriseRet.has_value() && sunsetRet.has_value()) {
          std::chrono::zoned_time<std::chrono::seconds> timeSunrise = *sunriseRet;
          std::chrono::zoned_time<std::chrono::seconds> timeSunset = *sunsetRet;
          if((timeStamp.get_local_time() > timeSunrise.get_local_time()) || (timeStamp.get_local_time() < timeSunset.get_local_time())) {
            daytime = true;
          }
        }
      }
    }
  }
  std::tuple<std::string, std::string, std::string> barFormat();
}; // class State
}   // namespace Weather


std::optional<std::string> getWeather(const std::pair<double, double>& coordinates);
std::optional<std::pair<std::string, std::string>> getPointsData(const std::pair<double, double>& coordinates, cURL::Handle& curl);
std::optional<std::tuple<std::string, std::string, std::string>> getCurrentConditions(const std::string& stationsURL, const std::pair<double,double> coordinates, cURL::Handle& curl);
std::optional<std::string> getClosestStation(const std::string& stationsURL, const std::pair<double,double> coordinates, cURL::Handle& curl);
std::optional<Json::Value> getStationsData(const std::string& stationsURL, cURL::Handle& curl);
void getForecastData(const std::string& forecastURL, cURL::Handle& curl);

#endif
