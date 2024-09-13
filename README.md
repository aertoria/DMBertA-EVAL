# ARMOR PLUS EVAL
# An Evaluation framework that evaluates NER models on their performance on Sensitive Topic information detection.

For example, BERT based NER model detects PII (Personal Identifiable Information) from a given prompt, such as Address, Name, and SocialSecurityNumber

This model calls huggingface NER models against a local evalbase. Then using a higher level model (Open AI GPT4o/claude sonnet 3.5/Gemini 1.5 Pro) as auto-rater
for the evaluation policy.

#note1: CMakeLists.txt shows the dependency needed
#note2: CMakeLists.txt read from ${VCPKG_ROOT}

vcpgk_root and vcpkg need to install
nlohmann/json.hpp

```
export VCPKG_ROOT=<your vcpkg root path>
vcpkg install nlohmann-json
```

sudo apt-get install libcurl4-openssl-dev

To execute
```
cd build/

# this step is for load vcpkgs
cmake -DCMAKE_TOOLCHAIN_FILE=<your vcpkg root path>/vcpkg.cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..

make

../evaluation_service
```

Example out put comparison of models
```
Prompt: Driver's license number D1234567 was issued to Emily Brown.
Labels:
  D1234567:23:31 : driver licence
  Emily Brown:46:57 : person
Score: 0.9
GEP return 0.5
GEP return 0.5
GEP return 0.5
  | - Inference Response [Gliner]:  
{
    "D1234567:24:32": "driver's license number",
    "Emily Brown:47:58": "person"
}
  | - Inference Response [Deberta]:  
{
    " D1234567:23:32": "pin",
    " Emily Brown.:46:59": "fullname"
}
  | - Inference Response [Spacy]:  
{
    "Driver:0:6": "org",
    "Emily Brown:47:58": "person"
}
Model: Deberta, Accumulate Score: 2.6
Model: Gliner, Accumulate Score: 3
Model: Spacy, Accumulate Score: 1.95
```

