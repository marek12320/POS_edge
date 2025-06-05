#include <opencv2/opencv.hpp>
#include <iostream>

int main() {

std :: string imagePath = "photos\cat.jpg";

// std :: string imagePath = "milkyway.jpg";
cv :: Mat image = cv :: imread(imagePath, cv :: IMREAD_COLOR);
cv::Mat edges;
cv::Canny(image, edges, 50, 150); // You can tweak these thresholds
cv::imshow("Original", image);
cv::imshow("Edges", edges);
cv::waitKey(0); // Wait for a key press

return 0;
}