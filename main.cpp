/**
 * @file main.cpp
 * @brief G��wny plik wykonywalny programu do przetwarzania obraz�w (Canny Edge Detection).
 */

#include "DatasImages.h"

/**
 * @brief G��wna funkcja programu.
 * 
 * Inicjalizuje obiekty, przetwarza pliki .jpg i .png z katalogu wej�ciowego i zapisuje je w katalogu wyj�ciowym.
 */
int main() {

    std::cout << "test1" << std::endl;
    Datas datas("config.ini", "source", "destination");//idealny scenariusz


    std::cout << std::endl <<  "test2" << std::endl;
    Datas datas2("config.ini", "source", "testowy2");//brak folderu output


    std::cout << std::endl << "test3" << std::endl;
    Datas datas3("config.ini", "testowy", "destination");//brak folderu input


    std::cout << std::endl << "test4" << std::endl;
    Datas datas4("CMakeLists.txt", "testowy", "testowy2");//inny plik wej


    return 0;
}
