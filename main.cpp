#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "Datas.cpp"

const unsigned int MAX_THREADS = std::thread::hardware_concurrency(); 
std::mutex coutMutex; 
std::mutex datasMutex; 

void processImage(const std::string& filePath, const std::string& filename, Datas& datas) {
    cv::Mat image = cv::imread(filePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Nie mozna zaladowac obrazu: " << filePath << std::endl;
        return;
    }

    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);

    {
        std::lock_guard<std::mutex> lock(datasMutex); 
        datas.save2path(edges, filename);
    }

    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Zapisano: " << filename << std::endl;
    }
}

void processFilesWithExtension(const std::string& inputDir, const std::string& extension, Datas& datas, std::vector<std::thread>& threads, int& count) {
    std::string searchPath = inputDir + "\\*." + extension;
    WIN32_FIND_DATAA fileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fileData);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        std::string filename = fileData.cFileName;
        std::string filePath = inputDir + "\\" + filename;

        while (threads.size() >= MAX_THREADS) {
            for (auto it = threads.begin(); it != threads.end();) {
                if (it->joinable()) {
                    it->join();
                    it = threads.erase(it);
                    count++;
                } else {
                    ++it;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        threads.emplace_back(processImage, filePath, filename, std::ref(datas));

    } while (FindNextFileA(hFind, &fileData) != 0);

    FindClose(hFind);
}

int main() {
    Datas datas("config.ini", "source", "destination");

    std::string inputDir = datas.getReadPath();
    std::vector<std::thread> threads;

    int count = 0;


    processFilesWithExtension(inputDir, "jpg", datas, threads, count);
    processFilesWithExtension(inputDir, "png", datas, threads, count);

    

    for (auto& t : threads) {
        if (t.joinable())
            t.join();
            count++;
    }

    if (count == 0) {
        std::cout << "bledna sciezka dostepu lub brak rozszerzen jpg/png" << std::endl;
    }
    else {
        std::cout << "przetworzono " << count << " obrazow"<< std::endl;
    }


    return 0;
}
