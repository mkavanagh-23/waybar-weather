#ifndef WEATHER_H
#define WEATHER_H

#include "data.h"
#include <optional>
#include <string>
#include <utility>


std::optional<std::string> getWeather(std::pair<double, double> coordinates);
std::optional<std::pair<std::string, std::string>> getPointsData(std::pair<double, double> coordinates, cURL::Handle& curl);
void getStationsData(const std::string& stationsURL, cURL::Handle& curl);
void getForecastData(const std::string& forecastURL, cURL::Handle& curl);


#endif
