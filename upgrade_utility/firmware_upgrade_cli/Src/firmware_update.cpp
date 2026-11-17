#include <iostream>
#include <fstream>
#include <string>
#include "firmware_updater.hpp"
#include "as_pkt/as_pkt.hpp"
#include "InterfaceIO/linux_serial.hpp"
#include "buffering/buffering.hpp"
#include "crc32_.hpp"

struct HexRecord {
    uint8_t length;
    uint32_t address;
    uint8_t type;
    std::vector<uint8_t> data;
};

extern std::ifstream fmwrFile;
extern FirmwareData firmwareDataObj;
extern AsPkt RecvPacket;
extern AsPkt SendPacket;
extern LinuxSerial *myport;

constexpr uint16_t PAYLOAD_LEN = 256;

uint16_t PacketNo = 0;
uint16_t total_packets;

extern void decode_debug_packet(const char *Tag, const uint16_t TagSize,  uint8_t *const paramBuffer, uint16_t Length);
bool parse_hex_line(const std::string &line, HexRecord &record);

Buffering<PAYLOAD_LEN + 50> payload;
bool eraseDone;

uint8_t hex_char_to_byte(const char HexByte)
{
    if(HexByte >= '0' && HexByte <= '9')
    {
        return HexByte - 48U; 
    } 
    
    if(HexByte >= 'A' && HexByte <= 'F')
    {
        return HexByte - 'A' + 0x0A; 
    }

    if(HexByte >= 'a' && HexByte <= 'f')
    {
        return HexByte - 'a' + 0x0A;
    }

    return 0;
}

