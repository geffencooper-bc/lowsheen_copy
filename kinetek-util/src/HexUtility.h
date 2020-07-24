// a utility class with helper functions to help extract data from a hex file
// Note: this class assumes that data records are 16 bytes long

// Hex file details

// Entries in hex files (called records) follow this format
//
// :llaaaatt[dd...dd]cc
//
// :          signifies the start of a record
// ll         signifies the number of bytes in the data field of the record
// aaaa       signifies the address of this data field
// tt         signifies the record type
// [dd...dd]  signifies the data bytes
// cc         signifies the two byte checksum


// function details

// CAN data is sent as an array of bytes, ex: {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
// a portion of the data array may contain a number like the starting address that requires multiple bytes

// ex: starting address 0x08008000 gets split up into 0x08 0x00 0x80 0x00 and placed into the data array

// some functions will require an array to be passed in to store these bytes. The according section of the data
// array can be passed in as the buffer --> ex: to get the 4 address bytes from above, you might pass in
// position two of a data array and a size of 4.


#ifndef HEX_UTIL_H
#define HEX_UTIL_H

#include <string>
#include <fstream>
#include <stdint.h>

using std::string;
using std::ifstream;

enum hex_record_type
{
    DATA = 0,
    END_OF_FILE = 1,
    EXTENDED_SEGMENT_AR = 2,
    EXTENDED_LINEAR_AR = 4,
    START_LINEAR_AR = 5
};

// start index of fields in a hex record
enum record_indices
{
    RECORD_DATA_LENGTH_START_I = 1,
    RECORD_ADDRESS_START_I = 3,
    RECORD_TYPE_START_I = 7,
    RECORD_DATA_START_I = 9
};

#define CAN_DATA_LEN 8
#define HEX_DATA_RECORD_LEN 16

class HexUtility
{
    public:
    // opens file, then loads hex data like checksums, data size, etc
    HexUtility(const string &hex_file_path);

    // closes file
    ~HexUtility();

    // parameters: >= 4 byte array to be filled with data size.
    // return: data size as an int
    int get_file_data_size(uint8_t* byte_array, uint8_t arr_size);
    
    // parameters: >= 4 byte array to be filled with checksum, stored in reverse for kinetek format
    // return: checksum as an int 
    int get_total_cs(uint8_t* byte_array, uint8_t arr_size, bool rev=true);

    // parameters: >= 4 byte array to be filled with start address
    // return: start addess as an int
    int get_start_address(uint8_t* byte_array, uint8_t arr_size);

    // parameters: >= 8 byte array to be filled with the next 8 data bytes in the hex file
    // return: sum of the 8 data bytes as an int, returns -1 if there is no more data (EOF)
    // Note: if record has less than 8 bytes, remaining bytes will be filled with 0xFF (not included in sum)
    int get_next_8_bytes(uint8_t* byte_array, uint8_t arr_size);
    
    // converts a number's hex representation to a list of bytes
    // parameters: number to convert, array to store byte representation
    // Note: array size determines representation (if to add extra 0x00 filler)
    // ex: 1000 in hex is 0x3E8 --> {0x03, 0xE8}
    void num_to_byte_list(int num, uint8_t* byte_array, uint8_t arr_size);


    private:
    ifstream hex_file; // file is open for object lifetime
    string curr_line;  // file will be read line by line

    bool is_first_8;   // reading 1st 8 data bytes or next 8 data bytes in each hex record 
    bool is_eof;       
    uint32_t hex_file_data_size;      
    uint32_t total_checksum; 
    uint32_t start_address;
    
    
    // Helper functions

    // first five functions extract and return certain part of hex record fed in as a string
    int get_record_data_length(const string &hex_record); // in bytes
    int get_record_address(const string &hex_record);
    hex_record_type get_record_type(const string &hex_record);
    // fills an array passed in with the data portion of a hex record
    // can specify number of bytes to get from a starting byte (ex: byte 2), returns sum of the bytes
    int get_record_data_bytes(const string &hex_record, uint8_t* data_bytes, uint8_t num_data_bytes, int start=0, int num_bytes=-1);
    int get_record_checksum(const string &hex_record);

    // converts a string of bytes "AABBCCDD" to an array of bytes {0xAA, 0xBB, 0xCC, 0xDD}
    int data_string_to_byte_list(const string &hex_data, uint8_t* data_bytes, uint8_t num_data_bytes);
    int load_hex_file_data();    
    uint8_t calc_hex_checksum(const string &hex_record);
};

#endif