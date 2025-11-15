//#include <iostream>
//#include <conio.h>          // _kbhit(), _getch()
//#include <thread>
//#include <chrono>
//#include <vector>
//#include <string>
//#include <direct.h>
//#include <Windows.h>
//#include <pylon/PylonIncludes.h>
//#include <pylon/usb/BaslerUsbInstantCamera.h>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/core/core.hpp>
//#include "dlp4500.h"
//#include <errno.h>       // for errno
//#define SAVE_PATH "D:/measure/some/usingforsave"
//#pragma comment(lib,"hidapi.lib")
//
//using namespace std;
//using namespace GenApi;
//using namespace Pylon;
//
//
//int imageCount = 1;
//
//// 相机类
//// 曝光在32行
//// 文件名后缀imageCount在29行
//void runBasler(const std::string& folderPath) {
//    imageCount = 1;
//    size_t m_countOfImagesToGrab = 8;
//    int grabbedlmages = 0;
//    double m_exposure = 35000;
//    double m_gain = 1;
//    Basler_UsbCameraParams::TriggerSourceEnums triggerSource = Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line1;
//
//    PylonInitialize();
//    Pylon::CBaslerUsbInstantCamera* camera_t = NULL;
//    Pylon::CDeviceInfo* info = new CDeviceInfo;
//    try
//    {
//        if (camera_t == NULL)
//        {
//            info->SetDeviceClass(CBaslerUsbInstantCamera::DeviceClass());
//            camera_t = new CBaslerUsbInstantCamera(CTlFactory::GetInstance().CreateFirstDevice(*info));
//            camera_t->GrabCameraEvents = true;
//        }
//        camera_t->Open();
//        std::cout << "Open Basler Camera successfully!" << std::endl;
//
//        // 等待1秒，确保相机完全初始化
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        INodeMap& nodemap = camera_t->GetNodeMap();
//        const GenApi::CEnumerationPtr pixelFormat(nodemap.GetNode("PixelFormat"));
//        if (IsAvailable(pixelFormat->GetEntryByName("Mono8")))
//        {
//            pixelFormat->FromString("Mono8");
//        }
//
//        if (IsAvailable(camera_t->TriggerSource.GetEntry(Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3))) {
//            camera_t->TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3);
//        }
//        else {
//            std::cerr << "错误：请求的触发源不可用;可以尝试设置其他触发源或退出;" << std::endl;
//        }
//
//        camera_t->MaxNumBuffer = 10;
//
//        // 设置图像大小
//        GenApi::CIntegerPtr width = nodemap.GetNode("Width");
//        GenApi::CIntegerPtr height = nodemap.GetNode("Height");
//        GenApi::CIntegerPtr offsetX = nodemap.GetNode("OffsetX");
//        GenApi::CIntegerPtr offsetY = nodemap.GetNode("OffsetY");
//        offsetX->SetValue(offsetX->GetMin());
//        offsetY->SetValue(offsetY->GetMin());
//        width->SetValue(width->GetMax());
//        height->SetValue(height->GetMax());
//        camera_t->TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);
//        camera_t->TriggerActivation.SetValue(Basler_UsbCameraParams::TriggerActivation_RisingEdge);
//        camera_t->TriggerSource.SetValue(triggerSource);
//
//        // 设置触发延时
//        camera_t->TriggerDelay.SetValue(0.0);
//        camera_t->LineDebouncerTime.SetValue(10.0);
//        camera_t->TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);
//
//        camera_t->ExposureTime.SetValue(m_exposure);
//
//        CBaslerUsbInstantCamera& camera = *camera_t;
//
//        std::cout << "ExposureTime  : " << camera.ExposureTime.GetValue() << std::endl;
//
//        // 设置增益
//        camera.GainAuto.SetValue(Basler_UsbCameraParams::GainAuto_Off);
//        camera.Gain.SetValue(m_gain);
//        std::cout << "Gain  : " << camera.Gain.GetValue() << std::endl;
//
//        // 新建Pylon ImageFormatConverter对象
//        CImageFormatConverter formatConverter;
//        CPylonImage pylonImage;
//        cv::Mat openCvImage;
//
//        // 开始抓取
//        camera.StartGrabbing(m_countOfImagesToGrab, GrabStrategy_OneByOne, GrabLoop_ProvidedByUser);
//        // 抓取结果数据指针
//        CGrabResultPtr ptrGrabResult;
//
//        // 确保相机已经准备好，拍摄延时
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//
//        while (camera.IsGrabbing())
//        {
//            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
//
//            // 每8张拍摄完成一组
//            if (imageCount < 8) {
//                if (imageCount > 8) {
//                    std::cout << "本组拍摄完毕" << std::endl;
//                    break;
//                }
//            }
//            else {
//                if (imageCount > 16) {
//                    std::cout << "本组拍摄完毕" << std::endl;
//                    break;
//                }
//            }
//
//            if (ptrGrabResult->GrabSucceeded())
//            {
//                // 将抓取的缓冲数据转化成Pylon image
//                formatConverter.Convert(pylonImage, ptrGrabResult);
//
//                // 将Pylon image转成OpenCV image
//                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t*)pylonImage.GetBuffer());
//
//                std::ostringstream s;
//                // 文件名生成，确保保存到指定子文件夹中
//                s << folderPath << "/" << imageCount << ".bmp";
//                std::string imageName(s.str());
//
//                // 保存图像到指定路径
//                if (cv::imwrite(imageName, openCvImage)) {
//                    std::cout << "Image saved: " << imageName << std::endl;
//                }
//                else {
//                    std::cout << "Failed to save image: " << imageName << std::endl;
//                }
//
//                imageCount++;
//            }
//            else {
//                std::cout << "Grab fail !" << std::endl;
//            }
//        }
//        // 停止抓取
//        camera.StopGrabbing();
//        // 关闭相机
//        camera.Close();
//    }
//    catch (const GenericException& e)
//    {
//        std::cerr << "An exception occurred: " << e.GetDescription() << std::endl;
//    }
//
//    // 释放Pylon库
//    PylonTerminate();
//}
//
//
//// 拍照方法
//void takePictures(int group) {
//    std::string cur = SAVE_PATH + std::string("/") + std::to_string(group);
//    _mkdir(cur.c_str());  // 创建子目录
//    runBasler(cur);       // 将子文件夹路径传递给 runBasler
//    imageCount = 1;       // 复位下组计数
//}
//
//// DLP 投影控制
//void PatSeqCtrlStop() {
//    int index = 0;
//    unsigned int patMode;
//    DLPC350_PatternDisplay(0);
//    while (1) {
//        DLPC350_GetPatternDisplay(&patMode);
//        if (patMode == 0) {
//            break;
//        }
//        else {
//            DLPC350_PatternDisplay(0);
//        }
//        if (index++ > 10) {
//            std::cout << "pattern display mode set fail" << std::endl;
//            break;
//        }
//    }
//}
//
//void runDLP() {
//    DLPC350_PatternDisplay(2);
//}
//
//void SetDLPC350CInPatternMode() {
//    int index = 0;
//    bool mode;
//    unsigned int patMode;
//    DLPC350_GetMode(&mode);
//    if (mode == false) {
//        DLPC350_SetMode(true);
//        std::this_thread::sleep_for(std::chrono::microseconds(100));
//        while (1) {
//            DLPC350_GetMode(&mode);
//            if (mode) {
//                break;
//            }
//            std::this_thread::sleep_for(std::chrono::microseconds(100));
//            if (index++ > 10) {
//                break;
//            }
//        }
//    }
//    else {
//        DLPC350_GetPatternDisplay(&patMode);
//        if (patMode != 0) {
//            PatSeqCtrlStop();
//        }
//    }
//}
//
//
//// 主函数
//int main() {
//    // 初始化硬件
//    ControlDLP4500 dlp;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//
//    int group = 0;  // 组号从 1 开始
//
//    // 无限循环：先投影→再拍照→平移台移动
//    do {
//        ++group;
//        std::string folder = SAVE_PATH + std::string("/") + std::to_string(group);
//
//        int ret = _mkdir(folder.c_str());
//        if (ret != 0 && errno != EEXIST) {
//            std::cerr << "创建目录失败: " << folder << '\n';
//        }
//
//        _mkdir(folder.c_str());
//        std::cout << "===== 第 " << group << " 组 =====" << std::endl;
//
//        // 启动投影
//        runDLP();
//        std::cout << "run Dlp" << endl;
//        std::this_thread::sleep_for(std::chrono::seconds(1)); // 确保投影稳定
//
//        // 拍照
//        takePictures(group);
//
//        // 停止投影
//        PatSeqCtrlStop();
//        std::cout << "stop DLP" << endl;
//        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 确保投影停止
//
//    } while (true);
//
//    return 0;
//}
