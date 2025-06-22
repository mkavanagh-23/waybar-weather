#ifndef WEATHER_H
#define WEATHER_H

#include "data.h"
#include <json/value.h>
#include <optional>
#include <string>
#include <utility>


std::optional<std::string> getWeather(const std::pair<double, double>& coordinates);
std::optional<std::pair<std::string, std::string>> getPointsData(const std::pair<double, double>& coordinates, cURL::Handle& curl);
void getCurrentConditions(const std::string& stationsURL, cURL::Handle& curl);
std::optional<std::string> getClosestStation(const std::string& stationsURL, cURL::Handle& curl);
std::optional<Json::Value> getStationsData(const std::string& stationsURL, cURL::Handle& curl);
void getForecastData(const std::string& forecastURL, cURL::Handle& curl);


#endif
