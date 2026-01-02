// HexToBin.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

// Convert a hexadecimal character to its numeric value
int HexCharToInt(wchar_t c)
{
    if (c >= L'0' && c <= L'9')
        return c - L'0';
    if (c >= L'A' && c <= L'F')
        return c - L'A' + 10;
    if (c >= L'a' && c <= L'f')
        return c - L'a' + 10;
    return -1; // Invalid hex character
}

int main()
{
    std::cout << "Enter name for binary output file.\n";
    
    std::string filename;
    std::cin >> filename;
    filename += ".bin";
    
    // Open the clipboard
    if (!OpenClipboard(nullptr))
    {
        std::cout << "Failed to open clipboard.\n";
        std::cout << "Enter any number to exit this program.\n";
        int dummy;
        std::cin >> dummy;
        return 1;
    }
    
    // Get the clipboard data as Unicode text
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == nullptr)
    {
        std::cout << "No text data in clipboard.\n";
        CloseClipboard();
        std::cout << "Enter any number to exit this program.\n";
        int dummy;
        std::cin >> dummy;
        return 1;
    }
    
    // Lock the handle to get a pointer to the text
    const wchar_t* hexString = static_cast<const wchar_t*>(GlobalLock(hData));
    if (hexString == nullptr)
    {
        std::cout << "Failed to access clipboard data.\n";
        CloseClipboard();
        std::cout << "Enter any number to exit this program.\n";
        int dummy;
        std::cin >> dummy;
        return 1;
    }
    
    // Open the output file
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile)
    {
        std::cout << "Failed to create output file.\n";
        GlobalUnlock(hData);
        CloseClipboard();
        std::cout << "Enter any number to exit this program.\n";
        int dummy;
        std::cin >> dummy;
        return 1;
    }
    
    // Buffer for binary data
    unsigned char buffer[4096];
    int bufferIndex = 0;
    size_t totalBytesWritten = 0;
    size_t i = 0;
    
    // Process the hexadecimal string
    while (hexString[i] != L'\0')
    {
        // Get the first hex digit
        int high = HexCharToInt(hexString[i]);
        if (high == -1)
            break; // Invalid character, stop processing
        
        i++;
        
        // Check if there's a second digit
        if (hexString[i] == L'\0')
            break; // Odd number of hex digits, stop
        
        // Get the second hex digit
        int low = HexCharToInt(hexString[i]);
        if (low == -1)
            break; // Invalid character, stop processing
        
        i++;
        
        // Combine the two hex digits into a byte
        unsigned char byte = static_cast<unsigned char>((high << 4) | low);
        buffer[bufferIndex++] = byte;
        
        // If buffer is full, write it to the file
        if (bufferIndex == 4096)
        {
            outFile.write(reinterpret_cast<const char*>(buffer), bufferIndex);
            totalBytesWritten += bufferIndex;
            bufferIndex = 0;
        }
    }
    
    // Write any remaining data in the buffer
    if (bufferIndex > 0)
    {
        outFile.write(reinterpret_cast<const char*>(buffer), bufferIndex);
        totalBytesWritten += bufferIndex;
    }
    
    // Clean up
    outFile.close();
    GlobalUnlock(hData);
    CloseClipboard();
    
    // Print results
    std::cout << totalBytesWritten << " bytes written to " << filename << "\n";
    std::cout << "Enter any number to exit this program.\n";
    
    int dummy;
    std::cin >> dummy;
    
    return 0;
}