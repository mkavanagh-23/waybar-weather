#include "data.h"
#include <ctime>
#include <string>
#include <vector>
#include <tuple>
#include <cstdlib>
#include <curl/curl.h>
#include <json/json.h>

namespace cURL {

// Write the header data
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
  size_t totalSize{ size * nitems };
  std::vector<std::string>* headers = static_cast<std::vector<std::string>*>(userdata);

  std::string header(buffer, totalSize);
  headers->push_back(header);

  return totalSize;
}

// Write the data to the string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize{ size * nmemb };    // Calculate the size of the callback buffer
  output->append((char*)contents, totalSize); // Store contents of callback buffer in output
  return totalSize;
}

// Fetch a data string from a remote source
std::tuple<Result, std::string, std::vector<std::string>> getData(const std::string& url, Handle& curl){
  std::string responseData;         // Create a string to hold the data
  std::vector<std::string> headers; // Create a vector to hold the response headers
  
  // Check for successful initialization
  if(!curl) {
    return { Result::INIT_FAILED, "", {} };
  }
  
  // Set cURL options
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str()); // Set the cURL url
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseData);     // Set the data to write
  curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 10L); // Set a 10 second timeout
  curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L); // Optional, depending on your SSL setup
  
  // Check for user agent
  const char* USER_AGENT = std::getenv("USER_AGENT");
  if(USER_AGENT != nullptr) {
    curl_easy_setopt(curl.get(), CURLOPT_USERAGENT, USER_AGENT); // Set the custom user agent (required for api.weather.gov)
  }

  // Set the cookies location
  curl_easy_setopt(curl.get(), CURLOPT_COOKIEFILE, "");     // Enable in-memory cookie management
  
  // Set up header handling
  curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, HeaderCallback); // Custom function to capture headers
  curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &headers);           // Pass headers vector to function
    
  // Retrieve the data
  CURLcode res = curl_easy_perform(curl.get());

  // Check for errors
  if(res != CURLE_OK) {
    std::string err = curl_easy_strerror(res);
    std::string errMsg = "Error retrieving data (" + err + ")";
    
    if(res == CURLE_UNSUPPORTED_PROTOCOL)
      return { Result::UNSUPPORTED_PROTOCOL, "", {} };
    else if(res == CURLE_URL_MALFORMAT)
      return { Result::BAD_URL, "", {} };
    else if(res == CURLE_OPERATION_TIMEDOUT)
      return { Result::TIMEOUT, "", {} };
    else
      return { Result::REQUEST_FAILED, "", {} };
  }
  return { Result::SUCCESS, responseData, headers };
}

// POST data to a remote endpoint
std::tuple<Result, std::string, std::vector<std::string>> postData(const std::string& url, const std::string& postData, Handle& curl) {
  std::string responseData;         // Create a string to hold the data
  std::vector<std::string> headers; // Create a vector to hold the response headers
  
  // Check for successful initialization
  if(!curl) {
    return { Result::INIT_FAILED, "", {} };
  }
  
  // Set cURL options
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str()); // Set the cURL url
  curl_easy_setopt(curl.get(), CURLOPT_POST, 1L); // Set the cURL POST method
  curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, postData.c_str()); // Set the POST data
  curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 10L); // Set a 10 second timeout
  curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L); // Optional, depending on your SSL setup
  
  // Set the cookies location
  curl_easy_setopt(curl.get(), CURLOPT_COOKIEFILE, "");     // Enable in-memory cookie management

  // Write the callback data 
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseData);     // Set the data to write

  // And write the header response
  curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, HeaderCallback); // Custom function to capture headers
  curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &headers);           // Pass headers vector to function

  // POST the data
  CURLcode res = curl_easy_perform(curl.get());
  
  // Check for errors
  if(res != CURLE_OK) {
    std::string err = curl_easy_strerror(res);
    std::string errMsg = "Error sending data (" + err + ")";
    
    if(res == CURLE_UNSUPPORTED_PROTOCOL)
      return { Result::UNSUPPORTED_PROTOCOL, "", {} };
    else if(res == CURLE_URL_MALFORMAT)
      return { Result::BAD_URL, "", {} };
    else if(res == CURLE_OPERATION_TIMEDOUT)
      return { Result::TIMEOUT, "", {} };
    else
      return { Result::REQUEST_FAILED, "", {} };
  }
  return { Result::SUCCESS, responseData, headers };
}

// Extract the content-type header from the response
std::string getContentType(const std::vector<std::string>& headers) {
  // Iterate through each header
  for(const auto& header : headers) {
    // Check for the "Content-Type" header
    if(header.find("content-type") != std::string::npos || header.find("Content-Type") != std::string::npos) {
      // Set an iterator to the start of the value string
      size_t pos = header.find(":");
      // Check if the key has a value
      if(pos != std::string::npos) {
        return header.substr(pos + 1);  // Ectract the value that follows the ":"
      }
    }
  }
  return "";  // Return an empty string if the header wasn't found
}

} // namespace cURL

namespace JSON {
// Parse events from a Json data stream
Json::Value parseData(const std::string& jsonData) {
  // Set up Json parsing objects
  Json::CharReaderBuilder builder;
  Json::Value root;                 // Root node of the parsed objects
  std::istringstream data(jsonData);
  std::string errs;                 // Hold errors in a string

  // Parse the string into the root Value object
  if(!Json::parseFromStream(builder, data, &root, &errs)) {
    // If initial parsing fails, send an error message
    std::string errMsg = "Parsing error (\"" + errs + "\")";
  }
  return root; // Return the parsed root of objects
}
} // namespace JSON

double ctof(double degreesC) {
  return degreesC * (9.0 / 5.0) + 32;
}

double kmtomi(double kilometers) {
  return kilometers * (.062137);
}

double patoin(double pascals) {
  return pascals * (.0040146307866177);
}
