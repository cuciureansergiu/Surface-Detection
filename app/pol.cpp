/***************************************************************************************
 ***                                                                                 ***
 ***  Copyright (c) 2019, Lucid Vision Labs, Inc.                                    ***
 ***                                                                                 ***
 ***  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     ***
 ***  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       ***
 ***  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    ***
 ***  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         ***
 ***  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  ***
 ***  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  ***
 ***  SOFTWARE.                                                                      ***
 ***                                                                                 ***
 ***************************************************************************************/

#include "stdafx.h"
#include "ArenaApi.h"
#include "SaveApi.h"
#include "string.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define TAB1 "  "
#define TAB2 "    "
#if defined _WIN32
#define PIXEL_FORMAT BGR8
#elif defined __linux__
#define PIXEL_FORMAT RGB8
#endif

#define FILE_NAME_PATTERN "Images/<format>.jpg"
#define FORMAT_MONO             "Mono8"
#define FORMAT_ANGLES           "PolarizedAngles_0d_45d_90d_135d_Mono8"
#define FORMAT_AOLP             "PolarizedAolp_Mono8"
#define FORMAT_DOLP             "PolarizedDolp_Mono8"
#define PACKET_SIZE 		        1500
#define IMAGE_TIMEOUT 		      2000
#define SYSTEM_TIMEOUT 		      100

void AcquireFrame(Arena::IDevice* pDevice, const char* pixelFormat)
{
	// Save initial values
	GenApi::CIntegerPtr pWidth = pDevice->GetNodeMap()->GetNode("Width");
	GenApi::CIntegerPtr pHeight = pDevice->GetNodeMap()->GetNode("Height");
        GenApi::CEnumerationPtr pPixelFormat = pDevice->GetNodeMap()->GetNode("PixelFormat");
	GenApi::CIntegerPtr pDeviceStreamChannelPacketSize = pDevice->GetNodeMap()->GetNode("DeviceStreamChannelPacketSize");
        GenICam::gcstring pixelFormatInitial = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PixelFormat");

        /*      
        GenICam::gcstring exposureAutoInitial = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "ExposureAuto");
        double exposureTimeInitial = Arena::GetNodeValue<double>(pDevice->GetNodeMap(), "ExposureTime");
        std::cout << TAB1 << "Set minimum exposure time";

        Arena::SetNodeValue<GenICam::gcstring>(
                pDevice->GetNodeMap(),
                "ExposureAuto",
                "Off");

        GenApi::CFloatPtr pExposureTime = pDevice->GetNodeMap()->GetNode("ExposureTime");

        if (!pExposureTime || !GenApi::IsReadable(pExposureTime) || !GenApi::IsWritable(pExposureTime))
        {
                throw GenICam::GenericException("ExposureTime node not found/readable/writable", __FILE__, __LINE__);
        }

        std::cout << " (" << pExposureTime->GetMin() << " " << pExposureTime->GetUnit() << ")\n";

        pExposureTime->SetValue(pExposureTime->GetMin());
        */

        Arena::SetNodeValue<GenICam::gcstring>(
                pDevice->GetNodeMap(),
                "PixelFormat",
                pixelFormat);

        std::cout << "\n\nPixel format : " << pixelFormat;

	if (!pWidth || !pHeight || !pPixelFormat)
	{
		throw GenICam::GenericException("Width, Height, or PixelFormat node could not be found", __FILE__, __LINE__);
	}

	if (!GenApi::IsReadable(pWidth) || !GenApi::IsReadable(pHeight) || !GenApi::IsReadable(pPixelFormat))
	{
		throw GenICam::GenericException("Width, Height, or PixelFormat node not readable", __FILE__, __LINE__);
	}

	pDeviceStreamChannelPacketSize->SetValue(9000);

	std::cout << "\nPacket size : " << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "DeviceStreamChannelPacketSize") << " " << pDeviceStreamChannelPacketSize->GetUnit();

        //pWidth->SetValue(1280);
        //pHeight->SetValue(720);
        std::cout << "\nDimensions : " << pWidth->GetValue() << "x" << pHeight->GetValue();

	Save::ImageParams params(
		static_cast<size_t>(pWidth->GetValue()),
		static_cast<size_t>(pHeight->GetValue()),
		Arena::GetBitsPerPixel(pPixelFormat->GetCurrentEntry()->GetValue()));

	Save::ImageWriter writer(
		params,
		FILE_NAME_PATTERN);
        writer << "<format>" << pixelFormat;

        pDevice->StartStream();

        Arena::IImage* pImage = pDevice->GetImage(IMAGE_TIMEOUT);

        writer << pImage->GetData();


        // clean up

        pDevice->RequeueBuffer(pImage);

        pDevice->StopStream();


        // return nodes to their initial values
        Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PixelFormat", pixelFormatInitial);
}

int main()
{
        bool exceptionThrown = false;

        try
        {
                // prepare example
                Arena::ISystem* pSystem = Arena::OpenSystem();
                pSystem->UpdateDevices(SYSTEM_TIMEOUT);
                std::vector<Arena::DeviceInfo> deviceInfos = pSystem->GetDevices();
                if (deviceInfos.size() == 0)
                {
                        std::cout << "\nNo camera connected\nPress enter to complete\n";
                        std::getchar();
                        return 0;
                }
                Arena::IDevice* pDevice = pSystem->CreateDevice(deviceInfos[0]);

                std::cout << "\n\nSaving frames\n\n";

                AcquireFrame(pDevice,FORMAT_MONO);
                AcquireFrame(pDevice,FORMAT_AOLP);
                AcquireFrame(pDevice,FORMAT_DOLP);

                // clean up example
                pSystem->DestroyDevice(pDevice);
                Arena::CloseSystem(pSystem);
        }
        catch (GenICam::GenericException& ge)
        {
                std::cout << "\nGenICam exception thrown: " << ge.what() << "\n";
                exceptionThrown = true;
        }
        catch (std::exception& ex)
        {
                std::cout << "\nStandard exception thrown: " << ex.what() << "\n";
                exceptionThrown = true;
        }
        catch (...)
        {
                std::cout << "\nUnexpected exception thrown\n";
                exceptionThrown = true;
        }

        std::cout << "\n\nAcquisition done\n";

        if (exceptionThrown)
                return -1;
        else
                return 0;
}
