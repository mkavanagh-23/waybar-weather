#ifndef DATA_H
#define DATA_H

#include <curl/curl.h>
#include <json/json.h>
#include <string>
#include <vector>
#include <tuple>
#include <ctime>


namespace cURL {

enum class Result {
  SUCCESS,
  UNSUPPORTED_PROTOCOL,
  INIT_FAILED,
  BAD_URL,
  TIMEOUT,
  REQUEST_FAILED
};

// Create a handler for CURL* objects to maintain RAII
class Handle {
private:
  CURL* handle;     // Handler manages a CURL*
public:
  Handle() : handle(curl_easy_init()) {}    // Initialize curl memory on construction
  ~Handle() { if(handle) curl_easy_cleanup(handle); }   // And delete it when we go out of scope

  CURL* get() { return handle; }    // Return the pointer
  operator bool() const { return handle != nullptr; }   // Check for valid intiialization

  // Explicitly delete the copy constructor and assignment operators
  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;
};

// Callback function for writing the header data
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata);
// Callback function for writing the result data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
// Fetch and process data from remote url (Result, Data, Headers)
std::tuple<Result, std::string, std::vector<std::string>> getData(const std::string& url, Handle& curl);
// POST data to a remote endpoint (Result, Data, Headers)
std::tuple<Result, std::string, std::vector<std::string>> postData(const std::string& url, const std::string& postData, Handle& curl);
// Extract the content type from the response headers
std::string getContentType(const std::vector<std::string>& headers);

} // namespace cURL

namespace JSON {
Json::Value parseData(const std::string& jsonData);
} // namespace JSON

double ctof(double degreesC);
double kmtomi(double kilometers);
double patoin(double pascals);

#endif
