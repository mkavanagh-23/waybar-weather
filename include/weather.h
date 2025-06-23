#ifndef WEATHER_H
#define WEATHER_H

#include "data.h"
#include <chrono>
#include <json/value.h>
#include <optional>
#include <string>
#include <utility>

namespace Weather {

const double INVALID_TEMP { 5000 }; // Check for NULL values

class State {
public:
  std::string stationName{ "" };    // NOT NULL
  std::string description{ "" };    // NOT NULL
  double tempF{ INVALID_TEMP };
  double humidPct{ 0.0 };
  double windChillF{ INVALID_TEMP };
  double heatIndexF{ INVALID_TEMP };
  double barPressureIn{ 0.0 };
  double windDegree{ 0 };
  double windSpeedMph{ 0.0 };
  std::chrono::zoned_time<std::chrono::seconds> timeStamp;

  State(const Json::Value& stationState)
  {
    if(stationState["properties"].isMember("stationName")) {
      stationName = stationState["properties"]["stationName"].asString();
    }
    if(stationState["properties"].isMember("textDescription")) {
      description = stationState["properties"]["textDescription"].asString();
      if(description.empty()){
        description = "Clear";
      }
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
        windDegree = value.asDouble();
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
