/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace
{
void split_image(cv::Mat image, double stddev)
{
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev)
    {
        image.setTo(mean);
        return;
    }

    const auto width = image.cols;
    const auto height = image.rows;

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
}
void merge_image(cv::Mat image, double stddev)
{
    const auto width = image.cols;
    const auto height = image.rows;

    if (width < 2 || height < 2)
        return;

    cv::Mat q1 = image(cv::Range(0, height / 2), cv::Range(0, width / 2));
    cv::Mat q2 = image(cv::Range(0, height / 2), cv::Range(width / 2, width));
    cv::Mat q3 = image(cv::Range(height / 2, height), cv::Range(width / 2, width));
    cv::Mat q4 = image(cv::Range(height / 2, height), cv::Range(0, width / 2));

    merge_image(q1, stddev);
    merge_image(q2, stddev);
    merge_image(q3, stddev);
    merge_image(q4, stddev);

    cv::Scalar m1 = cv::mean(q1);
    cv::Scalar m2 = cv::mean(q2);
    cv::Scalar m3 = cv::mean(q3);
    cv::Scalar m4 = cv::mean(q4);

    double avg = (m1[0] + m2[0] + m3[0] + m4[0]) / 4.0;
    if (std::abs(m1[0] - avg) <= stddev &&
        std::abs(m2[0] - avg) <= stddev &&
        std::abs(m3[0] - avg) <= stddev &&
        std::abs(m4[0] - avg) <= stddev)
    {
        image.setTo(avg);
    }
}
} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    // split part
    cv::Mat res = image;
    split_image(res, stddev);

    // merge part
    merge_image(res, stddev);
    return res;
}
} // namespace cvlib
