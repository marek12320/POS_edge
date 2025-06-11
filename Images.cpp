#include "DatasImages.h"

void Images::processImage(const std::string& filePath, const std::string& filename, Datas& datas) {
    // Wczytanie obrazu z dysku w kolorze
    cv::Mat image = cv::imread(filePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        // Obsluga bledu wczytania obrazu
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Nie mozna zaladowac obrazu: " << filePath << std::endl;
        return;
    }

    // Przetwarzanie obrazu - detekcja krawedzi
    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);

    {
        // Zapis do wewnetrznych kolekcji obrazow z uzyciem blokady
        std::lock_guard<std::mutex> lock(imagesMutex);
        collectedImages.push_back(edges.clone());     // Przetworzony obraz
        originalImages.push_back(image.clone());      // Oryginalny obraz
    }

    {
        // Zapis przetworzonego obrazu do katalogu wynikowego z uzyciem blokady
        std::lock_guard<std::mutex> lock(datasMutex);
        datas.save2path(edges, filename);
    }
}

void Images::showImageGrid(Datas& datas) {
    // Blokujemy dost�p do wewn�trznych kolekcji obraz�w, aby zapewni� bezpiecze�stwo w�tk�w
    std::lock_guard<std::mutex> lock(imagesMutex);

    // Je�li brak obraz�w, ko�czymy funkcj�
    if (collectedImages.empty() || originalImages.empty()) return;

    // Pomocnicza funkcja lambda tworz�ca siatk� (mozaik�) z wektora obraz�w
    auto createGrid = [](const std::vector<cv::Mat>& images) -> cv::Mat {
        size_t total = images.size();  // Liczba obraz�w
        int cols = static_cast<int>(std::ceil(std::sqrt(total)));  // Liczba kolumn
        int rows = static_cast<int>(std::ceil(static_cast<double>(total) / cols));  // Liczba wierszy

        int cellWidth = images[0].cols;   // Szeroko�� kom�rki (obrazka)
        int cellHeight = images[0].rows;  // Wysoko�� kom�rki

        // Tworzymy pusty obraz dla mozaiki, z odpowiednim rozmiarem i typem
        cv::Mat grid = cv::Mat::zeros(rows * cellHeight, cols * cellWidth, images[0].type());

        // Iterujemy po wszystkich obrazach i wstawiamy je do siatki
        for (size_t i = 0; i < total; ++i) {
            cv::Mat resized;
            // Dopasowujemy ka�dy obraz do rozmiaru kom�rki
            cv::resize(images[i], resized, cv::Size(cellWidth, cellHeight));

            // Obliczamy pozycj� w siatce (rz�d i kolumn�)
            int r = static_cast<int>(i) / cols;
            int c = static_cast<int>(i) % cols;
            cv::Rect roi(c * cellWidth, r * cellHeight, cellWidth, cellHeight);

            // Kopiujemy obraz do odpowiedniego miejsca w mozaice
            resized.copyTo(grid(roi));
        }

        return grid;  // Zwracamy gotow� mozaik�
        };

    // Deklaracja obraz�w mozaikowych
    cv::Mat edgeGrid, originalGrid;

    // Tworzenie mozaiki z przetworzonych obraz�w (kraw�dzi) w osobnym w�tku
    std::thread t1([&]() {
        edgeGrid = createGrid(collectedImages);  // Tworzymy mozaik�
        datas.save2path(edgeGrid, "mozaika_krawedzi.png");  // Zapisujemy do pliku
        });

    // Tworzenie mozaiki z oryginalnych obraz�w w osobnym w�tku
    std::thread t2([&]() {
        originalGrid = createGrid(originalImages);  // Tworzymy mozaik�
        datas.save2path(originalGrid, "mozaika_oryginalow.png");  // Zapisujemy do pliku
        });

    // Czekamy a� oba w�tki zako�cz� swoje zadania
    t1.join();
    t2.join();

    // Wy�wietlanie obu mozaik w oknach OpenCV
    try {
        cv::namedWindow("Mozaika - Kraw�dzie", cv::WINDOW_NORMAL);
        cv::imshow("Mozaika - Kraw�dzie", edgeGrid);

        cv::namedWindow("Mozaika - Orygina�y", cv::WINDOW_NORMAL);
        cv::imshow("Mozaika - Orygina�y", originalGrid);

        cv::waitKey(0);  // Czekamy na naci�ni�cie klawisza

        // Zamykamy okna
        cv::destroyWindow("Mozaika - Kraw�dzie");
        cv::destroyWindow("Mozaika - Orygina�y");
    }
    catch (const cv::Exception& e) {
        std::cerr << "B��d podczas wy�wietlania lub zamykania okien: " << e.what() << std::endl;
    }
}
