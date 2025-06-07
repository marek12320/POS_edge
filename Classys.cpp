#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <vector>
#include <thread>
#include <mutex>

/**
 * @brief Forward declaration klasy Images potrzebna do deklaracji metody z parametrem typu Images.
 */
class Images;

/**
 * @class Datas
 * @brief Klasa do zarz¹dzania œcie¿kami i konfiguracj¹ plików INI oraz zapisywaniem wyników.
 */
class Datas {
public:
    std::string fileINI;  ///< Nazwa pliku konfiguracyjnego INI
    std::string input;    ///< Klucz katalogu wejœciowego w INI
    std::string output;   ///< Klucz katalogu wyjœciowego w INI

    /**
     * @brief Konstruktor klasy Datas.
     * @param FileINI Nazwa pliku INI.
     * @param Input Klucz dla œcie¿ki wejœciowej.
     * @param Output Klucz dla œcie¿ki wyjœciowej.
     */
    Datas(std::string FileINI, std::string Input, std::string Output);

    /**
     * @brief Zapisuje przetworzony obraz do œcie¿ki wyjœciowej.
     * @param edges Obraz wynikowy (np. po Canny).
     * @param name Nazwa pliku.
     */
    void save2path(const cv::Mat& edges, const std::string& name);

    /**
     * @brief Pobiera œcie¿kê do katalogu wejœciowego z pliku INI.
     * @return Œcie¿ka do katalogu wejœciowego.
     */
    std::string getReadPath();

    /**
     * @brief Przetwarza wszystkie pliki z danym rozszerzeniem w folderze.
     *
     * @param extension Rozszerzenie plików do przetworzenia (np. "jpg").
     * @param images Referencja do obiektu klasy Images.
     * @param threads Wektor w¹tków do przetwarzania.
     * @param count Licznik przetworzonych obrazów.
     */
    void processFilesWithExtension(const std::string& extension, Images& images, std::vector<std::thread>& threads, int& count);

private:
    /**
     * @brief Pobiera wartoœæ przypisan¹ do danego klucza z pliku INI.
     * @param key Klucz w pliku INI.
     * @return Wartoœæ przypisana do klucza.
     */
    std::string getValue(const std::string& key);
};

/**
 * @class Images
 * @brief Klasa odpowiedzialna za przetwarzanie obrazów (np. Canny edge detection).
 */
class Images {
private:
    std::mutex coutMutex;   ///< Mutex do bezpiecznego wypisywania do konsoli.
    std::mutex datasMutex;  ///< Mutex do bezpiecznego zapisu obrazów.

public:
    /**
     * @brief Przetwarza pojedynczy obraz (Canny edge detection) i zapisuje go.
     *
     * @param filePath Pe³na œcie¿ka do pliku obrazu.
     * @param filename Nazwa pliku obrazu.
     * @param datas Referencja do obiektu klasy Datas (do zapisu wyniku).
     */
    void processImage(const std::string& filePath, const std::string& filename, Datas& datas);
};

// ---------- Implementacja metod Datas ----------

Datas::Datas(std::string FileINI, std::string Input, std::string Output) {
    fileINI = FileINI;
    input = Input;
    output = Output;
}

void Datas::save2path(const cv::Mat& edges, const std::string& name) {
    std::string savePath = getValue(output) + "\\" + name;
    cv::imwrite(savePath, edges);
}

std::string Datas::getReadPath() {
    return getValue(input);
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
    std::string inputDir = getReadPath();
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
