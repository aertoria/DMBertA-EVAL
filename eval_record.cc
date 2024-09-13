#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <curl/curl.h>
#include "inference_handler.h"
#include "eval_record.h"
#include <fstream>
#include <nlohmann/json.hpp>   // Include the nlohmann/json library

// EvalRecord class definition
EvalRecordOne::EvalRecordOne(const std::string& p,
             const std::map<std::string, std::string>& l,
             double s):prompt(p),label(l),score(s) {
}

// Method to print the record (for debugging purposes)
void EvalRecordOne::printRecord() const {
    std::cout << "------------------\n";
    std::cout << "Prompt: " << prompt << "\n";
    std::cout << "Labels:\n";
    for (const auto& entry : label) {
        std::cout << "  " << entry.first << " : " << entry.second << "\n";
    }
    std::cout << "Score: " << score << "\n";
}

std::string EvalRecordOne::getPrompt() const{
  return prompt;
}

std::map<std::string, std::string> EvalRecordOne::getLabel() const {
  return label;
}

std::string EvalRecordOne::getLabelAsString() const{
    std::string result;
    for (const auto& pair : label) {
        if (!result.empty()) {
            result += "&";  // Use any delimiter you like
        }
        result += pair.first + "=" + pair.second;
    }
    return result;
}

double EvalRecordOne::getScore() const{
  return score;
}


EvalRecordMultiple::EvalRecordMultiple(){
};

// Function to load a JSON file and parse it into a vector of EvalRecord objects
void EvalRecordMultiple::loadEvalRecords(const std::string& filename) {
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    nlohmann::json j;
    file >> j;

    for (const auto& entry : j) {
        std::string prompt = entry.at("prompt");
        std::map<std::string, std::string> labels;
        for (const auto& label_entry : entry.at("labels").items()) {
            labels[label_entry.key()] = label_entry.value();
        }
        double score = entry.at("score");

        records.emplace_back(EvalRecordOne(prompt, labels, score));
    }
}

const std::vector<EvalRecordOne>& EvalRecordMultiple::getRecords(){
  return records;
}