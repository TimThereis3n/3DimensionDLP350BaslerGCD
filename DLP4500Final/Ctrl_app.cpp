//#include <iostream>
//#include <conio.h>          // _kbhit(), _getch()
//#include <thread>
//#include <chrono>
//#include <vector>
//#include <cstdint>  // std::int16_t
//#include <cstddef>  // std::size_t
//#include <cmath>    // std::double_t, std::lround
//#include <io.h>  
//#include <string>
//#include <direct.h>
//#include <Windows.h>
//#include <pylon/PylonIncludes.h>
//#include <pylon/usb/BaslerUsbInstantCamera.h>
//#include <opencv2/opencv.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>
//#include "dlp4500.h"
//#include <errno.h>       // for errno
//#define SAVE_PATH "D:/usingforsave"
//#pragma comment(lib,"hidapi.lib")
//
//using namespace std;
//using namespace GenApi;
//using namespace Pylon;
//
//// ------- 触发源：全局变量与解析函数 -------
//std::string g_savePath = SAVE_PATH;
//Basler_UsbCameraParams::TriggerSourceEnums g_triggerSource =
//Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3;
//
//Basler_UsbCameraParams::TriggerSourceEnums ParseTriggerSourceFromUser(int sel) {
//    using T = Basler_UsbCameraParams::TriggerSourceEnums;
//    switch (sel) {
//    case 1: return T::TriggerSource_Line1;
//    case 2: return T::TriggerSource_Line2;
//    case 3: return T::TriggerSource_Line3;
//    case 4: return T::TriggerSource_Line4;
//    case 9: return T::TriggerSource_Software; // 预留：软件触发
//    default: return T::TriggerSource_Line3;   // 无效输入时回退到 Line3
//    }
//}
//
//
//int imageCount = 1;
//
//// 相机类
//// 文件名后缀imageCount在28行
//void runBasler(const std::string& folderPath, std::int16_t m_imageCount, std::double_t exposure, std::size_t countofiamgestograb) {
//    int grabbedlmages = 0;
//    imageCount = m_imageCount;
//    double m_exposure = exposure;
//    size_t m_countOfImagesToGrab = countofiamgestograb;
//    double m_gain = 1;
//    //Basler_UsbCameraParams::TriggerSourceEnums triggerSource = Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3;
//
//    PylonInitialize();
//    Pylon::CBaslerUsbInstantCamera* camera_t = NULL;
//    try
//    {
//        TlInfoList_t tls;
//        CTlFactory::GetInstance().EnumerateTls(tls);
//        std::cout << "Available TLs: " << tls.size() << std::endl;
//        for (const auto& t : tls) {
//            std::cout << " - " << t.GetFriendlyName()
//                << " (" << t.GetFullName() << ")\n";
//        }
//
//        // ===== 设备枚举 & USB 筛选 =====
//        CDeviceInfo info;
//        DeviceInfoList_t ds;
//        CTlFactory::GetInstance().EnumerateDevices(ds);
//        if (ds.empty()) {
//            std::cerr << "未发现任何相机（可能是 TL 未加载或驱动/连接问题）。\n";
//            return;
//        }
//
//        Pylon::String_t usbClass(Pylon::CBaslerUsbInstantCamera::DeviceClass());
//        bool foundUsb = false;
//        for (const auto& d : ds) {
//            if (d.GetDeviceClass().compare(usbClass) == 0) {
//                info = d;
//                foundUsb = true;
//                break;
//            }
//        }
//
//        if (!foundUsb) {
//            std::cerr << "发现相机但非 USB 设备类，或 USB TL 未加载。\n";
//            for (const auto& d : ds) {
//                std::cerr << " - " << d.GetModelName()
//                    << " via " << d.GetDeviceClass() << "\n";
//            }
//            return;
//        }
//
//        // ===== 创建相机对象 =====
//        camera_t = new CBaslerUsbInstantCamera(CTlFactory::GetInstance().CreateDevice(info));
//        camera_t->GrabCameraEvents = true;
//
//        camera_t->Open();
//        std::cout << "Open Basler Camera successfully!" << std::endl;
//
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
//        if (IsAvailable(camera_t->TriggerSource.GetEntry(g_triggerSource))) {
//            camera_t->TriggerSource.SetValue(g_triggerSource);
//        }
//        else {
//            std::cerr << "错误：所选触发源不可用，已回退到 Line3。" << std::endl;
//            if (IsAvailable(camera_t->TriggerSource.GetEntry(Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3))) {
//                camera_t->TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSourceEnums::TriggerSource_Line3);
//            }
//        }
//
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
//        camera_t->TriggerSource.SetValue(g_triggerSource);
//
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
//
//            //每8张拍摄完成一组
//            if (imageCount < 8) {
//                if (imageCount >= 8) {
//                    std::cout << "本组拍摄完毕" << std::endl;
//                }
//            }
//            else if (imageCount > 8 && imageCount < 15) {
//                if (imageCount >= 16) {
//                    std::cout << "本组拍摄完毕" << std::endl;
//                }
//            }
//            else {
//                if (imageCount == 17) {
//                    std::cout << "本组拍摄完毕" << std::endl;
//                }
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
//        std::cerr << "if Grab timed out Please choose your trigger line again" << endl;
//    }
//
//    // 释放Pylon库
//    PylonTerminate();
//}
//
//
//// 拍照方法
//void takePictures(int group, std::int16_t imagecount, double_t exposure, size_t count) {
//    std::string cur = g_savePath + std::string("/") + std::to_string(group);
//    runBasler(cur, imagecount, exposure, count);       // 将子文件夹路径传递给 runBasler
//    imageCount = 1;       // 复位下组计数
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
//        std::cout << "串口已打开" << std::endl;
//        return true;
//    }
//
//    void Close() {
//        if (hSerial != INVALID_HANDLE_VALUE) CloseHandle(hSerial);
//    }
//
//    // 把位移(mm)换成步数字节（1步=0.125mm）
//    static uint8_t MmToStepByte(double mm) {
//        const double kStepUnit = 0.125;                   // 每步 0.125 mm
//        int steps = static_cast<int>(std::lround(mm / kStepUnit));
//        if (steps < 1)   steps = 1;
//        if (steps > 255) steps = 255;                     // 单字节上限
//        return static_cast<uint8_t>(steps);
//    }
//
//    bool Move(uint8_t deviceID, bool direction, double distance_mm) {
//        uint8_t stepByte = MmToStepByte(distance_mm);     // 例如 10mm → 80 → 0x50
//        std::vector<uint8_t> frames[4] = {
//            {0x00, 0x00, 0x40, deviceID, 'P', 0x00, 0x00, stepByte, 0x00, 0x00},
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
//// 将用户输入规范化为 "COM<number>"，支持输入 "7" 或 "com7" 等
//std::string AskComPort(const std::string& defaultPort = "COM5") {
//    std::cout << "请输入串口（例如  " << defaultPort << "）：";
//    std::string in;
//    std::getline(std::cin >> std::ws, in);
//
//    if (in.empty()) return defaultPort;
//
//    // 去空白并转大写
//    for (auto& c : in) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
//
//    // 允许只输数字，例如 "7"
//    bool allDigits = !in.empty() && std::all_of(in.begin(), in.end(), ::isdigit);
//    if (allDigits) return "COM" + in;
//
//    // 允许 "COM7" 或 "com7"
//    if (in.rfind("COM", 0) != 0) {
//        // 用户可能输的是别的字符串，比如 "uart7"，尝试提取末尾数字
//        std::string digits;
//        for (char c : in) if (std::isdigit(static_cast<unsigned char>(c))) digits.push_back(c);
//        if (!digits.empty()) return "COM" + digits;
//        // 实在解析不了，就按原样返回
//    }
//    return in;
//}
//
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
//    //等待投影仪完全初始化
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//
//    GCD040101M stage;
//    const std::string comPort = AskComPort("COM5");
//    if (!stage.Open(comPort)) {
//        std::cerr << "无法打开串口 " << comPort << "\n";
//        return 1;
//    }
//    std::cout << "已使用串口：" << comPort << std::endl;
//
//    int group = 0;  // 组号从 1 开始
//
//    cout << "若出现DLP多个报错，请在LightCrafter4500软件中进行投影设置，确保关闭软件时，Validate Sequence=Stop(没报错不用管)" << endl;
//
//    cout << "-----参考实验DLP数据-----" << endl;
//    cout << "Pattern Exposure : 45000" << endl;
//    cout << "Pattern Period   : 300000" << endl;
//    cout << "-------------------------" << endl;
//    
//    // 选择保存路径（默认用宏 SAVE_PATH）
//    std::cout << "请输入保存根目录（若使用默认目录： " << SAVE_PATH << "，请输入1）：";
//    std::string inputPath;
//    std::getline(std::cin >> std::ws, inputPath);
//    if (!inputPath.empty() && inputPath != "1") {
//        g_savePath = inputPath;       // 用用户输入覆盖默认
//    }
//
//    // 不自动创建根目录：若不存在就提示并退出（避免误操作覆盖）
//    if (_access(g_savePath.c_str(), 0) != 0) {
//        std::cerr << "保存根目录不存在，请先手动创建： " << g_savePath << std::endl;
//        return 1;
//    }
//    std::cout << "本次保存根目录： " << g_savePath << std::endl;
//
//    int trigSel;
//    std::cout << "请选择相机触发源（1=Line1, 2=Line2, 3=Line3, 4=Line4, 9=Software；默认3）：";
//    std::cin >> trigSel;
//    g_triggerSource = ParseTriggerSourceFromUser(trigSel);
//    auto showTS = [](auto ts) {
//        switch (ts) {
//        case Basler_UsbCameraParams::TriggerSource_Line1: return "Line1";
//        case Basler_UsbCameraParams::TriggerSource_Line2: return "Line2";
//        case Basler_UsbCameraParams::TriggerSource_Line3: return "Line3";
//        case Basler_UsbCameraParams::TriggerSource_Line4: return "Line4";
//        case Basler_UsbCameraParams::TriggerSource_Software: return "Software";
//        default: return "Unknown";
//        }
//        };
//    std::cout << "触发源已设置为: " << showTS(g_triggerSource) << std::endl;
//
//    std::size_t nGroups;
//    std::cout << "请输入要拍摄的组数（0 表示无限循环）：";
//    std::cin >> nGroups;
//
//    int deviceID;
//    std::cout << "请输入平移台设备编号（本次实验设备编号为2）：";
//    std::cin >> deviceID;
//    deviceID = std::max(0, std::min(deviceID, 255));
//
//    double move_mm;
//    std::cout << "请输入单次移动距离(请输入0.125的倍数，移动单位为毫米)：";
//    std::cin >> move_mm;
//
//    int direction;
//    std::cout << "请输入平移台移动方向(1向后，0向前):";
//    cin >> direction;
//
//    cout << "请输入首张图片编号（参考：同心圆 1；螺旋光 9；棋盘格 17)：";
//    std::int16_t imagecount;
//    cin >> imagecount;
//
//    cout << "请输入曝光时间(参考数据：同心圆&螺旋光 125000；棋盘格 4000)：";
//    std::double_t exposure;
//    cin >> exposure;
//
//    cout << "请输入单组拍摄图片数量(参考：同心圆&螺旋光 8；棋盘格  1)：";
//    std::size_t count;
//    cin >> count;
//
//    cout << "设置成功,拍摄开始" << endl;
//
//    // 无限循环：先投影→再拍照→平移台移动
//    do {
//        ++group;
//        std::string folder = g_savePath + std::string("/") + std::to_string(group);
//
//        int ret = _mkdir(folder.c_str());
//        if (ret != 0 && errno != EEXIST) {
//            std::cerr << "创建目录失败: " << folder << '\n';
//        }
//
//
//        std::cout << "===== 第 " << group << " 组 =====" << std::endl;
//
//        // 启动投影
//        runDLP();
//        std::this_thread::sleep_for(std::chrono::seconds(1)); // 确保投影稳定
//
//        // 拍照
//        takePictures(group, imagecount, exposure, count);
//
//        // 停止投影
//        PatSeqCtrlStop();
//        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 确保投影停止
//
//        if (nGroups != 0 && group >= nGroups) {
//            std::cout << "已完成 " << nGroups << " 组拍摄，程序结束。" << std::endl;
//            break;
//        }
//
//        // 平移台移动
//        stage.Move(static_cast<uint8_t>(deviceID), static_cast<bool>(direction), move_mm);
//        std::this_thread::sleep_for(std::chrono::seconds(2));
//
//    } while (true);
//
//    system("pause");
//    return 0;
//}
