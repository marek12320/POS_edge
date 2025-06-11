#include "DatasImages.h"

Datas::Datas(std::string FileINI, std::string Input, std::string Output) {
    // Zapisanie sciezki do pliku INI
    fileINI = FileINI;

    // Pobranie wartosci dla folderu wejsciowego i wyjsciowego z pliku INI
    inputDir = getValue(Input);
    outputDir = getValue(Output);

    char odp;

    // Sprawdzenie, czy obie sciezki sa poprawne
    if (outputDir == "" || inputDir == "") {
        std::cout << "nie podano sciezki wyjsciowej lub wejsciowej";
        return;
    }

    // Sprawdzenie, czy folder wyjsciowy istnieje
    DWORD ftyp = GetFileAttributesA(outputDir.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
        // Jesli folder nie istnieje, zapytaj uzytkownika o utworzenie
        std::cout << "Folder wyjsciowy [" << outputDir << "] nie istnieje. " << std::endl << "Czy chcesz go utworzyc: t? ";

        std::cin >> odp;

        if (odp == 't' || odp == 'T') {
            // Proba utworzenia folderu
            if (!CreateDirectoryA(outputDir.c_str(), NULL)) {
                std::cerr << "Nie udalo sie utworzyc folderu: " << outputDir << std::endl;
                return;
            }
            else {
                std::cout << "Utworzono folder: " << outputDir << std::endl;
            }
        }
        else {
            std::cout << "Pomijanie operacji - folder wyjsciowy nie istnieje." << std::endl;
            return;
        }
    }

    // Tworzenie instancji obiektu Images i listy watkow
    Images images;
    std::vector<std::thread> threads;
    int count = 0;

    // Przetwarzanie plikow z rozszerzeniem .jpg i .png
    processFilesWithExtension("jpg", images, threads, count);
    processFilesWithExtension("png", images, threads, count);

    // Oczekiwanie na zakonczenie wszystkich watkow
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
            count++;
        }
    }

    // Informacja o wyniku przetwarzania
    if (count == 0) {
        std::cout << "bledna sciezka dostepu lub brak rozszerzen jpg/png [" << inputDir << "]" << std::endl << "dla pliku " << FileINI << std::endl;
    }
    else {
        std::cout << "przetworzono " << count << " obrazow dla pliku " << FileINI << std::endl;
        std::cout << "czy stworzyc mozaiki, moze to potrwac troche dluzej: t? ";
        std::cin >> odp;

        if (odp == 't' || odp == 'T') {
            // Wyswietlenie mozaiki z obrazow
            images.showImageGrid(std::ref(*this));
        }

    }
}

void Datas::save2path(const cv::Mat& obraz, const std::string& name) {
    // Zbudowanie pelnej sciezki zapisu
    std::string savePath = outputDir + "\\" + name;
    // Zapis obrazu do pliku
    cv::imwrite(savePath, obraz);
}

std::string Datas::getValue(const std::string& key) {
    // Otworzenie pliku INI
    std::ifstream file(fileINI);
    std::string line;

    // Przegladanie pliku linia po linii
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string foundKey = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            // Sprawdzenie, czy znaleziono zadany klucz
            if (foundKey == key) {
                return value;
            }
        }
    }

    // Zwraca pusty string, jesli nie znaleziono
    return "";
}

void Datas::processFilesWithExtension(const std::string& extension, Images& images, std::vector<std::thread>& threads, int& count) {
    // Jesli folder wejsciowy pusty, nic nie rob
    if (inputDir.empty()) return;

    // Tworzenie wzorca do wyszukiwania plikow z podanym rozszerzeniem
    std::string searchPath = inputDir + "\\*." + extension;
    WIN32_FIND_DATAA fileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fileData);

    // Ustalenie maksymalnej liczby watkow na podstawie CPU
    const unsigned int MAX_THREADS = std::thread::hardware_concurrency();

    // Jesli nie znaleziono zadnych plikow
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        // Pomijanie folderow
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        // Pobranie nazwy i pelnej sciezki do pliku
        std::string filename = fileData.cFileName;
        std::string filePath = inputDir + "\\" + filename;

        // Ograniczenie liczby aktywnych watkow
        while (threads.size() >= MAX_THREADS) {
            for (auto it = threads.begin(); it != threads.end();) {
                if (it->joinable()) {
                    it->join();
                    it = threads.erase(it);
                    count++;
                }
                else {
                    ++it;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Dodanie nowego watku do przetwarzania obrazu
        threads.emplace_back(&Images::processImage, &images, filePath, filename, std::ref(*this));

    } while (FindNextFileA(hFind, &fileData) != 0);

    // Zwolnienie zasobow systemowych
    FindClose(hFind);
}
