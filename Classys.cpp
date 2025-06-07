#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <vector>
#include <thread>
#include <mutex>

/// Forward declaration klasy Images
class Images;

/**
 * @class Datas
 * @brief Klasa odpowiedzialna za zarz�dzanie �cie�kami, plikiem INI oraz obs�ug� i zapisem obraz�w.
 */
class Datas {
public:
    std::string fileINI; ///< �cie�ka do pliku INI
    std::string input;   ///< Klucz odpowiadaj�cy katalogowi wej�ciowemu
    std::string output;  ///< Klucz odpowiadaj�cy katalogowi wyj�ciowemu

    /**
     * @brief Konstruktor klasy Datas.
     *
     * Inicjalizuje przetwarzanie obraz�w z rozszerzeniami JPG i PNG.
     *
     * @param FileINI �cie�ka do pliku INI.
     * @param Input Klucz z pliku INI okre�laj�cy katalog wej�ciowy.
     * @param Output Klucz z pliku INI okre�laj�cy katalog wyj�ciowy.
     */
    Datas(std::string FileINI, std::string Input, std::string Output);

    /**
     * @brief Zapisuje przetworzony obraz do katalogu wynikowego.
     *
     * @param edges Obraz do zapisania (np. wynik detekcji kraw�dzi).
     * @param name Nazwa pliku.
     */
    void save2path(const cv::Mat& edges, const std::string& name);

private:
    /**
     * @brief Przetwarza wszystkie pliki o okre�lonym rozszerzeniu w katalogu wej�ciowym.
     *
     * Tworzy osobne w�tki do przetwarzania ka�dego obrazu.
     *
     * @param extension Rozszerzenie plik�w (np. "jpg", "png").
     * @param images Obiekt klasy Images odpowiedzialny za przetwarzanie.
     * @param threads Wektor aktywnych w�tk�w.
     * @param count Licznik przetworzonych obraz�w.
     */
    void processFilesWithExtension(const std::string& extension, Images& images, std::vector<std::thread>& threads, int& count);

    /**
     * @brief Pobiera warto�� przypisan� do danego klucza z pliku INI.
     *
     * @param key Klucz do odczytu (np. "source", "destination").
     * @return std::string Warto�� przypisana do klucza lub pusty string, je�li nie znaleziono.
     */
    std::string getValue(const std::string& key);
};

/**
 * @class Images
 * @brief Klasa odpowiedzialna za przetwarzanie obraz�w (np. wykrywanie kraw�dzi).
 */
class Images {
private:
    std::mutex coutMutex;   ///< Mutex do synchronizacji wypisywania na konsol�
    std::mutex datasMutex;  ///< Mutex do synchronizacji operacji na danych
    std::vector<cv::Mat> collectedImages; ///< Zbi�r przetworzonych obraz�w
    std::mutex imagesMutex; ///< Mutex do synchronizacji dost�pu do kolekcji obraz�w


public:
    /**
 * @brief Tworzy siatk� z przetworzonych obraz�w i wy�wietla j� u�ytkownikowi.
 *
 * 
 */
    void showImageGrid();
    /**
     * @brief Przetwarza obraz, wykonuje detekcj� kraw�dzi (Canny) i zapisuje wynik.
     *
     * @param filePath Pe�na �cie�ka do pliku wej�ciowego.
     * @param filename Nazwa pliku.
     * @param datas Referencja do obiektu klasy Datas odpowiedzialnego za zapis.
     */
    void processImage(const std::string& filePath, const std::string& filename, Datas& datas);
};

// ============================== Implementacja Datas ==============================

Datas::Datas(std::string FileINI, std::string Input, std::string Output) {
    fileINI = FileINI;
    input = Input;
    output = Output;

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
    }
    images.showImageGrid();  

}

void Datas::save2path(const cv::Mat& edges, const std::string& name) {
    std::string savePath = getValue(output) + "\\" + name;
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
    std::string inputDir = getValue(input);
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

// ============================== Implementacja Images ==============================

void Images::processImage(const std::string& filePath, const std::string& filename, Datas& datas) {
    cv::Mat image = cv::imread(filePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Nie mozna zaladowac obrazu: " << filePath << std::endl;
        return;
    }

    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);

    {
        std::lock_guard<std::mutex> lock(imagesMutex);
        collectedImages.push_back(edges.clone());  // przechowaj kopi� obrazu
    }

    {
        std::lock_guard<std::mutex> lock(datasMutex);
        datas.save2path(edges, filename);
    }

    {
        //std::lock_guard<std::mutex> lock(coutMutex);
        //std::cout << "Zapisano: " << filename << std::endl;
    }

    


}
void Images::showImageGrid() {
    std::lock_guard<std::mutex> lock(imagesMutex);
    if (collectedImages.empty()) return;

    size_t total = collectedImages.size();
    int cols = static_cast<int>(std::ceil(std::sqrt(total)));
    int rows = static_cast<int>(std::ceil(static_cast<double>(total) / cols));

    int cellWidth = collectedImages[0].cols;
    int cellHeight = collectedImages[0].rows;

    cv::Mat grid = cv::Mat::zeros(rows * cellHeight, cols * cellWidth, collectedImages[0].type());

    for (size_t i = 0; i < total; ++i) {
        cv::Mat resized;
        cv::resize(collectedImages[i], resized, cv::Size(cellWidth, cellHeight));

        int r = static_cast<int>(i) / cols;
        int c = static_cast<int>(i) % cols;
        cv::Rect roi(c * cellWidth, r * cellHeight, cellWidth, cellHeight);

        resized.copyTo(grid(roi));
    }

    cv::namedWindow("Wszystkie obrazy", cv::WINDOW_NORMAL);
    cv::imshow("Wszystkie obrazy", grid);
    cv::waitKey(0);
    cv::destroyWindow("Wszystkie obrazy");
}

