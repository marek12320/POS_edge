/**
 * @file main.cpp
 * @brief G³ówny plik wykonywalny programu do przetwarzania obrazów (Canny Edge Detection).
 */

#include "Classys.cpp"

/**
 * @brief G³ówna funkcja programu.
 * 
 * Inicjalizuje obiekty, przetwarza pliki .jpg i .png z katalogu wejœciowego i zapisuje je w katalogu wyjœciowym.
 */
int main() {



    Datas datas("config.ini", "source", "destination");
    
    
    Datas datas2("config.ini", "testowy", "testowy2");


    Datas datas3("CMakeLists.txt", "testowy", "testowy2");


    return 0;
}
