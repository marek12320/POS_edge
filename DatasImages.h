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
 * @brief Klasa odpowiedzialna za zarz�dzanie plikiem INI oraz �cie�kami wej�cia i wyj�cia.
 *
 * Umo�liwia wczytanie �cie�ek z pliku INI, przetwarzanie plik�w graficznych oraz zapis wynik�w.
 */
class Datas {
public:
    std::string fileINI; ///< �cie�ka do pliku INI
    std::string inputDir;   ///< Klucz okre�laj�cy katalog wej�ciowy
    std::string outputDir;  ///< Klucz okre�laj�cy katalog wyj�ciowy

    /**
     * @brief Konstruktor klasy Datas.
     *
     * Inicjalizuje dane z pliku INI i rozpoczyna przetwarzanie obraz�w z rozszerzeniami .jpg i .png.
     *
     * @param FileINI �cie�ka do pliku INI.
     * @param Input Klucz w pliku INI okre�laj�cy folder wej�ciowy.
     * @param Output Klucz w pliku INI okre�laj�cy folder wyj�ciowy.
     */
    Datas(std::string FileINI, std::string Input, std::string Output);

    /**
     * @brief Zapisuje przetworzony obraz do katalogu wynikowego.
     *
     * @param edges Obiekt obrazu (np. wynik detekcji kraw�dzi).
     * @param name Nazwa pliku wynikowego.
     */
    void save2path(const cv::Mat& edges, const std::string& name);

private:
    /**
     * @brief Przetwarza obrazy w katalogu wej�ciowym o podanym rozszerzeniu.
     *
     * Tworzy osobne w�tki do przetwarzania ka�dego obrazu.
     *
     * @param extension Rozszerzenie plik�w (np. "jpg" lub "png").
     * @param images Referencja do obiektu Images do przetwarzania obraz�w.
     * @param threads Wektor aktywnych w�tk�w.
     * @param count Licznik przetworzonych obraz�w.
     */
    void processFilesWithExtension(const std::string& extension, Images& images, std::vector<std::thread>& threads, int& count);

    /**
     * @brief Odczytuje warto�� przypisan� do danego klucza z pliku INI.
     *
     * @param key Klucz, kt�rego warto�� ma zosta� pobrana.
     * @return std::string Zwraca warto�� przypisan� do klucza lub pusty string, je�li nie znaleziono.
     */
    std::string getValue(const std::string& key);
};

/**
 * @class Images
 * @brief Klasa odpowiedzialna za przetwarzanie obraz�w (np. detekcj� kraw�dzi).
 *
 * Umo�liwia asynchroniczne przetwarzanie obraz�w oraz prezentacj� ich siatki.
 */
class Images {
private:
    std::mutex coutMutex;              ///< Mutex do synchronizacji wypisywania na konsol�
    std::mutex datasMutex;             ///< Mutex do synchronizacji zapisu danych
    std::mutex imagesMutex;            ///< Mutex do synchronizacji dost�pu do obraz�w
    std::vector<cv::Mat> collectedImages; ///< Kolekcja przetworzonych obraz�w

public:
    /**
     * @brief Wy�wietla siatk� przetworzonych obraz�w w oknie OpenCV.
     */
    void showImageGrid();

    /**
     * @brief Przetwarza obraz, wykonuje detekcj� kraw�dzi i zapisuje wynik.
     *
     * @param filePath Pe�na �cie�ka do pliku obrazu.
     * @param filename Nazwa pliku.
     * @param datas Referencja do obiektu Datas, kt�ry zarz�dza zapisem obrazu.
     */
    void processImage(const std::string& filePath, const std::string& filename, Datas& datas);
};
