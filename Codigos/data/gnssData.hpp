#ifndef GNSS_DATA_HPP
#define GNSS_DATA_HPP

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <sstream> // Para std::istringstream
#include <iomanip>
#include <iostream> // Para std::cout
#include <algorithm> // Para std::nth_element e std::accumulate
#include "llaFromEcef.hpp"

/**
 * @brief Struct to hold GNSS data.
 */
struct GnssData {
    std::vector<double> time; ///< Time in seconds (GPST)
    std::vector<double> x; ///< X coordinate in meters (ECEF)
    std::vector<double> y; ///< Y coordinate in meters (ECEF)
    std::vector<double> z; ///< Z coordinate in meters (ECEF)
    std::vector<double> lat; ///< Latitude in degrees
    std::vector<double> lon; ///< Longitude in degrees
    std::vector<double> alt; ///< Altitude in meters
    std::vector<int> fix; ///< Fix status (1=fix, 2=float)
};

/**
 * @brief Loads GNSS data from a file.
 * 
 * @param fileName The name of the file to load data from.
 * @param logData If true, the function will log the GNSS data.
 * @return GnssData The loaded GNSS data.
 * @throws std::runtime_error If there is an error reading the file.
 */
GnssData loadGnssData(const std::string& fileName, bool logData);

/**
 * @brief Outputs GNSS data to a file.
 * 
 * @param gnssData The GNSS data to output.
 * @param outputFileName The name of the file to output data to.
 * @param initialIndex The initial index of the data to output.
 * @param finalIndex The final index of the data to output.
 * @throws std::runtime_error If there is an error opening the output file.
 */
void outputGnss(const GnssData& gnssData, const std::string& outputFileName, size_t initialIndex, size_t finalIndex);

std::string getLogStream(const GnssData &gnssData);

/**
 * @brief Logs GNSS data to the console.
 * 
 * @param gnssData The GNSS data to log.
 */
void logGnss(const GnssData& gnssData);

#endif // GNSS_DATA_HPP