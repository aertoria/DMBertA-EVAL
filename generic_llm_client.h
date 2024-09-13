#ifndef GENERIC_LLM_CLIENT_H
#define GENERIC_LLM_CLIENT_H

#include <string>

class GenericLLMClient {
public:
    GenericLLMClient(const std::string& apiKey);

    std::string getResponse(const std::string& prompt);

private:
    std::string apiKey_;

    struct curl_slist* getHeaders();
    std::string getPostFields(const std::string& prompt);
    static std::string parseResponse(const std::string& response);
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif // GENERIC_LLM_CLIENT_H
