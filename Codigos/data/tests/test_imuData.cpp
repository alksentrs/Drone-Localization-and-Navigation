#include <iostream>
#include <stdexcept>
#include "imuData.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.bin>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + ".txt";
    // Remove the path from the output file name
    outputFileName = outputFileName.substr(outputFileName.find_last_of('\\') + 1);

    try {
        int model = 0;
        ImuData imuData = loadImuData(inputFileName, model, true);
        
        outputImuData(imuData, outputFileName, 100000, 100100);
        std::cout << "IMU data has been written to " << outputFileName << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}