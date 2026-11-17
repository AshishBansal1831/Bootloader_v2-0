#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>

#include "InterfaceIO/linux_serial.hpp"
#include "as_pkt/as_pkt.hpp"
#include "buffering/debug_packet.hpp"
#include "firmware_updater.hpp"


/*
 * Function Declarations
 */
void decode_packet(const AsPkt &Pkt);
static bool ends_with(const char *str, const char *suffix);
void decode_debug_packet(const char *Tag, const uint16_t TagSize,  uint8_t *const paramBuffer, uint16_t Length);
void bootloader_handler(const ACKFLAG Flag);


/*
 * Global Variables
 */
std::ifstream fmwrFile;
FirmwareData firmwareDataObj;
AsPkt RecvPacket;
AsPkt SendPacket;
LinuxSerial *myport = nullptr;



/*
 * Function definitions
 */

/*--------------------------------------*/
/*          MAIN FUNCTION               */
/*--------------------------------------*/
int main(int argc, char *argv[])
{
    char portPath[30] = "";
    uint32_t bd = 0;
    char fileName[30] = ""; 

    if(argc < 2) // Port
    {
        std::cout << "Path - ";
        std::cin >>  portPath;
    }
    else
    {
        strcpy(portPath, argv[1]);
    }


    if(argc < 3) // Baudrate
    {
        std::cout << "Baudrate - ";
        std::cin >> bd;
    }
    else
    {
        bd = static_cast<uint32_t>(std::stoi(argv[2]));
    }

    if(argc < 4) // filename
    {
        std::cout << "Firmware File : ";
        std::cin >> fileName;
    }
    else
    {
        strcpy(fileName, argv[3]);
    }

    // Pass port path and baud rate via constructor
    myport = new LinuxSerial(portPath, bd);
    myport->init();

    if(myport->error != 0)
    {
        std::cout << "Port not opened "  << (int)myport->error << "\r\n";
        delete myport;
        return EXIT_FAILURE;
    }

    if(ends_with(fileName, ".hex") == 1)
    {
        fmwrFile.open(fileName);
        if(!fmwrFile.is_open())
        {
            std::cerr << "Failed to open file: " << fileName << std::endl;
            return false;
        }

        firmwareDataObj.fileType = 1; // HEX FILE
    }
    else if(ends_with(fileName, ".bin") == 1)
    {
        fmwrFile.open(fileName, std::ios::binary | std::ios::ate );
        if(!fmwrFile.is_open())
        {
            std::cerr << "Failed to opn file: " << fileName << std::endl;
            return false;
        }

        firmwareDataObj.fileType = 0; // binary FILE
    }
    else {
        std::cout << "UNSUPPORTED FILE TYPE\r\n";
    }

    // parse the complete file.
    fileParse();

    //tell the device to enter firmware update mode
    SendPacket.reset();
    SendPacket.pkt_id = PktID::FIRMWARE_UPDATE;

    uint8_t payload = 0;

    SendPacket.make_packet(&payload, 1);

    myport->write(SendPacket.payload, SendPacket.length);

    // bootloader handler will run now under main loop
    RecvPacket.reset();
    while(1)
    {
        while(RecvPacket.get_packet(myport->receive()))
        {
            decode_packet(RecvPacket);
        }
    }

    delete myport;
    return 0;
}

void decode_packet(const AsPkt &Pkt)
{
    // std::cout << "Got the packet" << std::endl;
    // std::cout << "Payload Length - " << Pkt.length << std::endl;
    // std::cout << "Flags - " << static_cast<int>(Pkt.flagsRaised) << std::endl;
    // std::cout << "PacketID - " << static_cast<int> (Pkt.pkt_id) << std::endl;
    // std::cout << "RecvPacket : " << std::endl;

    // for(uint8_t Iter = 0; Iter < Pkt.length; ++Iter)
    // {
    //     std::cout << std::hex << static_cast<int> (Pkt.payload[Iter]) << " ";
    // }

    // std::cout << std::endl;

    switch(Pkt.pkt_id)
    {
        case PktID::DEBUG_PRINT :
            {
                const char *Tag = (const char *)Pkt.payload;
                uint16_t strl = strlen(Tag);
                decode_debug_packet(Tag, strl, (uint8_t*)Pkt.payload + strl, Pkt.length);
            }
            break;

        case PktID::OPERATION_ACK:
            bootloader_handler(static_cast<ACKFLAG> (RecvPacket.payload[0]));
            break;

        default:
            break;
    }
}

