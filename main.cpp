/**
 * @file main.cpp
 * @brief G³ówny plik wykonywalny programu do przetwarzania obrazów (Canny Edge Detection).
 */

#include "DatasImages.h"

/**
 * @brief G³ówna funkcja programu.
 * 
 * Inicjalizuje obiekty, przetwarza pliki .jpg i .png z katalogu wejœciowego i zapisuje je w katalogu wyjœciowym.
 */
int main() {



    Datas datas("config.ini", "source", "destination");//idealny scenariusz
    
    Datas datas("config.ini", "source", "testowy2");//brak folderu output
    
    Datas datas3("config.ini", "testowy", "testowy2");//brak wej i wyj


    Datas datas4("CMakeLists.txt", "testowy", "testowy2");//inny plik wej


    return 0;
}
