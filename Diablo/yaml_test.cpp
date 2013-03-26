#include <fstream>
#include "yaml-cpp/yaml.h"

int main()
{
    std::ifstream fin("test.yml");
    YAML::Parser parser(fin);

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
       // ...
    }

    return 0;
}