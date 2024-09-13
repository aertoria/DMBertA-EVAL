#include "inference_handler.h"
#include <iostream>
#include <curl/curl.h>

//constructor
InferenceHandler::InferenceHandler(const std::string url):requestUrl(url){
  curl = curl_easy_init();
  headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
}

// Destructor to clean up resources
InferenceHandler::~InferenceHandler() {
    if (curl) {
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
}

// Convert any raw incoming data to json format
std::string prepare_inbound_traffic_json(const std::string& inbound_traffic_content){
  std::string json_data = R"({"inbound_traffic": ")" + inbound_traffic_content + R"("})";
  return json_data;
}

std::string InferenceHandler::PostCurl(const std::string& data){
  std::string raw_data = prepare_inbound_traffic_json(data);

  if(!curl) {
    return "Failed to initialize CURL";
  }

  std::string responseString;

  // Curl has be initialized
  // Set URL
  curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());

  // Set headers
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // Set POST data
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, raw_data.c_str());

  // Set up the callback function to capture the response
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

  // Perform the request, res will get the return code
  CURLcode res = curl_easy_perform(curl);

  // Check for errors
  if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  }

  return responseString;
}