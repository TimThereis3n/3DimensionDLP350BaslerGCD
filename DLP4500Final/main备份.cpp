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
//#include<sstream>
//#include <iomanip>
//#include <atomic>
//#define SAVE_PATH "D:/measure/new/usingforsave"
//#pragma comment(lib,"hidapi.lib")
//
//
//static const int imagecount = 1;//起拍摄始照片序号
//static const double CameraExposure = 80000.0;//相机曝光
//static const size_t ImagesToGrab = 24;//单组拍摄图片数
//static const int max_group = 1;//拍摄组数
//static const Basler_UsbCameraParams::TriggerSourceEnums theChosenTriggerLine = Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line1;//触发源
//static const int NumOfFirstGroup = 1;//起拍组号
//std::atomic_bool gCamTriggerReady{ false };
//
////动态全局变量，不要改动。后续代码会在局部做修改
//int imageCount = 1;
//
//// 相机类
//// 文件名后缀imageCount在28行
//void runBasler(const std::string& folderPath) {
//    int grabbedlmages = 0;
//    //拍摄 拍两次8张一共16张
//    //同心圆 1 125000 8
//    //螺旋光 9 125000 8
//    //标定  17 250000 1
//    imageCount = imagecount;
//    double m_exposure = CameraExposure;
//    size_t m_countOfImagesToGrab = ImagesToGrab;
//    double m_gain = 0;//相机增益
//    Basler_UsbCameraParams::TriggerSourceEnums triggerSource = theChosenTriggerLine;
//
//    Pylon::PylonInitialize();
//    Pylon::CBaslerUsbInstantCamera* camera_t = NULL;
//    Pylon::CDeviceInfo* info = new Pylon::CDeviceInfo;
//
//    // 诊断：列出可用的 Transport Layers 和设备
//    /*TlInfoList_t tls;
//    CTlFactory::GetInstance().EnumerateTls(tls);
//    std::cout << "Available TLs: " << tls.size() << std::endl;
//    for (const auto& t : tls) {
//        std::cout << " - " << t.GetFriendlyName()
//            << " (" << t.GetFullName() << ")\n";
//    }*/
//
//    /*DeviceInfoList_t devices;
//    CTlFactory::GetInstance().EnumerateDevices(devices);
//    std::cout << "Found devices: " << devices.size() << std::endl;
//    for (const auto& d : devices) {
//        std::cout << " - " << d.GetFriendlyName() << " ["
//            << d.GetModelName() << "] via " << d.GetDeviceClass() << "\n";
//    }*/
//
//
//    try
//    {
//        if (camera_t == NULL)
//        {
//            info->SetDeviceClass(Pylon::CBaslerUsbInstantCamera::DeviceClass());
//            camera_t = new Pylon::CBaslerUsbInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice(*info));
//            camera_t->GrabCameraEvents = true;
//        }
//        camera_t->Open();
//        //std::cout << "Open Basler Camera successfully!" << std::endl;
//
//        // 等待1秒，确保相机完全初始化
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        GenApi::INodeMap& nodemap = camera_t->GetNodeMap();
//        const GenApi::CEnumerationPtr pixelFormat(nodemap.GetNode("PixelFormat"));
//        if (GenApi::IsAvailable(pixelFormat->GetEntryByName("Mono8")))
//        {
//            pixelFormat->FromString("Mono8");
//        }
//
//        if (GenApi::IsAvailable(camera_t->TriggerSource.GetEntry(theChosenTriggerLine))) {
//            camera_t->TriggerSource.SetValue(theChosenTriggerLine);
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
//        Pylon::CBaslerUsbInstantCamera& camera = *camera_t;
//
//        //std::cout << "Camera Exposure  : " << camera.ExposureTime.GetValue() <<" us" << std::endl;
//
//        // 设置增益
//        camera.GainAuto.SetValue(Basler_UsbCameraParams::GainAuto_Off);
//        camera.Gain.SetValue(m_gain);
//        //std::cout << "Gain  : " << camera.Gain.GetValue() << std::endl;
//
//        // 新建Pylon ImageFormatConverter对象
//        Pylon::CImageFormatConverter formatConverter;
//        Pylon::CPylonImage pylonImage;
//        cv::Mat openCvImage;
//
//        // 开始抓取
//        camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByUser);
//        // 抓取结果数据指针
//        Pylon::CGrabResultPtr ptrGrabResult;
//
//        // 确保相机已经准备好，拍摄延时
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//
//        // ===== 等待相机进入“可接收下一次外部触发”的状态 =====
//        gCamTriggerReady = false;
//        if (!camera.WaitForFrameTriggerReady(5000, Pylon::TimeoutHandling_Return)) {
//            std::cerr << "[Camera] Not trigger-ready within 5s.\n";
//            gCamTriggerReady = false;
//        }
//        else {
//            gCamTriggerReady = true;
//        }
//
//        size_t saved = 0;//已经存储的图片数量
//        const size_t need = ImagesToGrab;//需要拍摄的图片数量       
//
//        while (camera.IsGrabbing() && saved < need)
//        {
//            // 建议：不抛异常的等待，没触发就继续等
//            bool got = camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_Return);
//            if (!got) continue;
//
//            if (ptrGrabResult->GrabSucceeded())
//            {
//                // 将抓取的缓冲数据转化成Pylon image
//                formatConverter.Convert(pylonImage, ptrGrabResult);
//
//                // 将Pylon image转成OpenCV image
//                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(),
//                    CV_8UC1, (uint8_t*)pylonImage.GetBuffer());
//
//                // 文件名 = 起拍序号(3) + 已保存偏移（3,4,5,...,18）
//                int fname_index = imagecount + static_cast<int>(saved);
//
//                std::ostringstream s;
//                s << folderPath << "/" << std::setw(4) << std::setfill('0') << fname_index << ".bmp";
//                std::string imageName = s.str();
//
//                if (cv::imwrite(imageName, openCvImage)) {
//                    std::cout << "Image saved: " << imageName << std::endl;
//                }
//                else {
//                    std::cout << "Failed to save image: " << imageName << std::endl;
//                }
//
//                ++saved;
//
//                // 可选：每8张提示一次
//                if (saved % 8 == 0) {
//                    std::cout << "[Info] 本组已保存 " << saved << " / " << need << " 张\n";
//                }
//            }
//            else {
//                std::cerr << "Grab failed: " << ptrGrabResult->GetErrorDescription() << std::endl;
//            }
//        }
//
//        // 停止抓取
//        camera.StopGrabbing();
//        // 关闭相机
//        camera.Close();
//    }
//    catch (const Pylon::GenericException& e)
//    {
//        std::cerr << "An exception occurred: " << e.GetDescription() << std::endl;
//        std::cerr << "if Grab timed out Please choose your trigger line again" << std::endl;
//    }
//
//    // 释放Pylon库
//    Pylon::PylonTerminate();
//}
//
//
//// 拍照方法
//void takePictures(int group) {
//    std::string cur = SAVE_PATH + std::string("/") + std::to_string(group);
//    runBasler(cur);       // 将子文件夹路径传递给 runBasler
//}
//
//
//// 平移台控制
//class GCD040101M {
//private:
//    HANDLE hSerial = INVALID_HANDLE_VALUE;
//
//    bool SendRaw(const std::vector<uint8_t>& frame) {
//        DWORD written = 0;
//        bool ok = WriteFile(hSerial, frame.data(), 10, &written, NULL) && written == 10;
//        return ok;
//    }
//
//public:
//    bool Open(const std::string& port) {
//        hSerial = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
//        if (hSerial == INVALID_HANDLE_VALUE) return false;
//
//        DCB dcb = { 0 };
//        dcb.DCBlength = sizeof(dcb);
//        GetCommState(hSerial, &dcb);
//        dcb.BaudRate = CBR_9600;
//        dcb.ByteSize = 8;
//        dcb.Parity = NOPARITY;
//        dcb.StopBits = ONESTOPBIT;
//        SetCommState(hSerial, &dcb);
//
//        COMMTIMEOUTS to = { 50, 50, 10, 50, 10 };
//        SetCommTimeouts(hSerial, &to);
//
//        std::cout << "===== 串口已打开 =====" << std::endl;
//        return true;
//    }
//
//    void Close() {
//        if (hSerial != INVALID_HANDLE_VALUE) CloseHandle(hSerial);
//    }
//
//    bool Move(uint8_t deviceID, uint16_t speed, bool direction) {
//        std::vector<uint8_t> frames[4] = {
//            {0x00, 0x00, 0x40, deviceID, 'P', 0x00, 0x00, 0x14, 0x00, 0x00},
//            {0x00, 0x00, 0x40, deviceID, 'S', 0x00, 0x05, 0x00, 0x00, 0x00},
//            {0x00, 0x00, 0x40, deviceID, 'D', 0x00, static_cast<uint8_t>(direction ? 0x01 : 0x00), 0x00, 0x00, 0x00},
//            {0x00, 0x00, 0x40, deviceID, 'G', 0x00, 0x00, 0x00, 0x00, 0x00}
//        };
//        for (const auto& f : frames) {
//            if (!SendRaw(f)) return false;
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }
//        return true;
//    }
//};
//
//// 返回 true 表示已停止；false 表示超时还没停
//static bool WaitPatternStopped(int timeout_ms) {
//    unsigned int patMode = 0;
//    const int poll_ms = 10;
//    int waited = 0;
//    while (waited < timeout_ms) {
//        if (DLPC350_GetPatternDisplay(&patMode) == 0) {
//            if (patMode == 0) return true;  // 已停止
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(poll_ms));
//        waited += poll_ms;
//    }
//    return false;
//}
//
//void PatSeqCtrlStop() {
//    int rc = DLPC350_PatternDisplay(0);  // 发停止命令
//    if (rc != 0) {
//        std::cerr << "[DLP] DLPC350_PatternDisplay(0) failed, rc=" << rc << std::endl;
//    }
//
//    // 最多等 2000ms 进入停止态
//    if (!WaitPatternStopped(2000)) {
//        unsigned int patMode = 0;
//        DLPC350_GetPatternDisplay(&patMode);
//        std::cerr << "[Warn] Pattern display not stopped within timeout. patMode=" << patMode << std::endl;
//        // 如仍不稳，可在这里追加更强的兜底（可选）：
//        bool mode = false; DLPC350_SetMode(mode);
//        std::this_thread::sleep_for(std::chrono::milliseconds(50));
//        DLPC350_SetMode(true);
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
//void ChangeDLPExposure(int exp) {
//
//}
//
//// 主函数
//int main() {
//    // 初始化硬件
//    ControlDLP4500 dlp;
//    //等待投影仪完全初始化
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//
//    GCD040101M stage;
//    if (!stage.Open("COM5")) {
//        std::cerr << "无法打开串口 COM5，无法控制平移台移动\n";
//        //return 1;如果不注释就会直接中止程序
//    }
//
//
//    try {
//        Pylon::PylonInitialize();
//        Pylon::TlInfoList_t tls;
//        Pylon::CTlFactory::GetInstance().EnumerateTls(tls);
//
//        std::cout << "===== Pylon Transport Layers =====" << std::endl;
//        std::cout << "Available TLs: " << tls.size() << std::endl;
//        for (const auto& t : tls) {
//            std::cout << " - " << t.GetFriendlyName()
//                << " (" << t.GetFullName() << ")\n";
//        }
//
//        Pylon::PylonTerminate();
//    }
//    catch (const Pylon::GenericException& e) {
//        std::cerr << "[Pylon] Enum TL failed: " << e.GetDescription() << std::endl;
//    }
//
//    int group = 1;  // 照片序号从 1 开始
//
//    // 读取并打印曝光&周期（单位：微秒us）
//    std::cout << "===== Initial Settings =====" << std::endl;
//    std::cout << "Camera Exposure  : " << CameraExposure << " us" << std::endl;
//    unsigned int expo_us = 0, frame_us = 0;
//    if (DLPC350_GetExposure_FramePeriod(&expo_us, &frame_us) == 0) {
//        double expo_ms = expo_us / 1000.0;
//        double frame_ms = frame_us / 1000.0;
//        double fps = (frame_us > 0) ? (1e6 / frame_us) : 0.0;
//        std::cout << "Pattern Exposure : " << expo_us << " us" << std::endl;// (" << expo_ms << " ms), " << endl;
//        std::cout << "Pattern period   : " << frame_us << " us" << std::endl;// (" << frame_ms << " ms), " << endl;
//    }
//    else {
//        std::cerr << "[DLP] Read exposure/frame period failed\n";
//    }
//    std::cout << "============================" << std::endl;
//
//    // 无限循环：先投影→再拍照→平移台移动
//    while (group <= max_group) {
//        std::string folder = SAVE_PATH + std::string("/") + std::to_string(group);
//
//        int ret = _mkdir(folder.c_str());
//        if (ret != 0 && errno != EEXIST) {
//            std::cerr << "创建目录失败: " << folder << '\n';
//        }
//
//        std::cout << "===== 第 " << group << " 组 =====" << std::endl;
//
//        // 先起相机线程（里面 StartGrabbing 并 WaitForFrameTriggerReady）
//        gCamTriggerReady = false;
//        std::thread camThread([&]() {
//            takePictures(group);  // 内部会调用 runBasler(cur)
//            });
//
//        // 等相机武装到“可接收下一次触发”
//        for (int i = 0; i < 500; ++i) { // 最长约5秒；10ms * 500
//            if (gCamTriggerReady.load()) break;
//            std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        }
//        if (!gCamTriggerReady.load()) {
//            std::cerr << "[Warn] Camera not trigger-ready, still starting DLP...\n";
//        }
//
//        // 再启动投影（DLP 开始出 TRIG_OUT）
//        runDLP();
//
//        // 等相机线程把本组 16 张拍完
//        camThread.join();
//
//
//        // 停止投影
//        PatSeqCtrlStop();
//        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 确保投影停止
//
//        // 平移台移动 1 cm
//        if (group == max_group) {
//            group++;
//            std::cout << "=== " << max_group << " 组拍摄结束===" << std::endl;
//            break;
//        }
//        else {
//            stage.Move(0x02, 15, false);//false向左，true向右,从桌子往实验台看
//            std::this_thread::sleep_for(std::chrono::milliseconds(3500));//3500ms = 3.5s
//        }
//
//        group++;
//
//    }
//    system("pause");
//    return 0;
//}