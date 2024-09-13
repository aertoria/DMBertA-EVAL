#include "generic_llm_client.h"
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>  // Requires installing the jsoncpp library

GenericLLMClient::GenericLLMClient(const std::string& apiKey) : apiKey_(apiKey) {}

std::string GenericLLMClient::getResponse(const std::string& prompt) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string postFields = getPostFields(prompt);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, getHeaders());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postFields.size());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
            return "";
        } else {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code != 200) {
                std::cerr << "API request failed with HTTP code " << http_code << std::endl;
                std::cerr << "Response: " << readBuffer << std::endl;
                return "";
            }
        }
    }

    return parseResponse(readBuffer);
}

struct curl_slist* GenericLLMClient::getHeaders() {
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    return headers;
}

// Method to create the JSON body for the API request
std::string GenericLLMClient::getPostFields(const std::string& prompt) {
    Json::Value root;
    root["model"] = "gpt-3.5-turbo";  // Updated model
    root["messages"] = Json::arrayValue;  // `gpt-3.5-turbo` expects a list of messages

    Json::Value message;
    message["role"] = "user";
    message["content"] = prompt;
    root["messages"].append(message);

    root["max_tokens"] = 100;  // Adjust as needed

    Json::StreamWriterBuilder writer;
    std::string postFields = Json::writeString(writer, root);
    return postFields;
}


// Method to parse the response from the API
std::string GenericLLMClient::parseResponse(const std::string& response) {
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errs;

    std::istringstream s(response);
    std::string output;

    if (Json::parseFromStream(reader, s, &root, &errs)) {
        if (root.isMember("choices") && root["choices"].isArray()) {
            output = root["choices"][0]["message"]["content"].asString();
        } else {
            std::cerr << "Unexpected response format: " << response << std::endl;
        }
    } else {
        std::cerr << "Failed to parse JSON: " << errs << std::endl;
    }

    return output;
}

// cURL callback function to write the response data to a string
size_t GenericLLMClient::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}