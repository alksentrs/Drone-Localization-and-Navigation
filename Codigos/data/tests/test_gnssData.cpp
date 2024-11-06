#include <iostream>
#include <stdexcept>
#include "gnssData.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.pos>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + ".txt";
    // Remove the path from the output file name
    outputFileName = outputFileName.substr(outputFileName.find_last_of('\\') + 1);

    try {
        GnssData gnssData = loadGnssData(inputFileName, true);
        std::cout << "GNSS data loaded successfully." << std::endl;

        outputGnss(gnssData, outputFileName, 1100, 1140);
        std::cout << "GNSS data has been written to " << outputFileName << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}