#pragma once
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

#include "caffe/caffe.hpp"
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace caffe;
using namespace cv;
class Classifier {
public:
	Classifier();
	~Classifier();
	void init(const string& model_file,	const string& trained_file);
	void Predict(const cv::Mat& img, cv::Mat label_colours, cv::Mat& output_image);

private:
	void SetMean(const string& mean_file);

	void WrapInputLayer(std::vector<cv::Mat>* input_channels);

	void Preprocess(const cv::Mat& img,
		std::vector<cv::Mat>* input_channels);

	void Visualization(cv::Mat prediction_map, cv::Mat label_colours, cv::Mat& output_image);

private:
	boost::shared_ptr<Net<float> > net_;
	cv::Size input_geometry_;
	int num_channels_;

};


