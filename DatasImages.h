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
 * @brief Klasa odpowiedzialna za zarzadzanie plikiem INI oraz sciezkami wejsciowa i wyjsciowa.
 *
 * Umozliwia wczytanie sciezek z pliku INI, przetwarzanie plikow graficznych oraz zapis wynikow.
 */
class Datas {
public:
    std::string fileINI;    ///< Sciezka do pliku INI
    std::string inputDir;   ///< Sciezka do katalogu wejsciowego
    std::string outputDir;  ///< Sciezka do katalogu wyjsciowego

    /**
     * @brief Konstruktor klasy Datas.
     *
     * Inicjalizuje dane z pliku INI i rozpoczyna przetwarzanie obrazow z rozszerzeniami .jpg i .png.
     *
     * @param FileINI Sciezka do pliku INI.
     * @param Input Klucz okreslajacy katalog wejsciowy.
     * @param Output Klucz okreslajacy katalog wyjsciowy.
     */
    Datas(std::string FileINI, std::string Input, std::string Output);

    /**
     * @brief Zapisuje przetworzony obraz do katalogu wynikowego.
     *
     * @param obraz Obiekt obrazu (np. wynik detekcji krawedzi).
     * @param name Nazwa pliku wynikowego.
     */
    void save2path(const cv::Mat& obraz, const std::string& name);

private:
    /**
     * @brief Przetwarza obrazy w katalogu wejsciowym o podanym rozszerzeniu.
     *
     * Tworzy osobne watki do przetwarzania kazdego obrazu.
     *
     * @param extension Rozszerzenie plikow (np. "jpg" lub "png").
     * @param images Referencja do obiektu Images odpowiedzialnego za przetwarzanie obrazow.
     * @param threads Wektor aktywnych watkow.
     * @param count Licznik przetworzonych obrazow.
     */
    void processFilesWithExtension(const std::string& extension, Images& images, std::vector<std::thread>& threads, int& count);

    /**
     * @brief Odczytuje wartosc przypisana do danego klucza z pliku INI.
     *
     * @param key Klucz, ktorego wartosc ma zostac pobrana.
     * @return std::string Wartosc przypisana do klucza lub pusty string, jesli nie znaleziono.
     */
    std::string getValue(const std::string& key);
};

/**
 * @class Images
 * @brief Klasa odpowiedzialna za przetwarzanie obrazow (np. detekcje krawedzi).
 *
 * Umozliwia asynchroniczne przetwarzanie obrazow oraz prezentacje ich siatki.
 */
class Images {
private:
    std::mutex coutMutex;               ///< Mutex do synchronizacji wypisywania na konsole
    std::mutex datasMutex;              ///< Mutex do synchronizacji zapisu danych
    std::mutex imagesMutex;             ///< Mutex do synchronizacji dostepu do obrazow
    std::vector<cv::Mat> collectedImages; ///< Kolekcja przetworzonych obrazow
    std::vector<cv::Mat> originalImages;  ///< Kolekcja oryginalnych obrazow

public:
    /**
     * @brief Wyswietla siatke przetworzonych i orginalnych obrazow w oknie OpenCV.
     *
     * @param datas Referencja do obiektu Datas, opcjonalnie uzywana przy prezentacji.
     */
    void showImageGrid(Datas& datas);

    /**
     * @brief Przetwarza obraz, wykonuje detekcje krawedzi i zapisuje wynik.
     *
     * @param filePath Pelna sciezka do pliku obrazu.
     * @param filename Nazwa pliku.
     * @param datas Referencja do obiektu Datas, ktory zarzadza zapisem obrazu.
     */
    void processImage(const std::string& filePath, const std::string& filename, Datas& datas);
};
