#include <unistd.h>
#include "inference_handler.h"
#include "eval_record.h"
#include <nlohmann/json.hpp>   // Include the nlohmann/json library
#include "generic_llm_client.h"
#include <iostream>

std::string fix_json_string(const std::string& rawString) {
    std::string fixedString = rawString;
    std::replace(fixedString.begin(), fixedString.end(), '\'', '"');
    return fixedString;
}


std::string extract_labels(const std::string rawString) {

  try {

      // Fix the JSON string by replacing single quotes with double quotes
      std::string fixedJsonString = fix_json_string(rawString);

      // Parse the outer JSON object
      nlohmann::json outerJsonObject = nlohmann::json::parse(rawString);

      // Extract and parse the "message" field as a JSON string
      nlohmann::json innerJsonObject = nlohmann::json::parse(outerJsonObject["message"].get<std::string>());

      // Extract the "labels" field
      auto labels = innerJsonObject["labels"];
      return labels.dump(4);

  } catch (const nlohmann::json::exception& e) {

      std::cerr << "raw string" << rawString << "JSON parsing error: " << e.what() << std::endl;
      return "{}";  // Return an empty JSON object on error
  }
}


float generative_evaluate_performance(const std::string label_labeled,const std::string label_inferred){
  GenericLLMClient client("<apikey>");


  std::string prompt = "label_labled is the ground truth of Personal identifiable information. label_inferred is output of models. I want "
                       "to evaluate the performance of those models. So please calculate how comprehensive and accurate is label_inferred, in comparison with label_labled,"
                       "note, the closer the better; ideally label_inferred as out put should cover everything label_labeled has, the string get labeled should roughly same, and the labels should be in the same category "
                       "the result should be a score between 0 to 1, 1 being the most accurate. 0 being not accurate at all"
                       "if the label_inferred is unreadable, not meaningful, empty, feel free to assign it a very low value"
                       "do not explain, do not reasoning, just output a single digit score as float, no string or literals around it";

  prompt += " label_labeled is following: " + label_labeled;
  prompt += " label_inferred is following: " + label_inferred;

  std::string response = client.getResponse(prompt);
  return std::stof(response);
}

// The entrance of start evaluation
int main() {
  // Print the current location
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
     std::cout << "Current working directory: " << cwd << std::endl;
  }

  // Start loading the JSON
  EvalRecordMultiple eval_record_multiple=EvalRecordMultiple();
  eval_record_multiple.loadEvalRecords("../txtbase/evalbase.json");//counting starting from evaluation_service/cmake-build-debug


  // Initialize the InferenceHandler with the request URL
  InferenceHandler
      handler_shieldgemma("http://127.0.0.1:9092/inference_outbound_traffic_shieldgemma");

  InferenceHandler
      handler_gliner("http://127.0.0.1:9092/inference_outbound_traffic_gliner");

  InferenceHandler
      handler_deberta("http://127.0.0.1:9092/inference_outbound_traffic_deberta");

  InferenceHandler
      handler_spacy("http://127.0.0.1:9092/inference_outbound_traffic_spacy");

  InferenceHandler
      handler_i2b2("http://127.0.0.1:9092/inference_outbound_traffic_i2b2");



  // make a map
  std::map<std::string, float> evaluationScore;

  // Insert elements into the map
  evaluationScore["Gliner"] = 0.0;
  evaluationScore["Deberta"] = 0.0;
  evaluationScore["Spacy"] = 0.0;


  //  Print the records for verification
  for (const EvalRecordOne& record : eval_record_multiple.getRecords()) {
      record.printRecord();

      evaluationScore["Gliner"] += generative_evaluate_performance(record.getLabelAsString(),extract_labels(handler_gliner.PostCurl(record.getPrompt())));
      evaluationScore["Deberta"] += generative_evaluate_performance(record.getLabelAsString(),extract_labels(handler_deberta.PostCurl(record.getPrompt())));
      evaluationScore["Spacy"] += generative_evaluate_performance(record.getLabelAsString(),extract_labels(handler_spacy.PostCurl(record.getPrompt())));

      for (const auto& kvPair: evaluationScore){
        std::cout << "Model: "<< kvPair.first << ", Accumulate Score: "<<kvPair.second<<std::endl;
      }

  }

  //loop through map
  for (const auto& kvPair: evaluationScore){
    std::cout << "Model: "<< kvPair.first << ", Accumulate Score: "<<kvPair.second<<std::endl;
  }

  return 0;
}

