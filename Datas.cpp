#include "DatasImages.h"

Datas::Datas(std::string FileINI, std::string Input, std::string Output) {
    fileINI = FileINI;
    inputDir = getValue(Input);
    outputDir = getValue(Output);



    Images images;
    std::vector<std::thread> threads;
    int count = 0;

    processFilesWithExtension("jpg", images, threads, count);
    processFilesWithExtension("png", images, threads, count);

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
            count++;
        }
    }

    if (count == 0) {
        std::cout << "bledna sciezka dostepu lub brak rozszerzen jpg/png dla pliku " << FileINI << std::endl;
    }
    else {
        std::cout << "przetworzono " << count << " obrazow dla pliku " << FileINI << std::endl;
        images.showImageGrid();
    }

    
}

void Datas::save2path(const cv::Mat& edges, const std::string& name) {
    std::string savePath = outputDir + "\\" + name;
    cv::imwrite(savePath, edges);
}

std::string Datas::getValue(const std::string& key) {
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

void Datas::processFilesWithExtension(const std::string& extension, Images& images, std::vector<std::thread>& threads, int& count) {
    if (inputDir.empty()) return;

    std::string searchPath = inputDir + "\\*." + extension;
    WIN32_FIND_DATAA fileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fileData);
    const unsigned int MAX_THREADS = std::thread::hardware_concurrency();

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
                }
                else {
                    ++it;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        threads.emplace_back(&Images::processImage, &images, filePath, filename, std::ref(*this));

    } while (FindNextFileA(hFind, &fileData) != 0);

    FindClose(hFind);
}
