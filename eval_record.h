#ifndef EVAL_RECORD_H
#define EVAL_RECORD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include "inference_handler.h"

//Example
//("Ethan is working with a patient who lives in Palo Alto, CA.",
//{
//  {"patient:5:6", "PII"},
//  {"Palo Alto:9:11", "location"}
//},
//0.9)
//
//Explaination:
//("original prompt",
//{
//  "original_txt:start_idx:end_idx", "label"
//},
//score_of_liklyhood_prompt_contains_PII
//)
// Define the EvalRecord class
class EvalRecordOne {
 private:
    const std::string prompt;
    const std::map<std::string, std::string> label;
    const double score;

 public:
    // Constructor
    EvalRecordOne(const std::string& p,
             const std::map<std::string, std::string>& l,
             double s);

    // Method to print the record (for debugging purposes)
    void printRecord() const;

    std::string getPrompt() const;

    std::map<std::string, std::string> getLabel() const;

    std::string getLabelAsString() const;

    double getScore() const;
};


class EvalRecordMultiple{
 private:
    std::vector<EvalRecordOne> records;

 public:
    //Constructor
    EvalRecordMultiple();

    // Function to load a JSON file and parse it into a vector of EvalRecord objects
    void loadEvalRecords(const std::string& filename);

    const std::vector<EvalRecordOne>& getRecords();
};


#endif // EVAL_RECORD_H
