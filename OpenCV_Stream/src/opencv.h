#ifndef OPENCV_H
#define OPENCV_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <mutex>
#include <condition_variable>

static std::mutex mu;
static std::condition_variable _cv_buffer;
static cv::Mat _frame;
static std::deque<cv::Mat> _buffer;

class OpenCV
{
    private:
        int _queue_length = 1000;

    public:        
        cv::VideoCapture startStream();
        void stopStream();
        void frameBuffer(cv::VideoCapture _cap);
        cv::Mat getFrameColor();
        cv::Mat getFrameGray();
        cv::Mat getFaceFrame(cv::Mat _frame, float circle_dia);

        void add(cv::Mat _frame)
        {
            std::unique_lock<std::mutex> buff_lock(mu);
            _cv_buffer.wait(buff_lock, [this]{ return !isFull(); });
            _buffer.push_front(_frame);
            buff_lock.unlock();
            _cv_buffer.notify_all();
        }

        cv::Mat get()
        {
            std::unique_lock<std::mutex> buff_lock(mu);
            _cv_buffer.wait(buff_lock, [this]{ return !isEmpty(); });
            cv::Mat request = _buffer.back();
            _buffer.pop_back();
            buff_lock.unlock();
            _cv_buffer.notify_all();
            return request;
        }

        bool isFull() const
        {
           return _buffer.size() >= static_cast<unsigned int>(_queue_length);
        }

        bool isEmpty() const
        {
            return _buffer.size() == static_cast<unsigned int>(0);
        }

};
#endif
