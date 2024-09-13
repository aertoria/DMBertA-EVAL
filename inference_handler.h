#ifndef _INFERENCE_HANDLER_H_
#define _INFERENCE_HANDLER_H_

#include <curl/curl.h>
#include <string>

class InferenceHandler {
  private:
      CURL *curl;
      const std::string requestUrl;
      struct curl_slist *headers = NULL;

      // Static callback function to handle data received from the server
      static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
          s->append((char*)contents, size * nmemb);
          return size * nmemb;
      }

   public:
    InferenceHandler(const std::string url);

    ~InferenceHandler();

    std::string PostCurl(const std::string& data);
};

#endif //_INFERENCE_HANDLER_H_