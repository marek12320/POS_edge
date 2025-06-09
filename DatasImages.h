#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <vector>
#include <thread>
#include <mutex>

class Images;  ///< Forward declaration klasy Images

/**
 * @class Datas
 * @brief Klasa odpowiedzialna za zarz¹dzanie plikiem INI oraz œcie¿kami wejœcia i wyjœcia.
 *
 * Umo¿liwia wczytanie œcie¿ek z pliku INI, przetwarzanie plików graficznych oraz zapis wyników.
 */
class Datas {
public:
    std::string fileINI; ///< Œcie¿ka do pliku INI
    std::string inputDir;   ///< Klucz okreœlaj¹cy katalog wejœciowy
    std::string outputDir;  ///< Klucz okreœlaj¹cy katalog wyjœciowy

    /**
     * @brief Konstruktor klasy Datas.
     *
     * Inicjalizuje dane z pliku INI i rozpoczyna przetwarzanie obrazów z rozszerzeniami .jpg i .png.
     *
     * @param FileINI Œcie¿ka do pliku INI.
     * @param Input Klucz w pliku INI okreœlaj¹cy folder wejœciowy.
     * @param Output Klucz w pliku INI okreœlaj¹cy folder wyjœciowy.
     */
    Datas(std::string FileINI, std::string Input, std::string Output);

    /**
     * @brief Zapisuje przetworzony obraz do katalogu wynikowego.
     *
     * @param edges Obiekt obrazu (np. wynik detekcji krawêdzi).
     * @param name Nazwa pliku wynikowego.
     */
    void save2path(const cv::Mat& edges, const std::string& name);

private:
    /**
     * @brief Przetwarza obrazy w katalogu wejœciowym o podanym rozszerzeniu.
     *
     * Tworzy osobne w¹tki do przetwarzania ka¿dego obrazu.
     *
     * @param extension Rozszerzenie plików (np. "jpg" lub "png").
     * @param images Referencja do obiektu Images do przetwarzania obrazów.
     * @param threads Wektor aktywnych w¹tków.
     * @param count Licznik przetworzonych obrazów.
     */
    void processFilesWithExtension(const std::string& extension, Images& images, std::vector<std::thread>& threads, int& count);

    /**
     * @brief Odczytuje wartoœæ przypisan¹ do danego klucza z pliku INI.
     *
     * @param key Klucz, którego wartoœæ ma zostaæ pobrana.
     * @return std::string Zwraca wartoœæ przypisan¹ do klucza lub pusty string, jeœli nie znaleziono.
     */
    std::string getValue(const std::string& key);
};

/**
 * @class Images
 * @brief Klasa odpowiedzialna za przetwarzanie obrazów (np. detekcjê krawêdzi).
 *
 * Umo¿liwia asynchroniczne przetwarzanie obrazów oraz prezentacjê ich siatki.
 */
class Images {
private:
    std::mutex coutMutex;              ///< Mutex do synchronizacji wypisywania na konsolê
    std::mutex datasMutex;             ///< Mutex do synchronizacji zapisu danych
    std::mutex imagesMutex;            ///< Mutex do synchronizacji dostêpu do obrazów
    std::vector<cv::Mat> collectedImages; ///< Kolekcja przetworzonych obrazów

public:
    /**
     * @brief Wyœwietla siatkê przetworzonych obrazów w oknie OpenCV.
     */
    void showImageGrid();

    /**
     * @brief Przetwarza obraz, wykonuje detekcjê krawêdzi i zapisuje wynik.
     *
     * @param filePath Pe³na œcie¿ka do pliku obrazu.
     * @param filename Nazwa pliku.
     * @param datas Referencja do obiektu Datas, który zarz¹dza zapisem obrazu.
     */
    void processImage(const std::string& filePath, const std::string& filename, Datas& datas);
};
