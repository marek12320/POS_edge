/**
 * @file main.cpp
 * @brief Glowny plik wykonywalny programu do przetwarzania obrazow (Canny Edge Detection).
 */

#include "DatasImages.h"

 /**
  * @brief Glowna funkcja programu.
  *
  * Inicjalizuje obiekty, przetwarza pliki .jpg i .png z katalogu wejsciowego i zapisuje je w katalogu wyjsciowym.
  */
int main() {

    std::cout << "test1" << std::endl;
    Datas datas("config.ini", "source", "destination");//idealny scenariusz


    std::cout << std::endl << "test2" << std::endl;
    Datas datas2("config.ini", "source", "testowy2");//brak folderu output


    std::cout << std::endl << "test3" << std::endl;
    Datas datas3("config.ini", "testowy", "destination");//brak folderu input


    std::cout << std::endl << "test4" << std::endl;
    Datas datas4("CMakeLists.txt", "testowy", "testowy2");//inny plik wej


    return 0;
}
