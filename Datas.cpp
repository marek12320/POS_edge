#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

class Datas {
public:
    std::string fileINI;  // Œcie¿ka do pliku konfiguracyjnego
    std::string input;    // Klucz Ÿród³a
    std::string output;   // Klucz celu

    
    Datas(std::string FileINI, std::string Input, std::string Output) {
        fileINI = FileINI;
        input = Input;
        output = Output;
    }

    
    void save2path(cv::Mat edges, std::string name) {
        std::string savePath = getValue(output)+"\\"+name;
        cv::imwrite(savePath, edges);
    }

    std::string getReadPath() {
        return getValue(input);
    }

private:
    std::string getValue(const std::string& key) {
        std::ifstream file(fileINI);
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string foundKey = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                if (foundKey == key) {
                    return value;
                }
            }
        }
        return "";
    }
};
