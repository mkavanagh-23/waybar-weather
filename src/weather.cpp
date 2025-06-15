// TODO:
// Need to create an application identifier header to include with my requests, possibly need to alter my data utilities to accomodate this
// Use a single curl handle for Weather requests!!!
// getWeather should return both a current state and forecast objects
// Function to parse observationStations and gridPoints URLs from points request
// Function to parse list of stations retrieved via another GET request to observationStations URL
  // Extract each stations ID and coordinates
  // Should be stored in a local array/vector
// Function to calculate distance between two points to find closest station HAVERSINE formula
// Function to find the closest station
  // Loop through each station stored in the array, using the distance formula to find the closest
// Function to get the latest observations from the closest station
// Function to parse conditions, temp, etc from latest observations
// Function to parse hourly forecast from gridpoints object
