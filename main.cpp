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



    Datas datas("config.ini", "source", "destination");//idealny scenariusz
    
    Datas datas("config.ini", "source", "testowy2");//brak folderu output
    
    Datas datas3("config.ini", "testowy", "testowy2");//brak wej i wyj


    Datas datas4("CMakeLists.txt", "testowy", "testowy2");//inny plik wej


    return 0;
}
