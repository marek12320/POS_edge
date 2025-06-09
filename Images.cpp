#include "DatasImages.h"

void Images::processImage(const std::string& filePath, const std::string& filename, Datas& datas) {
    cv::Mat image = cv::imread(filePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Nie mozna zaladowac obrazu: " << filePath << std::endl;
        return;
    }

    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);

    {
        std::lock_guard<std::mutex> lock(imagesMutex);
        collectedImages.push_back(edges.clone());
    }

    {
        std::lock_guard<std::mutex> lock(datasMutex);
        datas.save2path(edges, filename);
    }
}

void Images::showImageGrid() {
    std::lock_guard<std::mutex> lock(imagesMutex);
    if (collectedImages.empty()) return;

    size_t total = collectedImages.size();
    int cols = static_cast<int>(std::ceil(std::sqrt(total)));
    int rows = static_cast<int>(std::ceil(static_cast<double>(total) / cols));

    int cellWidth = collectedImages[0].cols;
    int cellHeight = collectedImages[0].rows;

    cv::Mat grid = cv::Mat::zeros(rows * cellHeight, cols * cellWidth, collectedImages[0].type());

    for (size_t i = 0; i < total; ++i) {
        cv::Mat resized;
        cv::resize(collectedImages[i], resized, cv::Size(cellWidth, cellHeight));

        int r = static_cast<int>(i) / cols;
        int c = static_cast<int>(i) % cols;
        cv::Rect roi(c * cellWidth, r * cellHeight, cellWidth, cellHeight);

        resized.copyTo(grid(roi));
    }

    cv::namedWindow("Wszystkie obrazy", cv::WINDOW_NORMAL);
    cv::imshow("Wszystkie obrazy", grid);
    cv::waitKey(0);
    cv::destroyWindow("Wszystkie obrazy");
}
