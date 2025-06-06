#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "Datas.cpp"


int main() {

    Datas datas("config.ini", "source", "destination");

    std::string imagePath = datas.getReadPath();



    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Nie mozna zaladowac obrazu: " << imagePath << std::endl;
        return 1;
    }

    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);
    cv::imshow("Original", image);
    cv::imshow("Edges", edges);

    datas.save2path(edges);
    

    cv::waitKey(0);

    return 0;
}