static bool ends_with(const char *str, const char *suffix) 
{
    if (!str || !suffix)
    {
        return false;
    }
    
    size_t str_len = std::strlen(str);
    size_t suffix_len = std::strlen(suffix);
    
    // Suffix cannot be longer than the string itself
    if (suffix_len > str_len) return false;
    
    // Compare the tail of the string with the suffix
    return std::strcmp(str + str_len - suffix_len, suffix) == 0;
}

void decode_debug_packet(const char *Tag, const uint16_t TagSize,  uint8_t *const paramBuffer, uint16_t Length)
{
    std:: cout << "TAG : " << (const char *)paramBuffer << " : " << std::endl;

    uint8_t *Buffer = paramBuffer + TagSize;

    const uint8_t * const end_Ptr = Buffer + Length;

    while(Buffer < end_Ptr)
    {
        const DebugDataType type =  static_cast<DebugDataType>(*Buffer);
        
        Buffer++;

        switch(type)
        {
            case DebugDataType::U8 :
                std::cout << " U8 : " << (unsigned int) (*Buffer);
                Buffer++;
                break;

            case DebugDataType::CH :
                std::cout << " CH : " << (char) (*Buffer);
                Buffer++;
                break;

            case DebugDataType::U16 :
            {
                uint16_t temp = *(uint16_t*)(Buffer);
                std::cout << " U16 : " << temp;
                Buffer+=2;
            }
            break;

            case DebugDataType::U32 :
            {
                uint32_t temp = *(uint32_t*)(Buffer);
                std::cout << " U32 : " << temp;
                Buffer+=4;
            }
            break;

            case DebugDataType::U64 :
            {
                uint64_t temp = *(uint64_t*)(Buffer);
                std::cout << " U64 : " << temp;
                Buffer+=8;
            }
            break;

            case DebugDataType::I8 :
                std::cout << " I8 : " << (int) (*Buffer);
                Buffer++;
                break;

            case DebugDataType::I16 :
            {
                int16_t temp = *(int16_t*)(Buffer);
                std::cout << " I16 : " << temp;
                Buffer+=2;
            }
            break;

            case DebugDataType::I32 :
            {
                int32_t temp = *(int32_t*)(Buffer);
                std::cout << " I32 : " << temp;
                Buffer+=4;
            }
            break;

            case DebugDataType::I64 :
            {
                int64_t temp = *(int64_t*)(Buffer);
                std::cout << " I64 : " << temp;
                Buffer+=8;
            }
            break;

            case DebugDataType::F32 :
            {
                float temp = *(float*)(Buffer);
                std::cout << " F32 : " << temp;
                Buffer+=4;
            }            
            break;

            case DebugDataType::F64 :
            {
                double temp = *(double*)(Buffer);
                std::cout << " F64 : " << temp;
                Buffer+=8;
            }

            case DebugDataType::STR :
            {
                uint16_t Len = *(uint16_t*)Buffer;
                Buffer+=2;
                std::cout << " STR (" << Len << ") : " << std::string_view((const char *)Buffer, Len);
                Buffer+=Len;
            }            
            break;

            case DebugDataType::U8P :
            {
                uint16_t Len = *(uint16_t*)Buffer;
                Buffer+=2;

                std::cout << " U8P (" << Len << ") : " << std::endl;
                
                for(uint8_t Iter = 0; Iter < Len; ++Iter, Buffer++)
                {
                    std::cout << std::hex << (int) (*Buffer); 
                }
                Buffer+=Len;
            }
            break;

            default :
                std::cout << "Invalid value " << (int)type << std::endl;
        }

        std::cout << std::endl;
    }
}