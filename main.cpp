/**
 * @file main.cpp
 * @brief G��wny plik wykonywalny programu do przetwarzania obraz�w (Canny Edge Detection).
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "Classys.cpp"

 /**
  * @brief G��wna funkcja programu.
  *
  * Inicjalizuje obiekty, przetwarza pliki .jpg i .png z katalogu wej�ciowego i zapisuje je w katalogu wyj�ciowym.
  */
int main() {
    Datas datas("config.ini", "source", "destination");
    Images images;

    std::vector<std::thread> threads;
    int count = 0;

    datas.processFilesWithExtension("jpg", images, threads, count);
    datas.processFilesWithExtension("png", images, threads, count);

    for (auto& t : threads) {
        if (t.joinable())
            t.join();
        count++;
    }

    if (count == 0) {
        std::cout << "bledna sciezka dostepu lub brak rozszerzen jpg/png" << std::endl;
    }
    else {
        std::cout << "przetworzono " << count << " obrazow" << std::endl;
    }

    return 0;
}