void bootloader_handler(const ACKFLAG Flag)
{
    std::cout << "bootloader handler - " << (signed int)(Flag) << " firmwareDataObj.Stage " << (int) firmwareDataObj.Stage << std::endl;

    switch (firmwareDataObj.Stage)
    {
    case FirmwarUpdateStages::START_FIRMWARE_UPDATE:
        {
            if(Flag == ACKFLAG::ACK_BOOT_RESP2)
            {
                SendPacket.reset();
                SendPacket.pkt_id = PktID::FIRMWARE_UPDATE;

                total_packets = (firmwareDataObj.size / PAYLOAD_LEN) + 
                                         ((firmwareDataObj.size % PAYLOAD_LEN) == 0 ? 0 : 1);

                // Send firmware size (u32) and total number of packets (u16)
                payload.clear();
                payload << firmwareDataObj.size << total_packets; 
                fflush(stdout);
                std::cout << "firmware size - " << (int)firmwareDataObj.size << " Packet " << total_packets << std::endl;
                // for(uint16_t Iter = 0; Iter < payload.length(); ++Iter)
                // {
                //     printf("%02X, ", *(uint8_t*)(payload.data() + Iter));
                // }
                SendPacket.make_packet(payload.data(), payload.length());
                myport->write(SendPacket.payload, SendPacket.length);

                firmwareDataObj.Stage = FirmwarUpdateStages::CONFIRM_ERASE;
            }
            else if(Flag == ACKFLAG::ACK_BOOT_RESP1)
            {
                myport->write(SendPacket.payload, SendPacket.length);
            }
        }
        break;

    case FirmwarUpdateStages::CONFIRM_ERASE:
        {
            if(Flag != ACKFLAG::ACK)
            {
                if(firmwareDataObj.retyCnt < 3)
                {
                    firmwareDataObj.retyCnt++;
                    firmwareDataObj.Stage = FirmwarUpdateStages::START_FIRMWARE_UPDATE;
                }
                else
                {
                    std::cerr << "Erase didn't occur even after 3 tries.\r\n";
                    delete myport;
                    exit(-1); 
                }
            }
            else
            {
                // Erase successful, prepare and send the FIRST data packet (Packet 0)
                firmwareDataObj.offset = 0;
                PacketNo = 0;

                uint16_t current_chunk_len = static_cast<uint16_t>(
                    std::min(
                        static_cast<size_t>(PAYLOAD_LEN), 
                        static_cast<size_t>(firmwareDataObj.size - firmwareDataObj.offset))
                );

                payload.clear();
                payload << PacketNo << current_chunk_len;
                payload.add(firmwareDataObj.firmwareData.data() + firmwareDataObj.offset, current_chunk_len);
                
                SendPacket.reset();
                SendPacket.flagsRaised = 0;
                SendPacket.pkt_id = PktID::FIRMWARE_UPDATE;
                SendPacket.make_packet(payload.data(), payload.length());

                printf("payload Length - %d\r\n", SendPacket.length);
                for(uint16_t Iter = 0; Iter < SendPacket.length; ++Iter)
                {
                    printf("%02X, ", SendPacket.payload[Iter]);
                }
                printf("\r\n");
                myport->write(SendPacket.payload, SendPacket.length);

                firmwareDataObj.Stage = FirmwarUpdateStages::CONTINUE_PACKET_SENDING;
            }
        }
        break;

    case FirmwarUpdateStages::CONTINUE_PACKET_SENDING:
        {
            if (Flag == ACKFLAG::ACK)
            {
                // Reset retry counter on a successful ACK
                firmwareDataObj.retyCnt = 0;

                // 1. Advance offset by the chunk size we just successfully sent and got ACKed for
                uint16_t sent_len = static_cast<uint16_t>(
                    std::min(
                        static_cast<size_t>(PAYLOAD_LEN), 
                        static_cast<size_t>(firmwareDataObj.size - firmwareDataObj.offset))
                );
                firmwareDataObj.offset += sent_len;
                PacketNo++;

                // 2. Check if all bytes have been sent
                if (PacketNo >= total_packets) {
                    std::cout << "All firmware packets sent successfully! Triggering jump." << std::endl;
                    
                    uint32_t checksum = calculate_crc32(firmwareDataObj.firmwareData);
                    std::cout << "Firmware update complete. Verifying checksum " << std::hex << checksum << std::dec << std::endl;
                    payload.clear();
                    payload << checksum;
                    SendPacket.reset();
                    SendPacket.flagsRaised = 0;
                    SendPacket.pkt_id = PktID::FIRMWARE_UPDATE;
                    SendPacket.make_packet(payload.data(), payload.length());
                    printf("payload length - %d\r\n", SendPacket.length);
                    for(uint16_t Iter = 0; Iter < SendPacket.length; ++Iter)
                    {
                        printf("%02X, ", SendPacket.payload[Iter]);
                    }
                    printf("\r\n");

                    myport->write(SendPacket.payload, SendPacket.length);
                
                    
                    firmwareDataObj.Stage = FirmwarUpdateStages::JUMP_TO_APP;
                    break;
                }

                // 3. Prepare next chunk for the new PacketNo and offset
                uint16_t next_chunk_len = static_cast<uint16_t>(
                    std::min(
                        static_cast<size_t>(PAYLOAD_LEN), 
                        static_cast<size_t>(firmwareDataObj.size - firmwareDataObj.offset))
                );

                std::cout << "Offset: " << firmwareDataObj.offset << " / " << firmwareDataObj.size << " " << next_chunk_len << std::endl;
                
                payload.clear();
                payload << PacketNo << next_chunk_len;                
                payload.add(firmwareDataObj.firmwareData.data() + firmwareDataObj.offset, next_chunk_len);
                
                SendPacket.reset();
                SendPacket.pkt_id = PktID::FIRMWARE_UPDATE;
                SendPacket.make_packet(payload.data(), payload.length());

                printf("payload length - %d %d\r\n", SendPacket.length, next_chunk_len);
                for(uint16_t Iter = 0; Iter < SendPacket.length; ++Iter)
                {
                    printf("%02X, ", SendPacket.payload[Iter]);
                }
                printf("\r\n");
                
                // Transmit the new packet
                myport->write(SendPacket.payload, SendPacket.length);
            }
            else 
            {
                // NACK or timeout error handling
                firmwareDataObj.retyCnt++;
                std::cout << "Write error/NACK occurred on packet " << PacketNo << ", retry count: " << firmwareDataObj.retyCnt << std::endl;
                
                if (firmwareDataObj.retyCnt < 3)
                {
                    // Re-transmit the exact same packet payload currently buffered in SendPacket
                    myport->write(SendPacket.payload, SendPacket.length);
                }
                else
                {
                    std::cerr << "Max retries reached for packet " << PacketNo << ". Aborting update.\r\n";
                    delete myport;
                    exit(EXIT_FAILURE);
                }
            }
        }
        break;

    case FirmwarUpdateStages::JUMP_TO_APP:
            if(Flag == ACKFLAG::ACK)
            {
                std::cout << "success " << std::endl;
                SendPacket.reset();
                SendPacket.pkt_id = PktID::FIRMWARE_UPDATE;
                payload.clear();
                SendPacket.make_packet(payload.data(), payload.length());

                myport->write(SendPacket.payload, SendPacket.length);
                usleep(200* 1000) ;
                delete myport;
                exit(EXIT_SUCCESS);
            }
        break;
    
    default:
        break;
    }
}

