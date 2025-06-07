/**
 * @file main.cpp
 * @brief G³ówny plik wykonywalny programu do przetwarzania obrazów (Canny Edge Detection).
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "Classys.cpp"

 /**
  * @brief G³ówna funkcja programu.
  *
  * Inicjalizuje obiekty, przetwarza pliki .jpg i .png z katalogu wejœciowego i zapisuje je w katalogu wyjœciowym.
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
