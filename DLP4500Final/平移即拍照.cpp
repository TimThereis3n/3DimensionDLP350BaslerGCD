#include <iostream>
#include <conio.h>          // _kbhit(), _getch()
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <direct.h>
#include <Windows.h>
#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "dlp4500.h"
#include <errno.h>       // for errno
#define SAVE_PATH "D:/measure/some/usingforsave"
#pragma comment(lib,"hidapi.lib")

using namespace std;
using namespace GenApi;
using namespace Pylon;


int imageCount = 1;

// 相机类
// 曝光在32行
// 文件名后缀imageCount在29行
void runBasler(const std::string& folderPath) {
    imageCount = 1;
    size_t m_countOfImagesToGrab = 8;
    int grabbedlmages = 0;
    double m_exposure = 35000;
    double m_gain = 1;
    Basler_UsbCameraParams::TriggerSourceEnums triggerSource = Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3;

    PylonInitialize();
    Pylon::CBaslerUsbInstantCamera* camera_t = NULL;
    Pylon::CDeviceInfo* info = new CDeviceInfo;
    try
    {
        if (camera_t == NULL)
        {
            info->SetDeviceClass(CBaslerUsbInstantCamera::DeviceClass());
            camera_t = new CBaslerUsbInstantCamera(CTlFactory::GetInstance().CreateFirstDevice(*info));
            camera_t->GrabCameraEvents = true;
        }
        camera_t->Open();
        std::cout << "Open Basler Camera successfully!" << std::endl;

        // 等待1秒，确保相机完全初始化
        std::this_thread::sleep_for(std::chrono::seconds(1));

        INodeMap& nodemap = camera_t->GetNodeMap();
        const GenApi::CEnumerationPtr pixelFormat(nodemap.GetNode("PixelFormat"));
        if (IsAvailable(pixelFormat->GetEntryByName("Mono8")))
        {
            pixelFormat->FromString("Mono8");
        }

        if (IsAvailable(camera_t->TriggerSource.GetEntry(Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3))) {
            camera_t->TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3);
        }
        else {
            std::cerr << "错误：请求的触发源不可用;可以尝试设置其他触发源或退出;" << std::endl;
        }

        camera_t->MaxNumBuffer = 10;

        // 设置图像大小
        GenApi::CIntegerPtr width = nodemap.GetNode("Width");
        GenApi::CIntegerPtr height = nodemap.GetNode("Height");
        GenApi::CIntegerPtr offsetX = nodemap.GetNode("OffsetX");
        GenApi::CIntegerPtr offsetY = nodemap.GetNode("OffsetY");
        offsetX->SetValue(offsetX->GetMin());
        offsetY->SetValue(offsetY->GetMin());
        width->SetValue(width->GetMax());
        height->SetValue(height->GetMax());
        camera_t->TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);
        camera_t->TriggerActivation.SetValue(Basler_UsbCameraParams::TriggerActivation_RisingEdge);
        camera_t->TriggerSource.SetValue(triggerSource);

        // 设置触发延时
        camera_t->TriggerDelay.SetValue(0.0);
        camera_t->LineDebouncerTime.SetValue(10.0);
        camera_t->TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);

        camera_t->ExposureTime.SetValue(m_exposure);

        CBaslerUsbInstantCamera& camera = *camera_t;

        std::cout << "ExposureTime  : " << camera.ExposureTime.GetValue() << std::endl;

        // 设置增益
        camera.GainAuto.SetValue(Basler_UsbCameraParams::GainAuto_Off);
        camera.Gain.SetValue(m_gain);
        std::cout << "Gain  : " << camera.Gain.GetValue() << std::endl;

        // 新建Pylon ImageFormatConverter对象
        CImageFormatConverter formatConverter;
        CPylonImage pylonImage;
        cv::Mat openCvImage;

        // 开始抓取
        camera.StartGrabbing(m_countOfImagesToGrab, GrabStrategy_OneByOne, GrabLoop_ProvidedByUser);
        // 抓取结果数据指针
        CGrabResultPtr ptrGrabResult;

        // 确保相机已经准备好，拍摄延时
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        while (camera.IsGrabbing())
        {
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

            // 每8张拍摄完成一组
            if (imageCount < 8) {
                if (imageCount > 8) {
                    std::cout << "本组拍摄完毕" << std::endl;
                    break;
                }
            }
            else {
                if (imageCount > 16) {
                    std::cout << "本组拍摄完毕" << std::endl;
                    break;
                }
            }

            if (ptrGrabResult->GrabSucceeded())
            {
                // 将抓取的缓冲数据转化成Pylon image
                formatConverter.Convert(pylonImage, ptrGrabResult);

                // 将Pylon image转成OpenCV image
                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t*)pylonImage.GetBuffer());

                std::ostringstream s;
                // 文件名生成，确保保存到指定子文件夹中
                s << folderPath << "/" << imageCount << ".bmp";
                std::string imageName(s.str());

                // 保存图像到指定路径
                if (cv::imwrite(imageName, openCvImage)) {
                    std::cout << "Image saved: " << imageName << std::endl;
                }
                else {
                    std::cout << "Failed to save image: " << imageName << std::endl;
                }

                imageCount++;
            }
            else {
                std::cout << "Grab fail !" << std::endl;
            }
        }
        // 停止抓取
        camera.StopGrabbing();
        // 关闭相机
        camera.Close();
    }
    catch (const GenericException& e)
    {
        std::cerr << "An exception occurred: " << e.GetDescription() << std::endl;
        std::cerr << "if Grab timed out : Please choose your trigger line agian" << endl;
    }

    // 释放Pylon库
    PylonTerminate();
}


