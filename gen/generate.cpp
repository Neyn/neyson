#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#define Function(TYPE, FUNC)           \
    Value(TYPE &&val);                 \
    Value(const TYPE &val);            \
    Value &operator=(TYPE &&val);      \
    Value &operator=(const TYPE &val); \
    TYPE &FUNC(TYPE &&val);            \
    TYPE &FUNC(const TYPE &val);       \
    TYPE &FUNC();                      \
    const TYPE &FUNC() const;

using namespace std;

int main()
{
    ifstream input("source");
    ofstream output("generated");
    string data(istreambuf_iterator<char>(input), {});

    vector<pair<string, string>> types;
    types.push_back({"bool", "boolean"});
    types.push_back({"Integer", "integer"});
    types.push_back({"Real", "real"});
    types.push_back({"String", "string"});
    types.push_back({"Array", "array"});
    types.push_back({"Object", "object"});

    for (auto pair : types)
    {
        auto copy = std::regex_replace(data, std::regex("TYPE"), pair.first);
        copy = std::regex_replace(copy, std::regex("FUNC"), pair.second);
        output << copy << endl << endl << endl << endl;
    }
}
