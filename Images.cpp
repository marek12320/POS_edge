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
    // Blokujemy dostêp do wewnêtrznych kolekcji obrazów, aby zapewniæ bezpieczeñstwo w¹tków
    std::lock_guard<std::mutex> lock(imagesMutex);

    // Jeœli brak obrazów, koñczymy funkcjê
    if (collectedImages.empty() || originalImages.empty()) return;

    // Pomocnicza funkcja lambda tworz¹ca siatkê (mozaikê) z wektora obrazów
    auto createGrid = [](const std::vector<cv::Mat>& images) -> cv::Mat {
        size_t total = images.size();  // Liczba obrazów
        int cols = static_cast<int>(std::ceil(std::sqrt(total)));  // Liczba kolumn
        int rows = static_cast<int>(std::ceil(static_cast<double>(total) / cols));  // Liczba wierszy

        int cellWidth = images[0].cols;   // Szerokoœæ komórki (obrazka)
        int cellHeight = images[0].rows;  // Wysokoœæ komórki

        // Tworzymy pusty obraz dla mozaiki, z odpowiednim rozmiarem i typem
        cv::Mat grid = cv::Mat::zeros(rows * cellHeight, cols * cellWidth, images[0].type());

        // Iterujemy po wszystkich obrazach i wstawiamy je do siatki
        for (size_t i = 0; i < total; ++i) {
            cv::Mat resized;
            // Dopasowujemy ka¿dy obraz do rozmiaru komórki
            cv::resize(images[i], resized, cv::Size(cellWidth, cellHeight));

            // Obliczamy pozycjê w siatce (rz¹d i kolumnê)
            int r = static_cast<int>(i) / cols;
            int c = static_cast<int>(i) % cols;
            cv::Rect roi(c * cellWidth, r * cellHeight, cellWidth, cellHeight);

            // Kopiujemy obraz do odpowiedniego miejsca w mozaice
            resized.copyTo(grid(roi));
        }

        return grid;  // Zwracamy gotow¹ mozaikê
        };

    // Deklaracja obrazów mozaikowych
    cv::Mat edgeGrid, originalGrid;

    // Tworzenie mozaiki z przetworzonych obrazów (krawêdzi) w osobnym w¹tku
    std::thread t1([&]() {
        edgeGrid = createGrid(collectedImages);  // Tworzymy mozaikê
        datas.save2path(edgeGrid, "mozaika_krawedzi.png");  // Zapisujemy do pliku
        });

    // Tworzenie mozaiki z oryginalnych obrazów w osobnym w¹tku
    std::thread t2([&]() {
        originalGrid = createGrid(originalImages);  // Tworzymy mozaikê
        datas.save2path(originalGrid, "mozaika_oryginalow.png");  // Zapisujemy do pliku
        });

    // Czekamy a¿ oba w¹tki zakoñcz¹ swoje zadania
    t1.join();
    t2.join();

    // Wyœwietlanie obu mozaik w oknach OpenCV
    try {
        cv::namedWindow("Mozaika - Krawêdzie", cv::WINDOW_NORMAL);
        cv::imshow("Mozaika - Krawêdzie", edgeGrid);

        cv::namedWindow("Mozaika - Orygina³y", cv::WINDOW_NORMAL);
        cv::imshow("Mozaika - Orygina³y", originalGrid);

        cv::waitKey(0);  // Czekamy na naciœniêcie klawisza

        // Zamykamy okna
        cv::destroyWindow("Mozaika - Krawêdzie");
        cv::destroyWindow("Mozaika - Orygina³y");
    }
    catch (const cv::Exception& e) {
        std::cerr << "B³¹d podczas wyœwietlania lub zamykania okien: " << e.what() << std::endl;
    }
}
