#ifndef WEATHER_H
#define WEATHER_H

#include "data.h"
#include <json/value.h>
#include <optional>
#include <string>
#include <utility>


std::optional<std::string> getWeather(std::pair<double, double> coordinates);
std::optional<std::pair<std::string, std::string>> getPointsData(std::pair<double, double> coordinates, cURL::Handle& curl);
void getCurrentConditions(std::string stationID, cURL::Handle& curl);
void getClosestStation(const std::string& stationsURL, cURL::Handle& curl);
std::optional<Json::Value> getStationsData(const std::string& stationsURL, cURL::Handle& curl);
void getForecastData(const std::string& forecastURL, cURL::Handle& curl);


#endif