void fileParse()
{
    if(firmwareDataObj.fileType == 0) // binary file
    {
        // Get file size
        fmwrFile.seekg(0, std::ios::end);
        std::streamsize size = fmwrFile.tellg();
        fmwrFile.seekg(0, std::ios::beg);
        
        firmwareDataObj.firmwareData.resize(size);
        
        if(fmwrFile.read(reinterpret_cast<char*>(firmwareDataObj.firmwareData.data()), size)) 
        {
            std::cout << "Successfully read " << size << " bytes." << std::endl;
            firmwareDataObj.size = static_cast<uint32_t>(size);
        }
        else 
        {
            std::cerr << "Error: Read fewer bytes than expected!" << std::endl;
        }
    }
    else if(firmwareDataObj.fileType == 1) // Intel HEX file
    {
        std::string line;
        HexRecord record;
        
        uint32_t upper_base_address = 0;
        uint32_t lowest_address = 0xFFFFFFFF;
        uint32_t highest_address = 0;
        
        // Temporary structure to store parsed records before mapping to contiguous vector
        struct MemBlock {
            uint32_t address;
            std::vector<uint8_t> data;
        };
        std::vector<MemBlock> memory_blocks;

        // Pass 1: Parse all lines and collect data blocks
        while(std::getline(fmwrFile, line))
        {
            // Strip trailing carriage returns if reading files from Windows/Linux mixed environments
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (!parse_hex_line(line, record)) continue;

            if (record.type == 0x00) // Data Record
            {
                uint32_t absolute_address = upper_base_address + record.address;
                
                MemBlock block;
                block.address = absolute_address;
                block.data = record.data;
                memory_blocks.push_back(block);

                if (absolute_address < lowest_address) {
                    lowest_address = absolute_address;
                }
                if ((absolute_address + record.length) > highest_address) {
                    highest_address = absolute_address + record.length;
                }
            }
            else if (record.type == 0x04) // Extended Linear Address Record
            {
                if (record.data.size() >= 2) {
                    upper_base_address = (static_cast<uint32_t>(record.data[0]) << 24) | 
                                         (static_cast<uint32_t>(record.data[1]) << 16);
                }
            }
            else if (record.type == 0x01) // End of File Record
            {
                break;
            }
        }

        // Pass 2: Map scattered blocks into a single contiguous vector
        if (!memory_blocks.empty() && highest_address > lowest_address)
        {
            uint32_t total_span = highest_address - lowest_address;
            firmwareDataObj.firmwareData.assign(total_span, 0xFF); // Initialize with 0xFF (erased flash state)

            for (const auto &block : memory_blocks) {
                uint32_t offset = block.address - lowest_address;
                for (size_t i = 0; i < block.data.size(); ++i) {
                    firmwareDataObj.firmwareData[offset + i] = block.data[i];
                }
            }

            firmwareDataObj.size = total_span;
            std::cout << "Successfully parsed HEX file. Total size: " << firmwareDataObj.size 
                      << " bytes (Start: 0x" << std::hex << lowest_address << std::dec << ")." << std::endl;
        }
        else
        {
            std::cerr << "Error: No valid data records found in HEX file!" << std::endl;
        }
    }

    fmwrFile.close();
}


bool parse_hex_line(const std::string &line, HexRecord &record) {
    if (line.empty() || line[0] != ':') return false;

    auto hex_to_byte = [](char c1, char c2) {
        char str[3] = {c1, c2, '\0'};
        return static_cast<uint8_t>(std::stoul(str, nullptr, 16));
    };

    record.length = hex_to_byte(line[1], line[2]);
    uint16_t addr_offset = (hex_to_byte(line[3], line[4]) << 8) | hex_to_byte(line[5], line[6]);
    record.type = hex_to_byte(line[7], line[8]);

    record.data.resize(record.length);
    for (int i = 0; i < record.length; ++i) {
        record.data[i] = hex_to_byte(line[9 + (i * 2)], line[10 + (i * 2)]);
    }

    // Base address calculation handled via Extended Linear Address (Type 04) when needed
    record.address = addr_offset;
    return true;
}


