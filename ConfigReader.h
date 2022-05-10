#pragma once
#include <unordered_map>
#include <string>
#include <fstream>

class ConfigReader
{
public:
	static std::unordered_map<std::string, std::string> readConfigFile(char* filepath) {
        std::unordered_map<std::string, std::string> to_return;

        std::string line;
        std::ifstream config_file(filepath);
		if (config_file.is_open()) {
			while (getline(config_file, line)) {
                size_t colon_loc = line.find(":");
                if (colon_loc == std::string::npos) {
                    fprintf(stderr, "Missing colon in config file %s\n", filepath);
                    throw std::invalid_argument("Missing colon");
                }
                std::string key = line.substr(0, colon_loc);
                key.erase(std::remove_if(key.begin(), key.end(), std::isspace), key.end());

                std::string value = line.substr(colon_loc + 1);
                value.erase(std::remove_if(value.begin(), value.end(), std::isspace), value.end());

                to_return[key] = value;
            }
            config_file.close();
		}
		else {
            fprintf(stderr, "Unable to open %s\n", filepath);
            throw std::invalid_argument("Unable to open file");
        }

        return to_return;
	}
};