// 拍照方法
void takePictures(int group) {
    std::string cur = SAVE_PATH + std::string("/") + std::to_string(group);
    _mkdir(cur.c_str());  // 创建子目录
    runBasler(cur);       // 将子文件夹路径传递给 runBasler
    imageCount = 1;       // 复位下组计数
}


// 平移台控制
class GCD040101M {
private:
    HANDLE hSerial = INVALID_HANDLE_VALUE;

    bool SendRaw(const std::vector<uint8_t>& frame) {
        DWORD written = 0;
        bool ok = WriteFile(hSerial, frame.data(), 10, &written, NULL) && written == 10;
        return ok;
    }

public:
    bool Open(const std::string& port) {
        hSerial = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hSerial == INVALID_HANDLE_VALUE) return false;

        DCB dcb = { 0 };
        dcb.DCBlength = sizeof(dcb);
        GetCommState(hSerial, &dcb);
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        SetCommState(hSerial, &dcb);

        COMMTIMEOUTS to = { 50, 50, 10, 50, 10 };
        SetCommTimeouts(hSerial, &to);

        std::cout << "串口已打开" << std::endl;
        return true;
    }

    void Close() {
        if (hSerial != INVALID_HANDLE_VALUE) CloseHandle(hSerial);
    }

    bool Move(uint8_t deviceID, uint16_t speed, bool direction) {
        std::vector<uint8_t> frames[4] = {
            {0x00, 0x00, 0x40, deviceID, 'P', 0x00, 0x00, 0x10, 0x00, 0x00},
            {0x00, 0x00, 0x40, deviceID, 'S', 0x00, 0x05, 0x00, 0x00, 0x00},
            {0x00, 0x00, 0x40, deviceID, 'D', 0x00, static_cast<uint8_t>(direction ? 0x01 : 0x00), 0x00, 0x00, 0x00},
            {0x00, 0x00, 0x40, deviceID, 'G', 0x00, 0x00, 0x00, 0x00, 0x00}
        };
        for (const auto& f : frames) {
            if (!SendRaw(f)) return false;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return true;
    }
};

// 主函数
int main() {
    // 初始化硬件
    GCD040101M stage;
    if (!stage.Open("COM5")) {
        std::cerr << "无法打开串口 COM5\n";
        return 1;
    }

    int group = 0;  // 组号从 1 开始

    // 无限循环：先拍照→平移台移动
    do {
        ++group;
        std::string folder = SAVE_PATH + std::string("/") + std::to_string(group);

        int ret = _mkdir(folder.c_str());
        if (ret != 0 && errno != EEXIST) {
            std::cerr << "创建目录失败: " << folder << '\n';
        }

        _mkdir(folder.c_str());
        std::cout << "===== 第 " << group << " 组 =====" << std::endl;

        // 拍照
        takePictures(group);

        // 平移台移动 2 mm
        stage.Move(0x02, 15, false);//false向左，true向右,从桌子往实验台看
        std::this_thread::sleep_for(std::chrono::seconds(2));

    } while (true);

    return 0;
}
