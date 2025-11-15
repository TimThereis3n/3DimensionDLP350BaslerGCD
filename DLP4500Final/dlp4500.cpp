#include"dlp4500.h"

ControlDLP4500::ControlDLP4500()
{
	if (DLPC350_USB_Init() != 0) {
		std::cout << "USB_Init_Error" << std::endl;
	}
	else {
		std::cout << "USB_Init" << std::endl;
	}
	DLPC350_USB_Open();
	if (DLPC350_USB_IsConnected()) {
		std::cout << "USB_Conected" << std::endl;

		if (DLPC350_GetVersion(&APP_ver, &API_ver, &SWConfig_ver, &SeqConfig_ver) == 0) {
			sprintf(versionStr, "%d.%d.%d", (APP_ver >> 24), ((APP_ver << 8) >> 24), ((APP_ver << 16) >> 16));
			std::cout << versionStr << std::endl;
		}

		unsigned char firmwareTag[33];
		if (DLPC350_GetFirmwareTagInfo(&firmwareTag[0]) == 0)
		{
			std::cout << firmwareTag << std::endl;
		}

		bool standby;
		if (DLPC350_GetPowerMode(&standby) != 0) {
			std::cout << "Power Mode error" << std::endl;
		}

		SetDLPC350CInPatternMode();

	}
	else {
		std::cout << "USB_Conected_Error" << std::endl;
	}

	//--------------------------------------------------------------------
	PatSeqCtrlStop();


	if (DLPC350_StartPatLutValidate()) {
		std::cout << "Error validating LUT data" << std::endl;
	}
	unsigned int status;
	bool ready;
	for (int i = 0; i < MAX_NUM_RETRIES; i++) {
		if (DLPC350_CheckPatLutValidate(&ready, &status) < 0)
		{
			if ((status & BIT0) == BIT0)
			{
				std::cerr << "Exposure or frame period OUT OF RANGE" << std::endl;
			}
			if ((status & BIT1) == BIT1)
			{
				std::cerr << "Pattern number in lookup table INVALID" << std::endl;
			}
			if ((status & BIT2) == BIT2)
			{
				cerr << "Continued output trigger OVERLAPS black vector" << std::endl;
			}
			if ((status & BIT3) == BIT3)
			{
				std::cerr << "Black vector MISSING when exposure less than frame period" << std::endl;
			}
			if ((status & BIT4) == BIT4)
			{
				std::cerr << "Difference between exposure and frame period less than 230us" << std::endl;
			}
			if (i == MAX_NUM_RETRIES - 1) {
				std::cout << "out of time" << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
		else {
			break;
		}
	}
}

ControlDLP4500::~ControlDLP4500()
{
	PatSeqCtrlStop();
	DLPC350_USB_Close();
	DLPC350_USB_Exit();
}

void ControlDLP4500::runDLP()
{
	DLPC350_PatternDisplay(2);
}

void ControlDLP4500::PatSeqCtrlStop()
{
	int index = 0;
	unsigned int patMode;
	DLPC350_PatternDisplay(0);
	while (1)
	{
		DLPC350_GetPatternDisplay(&patMode);
		std::cout << "Patmode: " << patMode << std::endl;
		if (patMode == 0) {
			break;
		}
		else {
			DLPC350_PatternDisplay(0);
		}
		if (index++ > MAX_NUM_RETRIES) {
			std::cout << "pattern display mode set fail" << std::endl;
			break;
		}
	}
}

void ControlDLP4500::SetDLPC350CInPatternMode()
{
	int index = 0;
	bool mode;
	unsigned int patMode;
	DLPC350_GetMode(&mode);
	if (mode == false) {
		DLPC350_SetMode(true);
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		while (1)
		{
			DLPC350_GetMode(&mode);
			if (mode) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			if (index++ > MAX_NUM_RETRIES) {
				break;
			}
		}
	}
	else {
		DLPC350_GetPatternDisplay(&patMode);
		if (patMode != 0) {
			PatSeqCtrlStop();
		}

	}
	return;
}