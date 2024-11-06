#ifndef IMU_DATA_HPP
#define IMU_DATA_HPP

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <cstdint>
#include <sstream>

/**
 * @brief Struct to hold IMU data.
 */
struct ImuData {
    std::vector<double> timeStamp; ///< Timestamps of the IMU data
    std::vector<double> accx; ///< Accelerometer data in X direction
    std::vector<double> accy; ///< Accelerometer data in Y direction
    std::vector<double> accz; ///< Accelerometer data in Z direction
    std::vector<double> gx; ///< Gyroscope data in X direction
    std::vector<double> gy; ///< Gyroscope data in Y direction
    std::vector<double> gz; ///< Gyroscope data in Z direction
};

/**
 * @brief Loads IMU data from a binary file.
 * 
 * @param fileName The name of the file to load data from.
 * @param imuModel The IMU model to use. If 0, the function will try to determine the model.
 * @param logData If true, the function will log the IMU data.
 * @return ImuData The loaded IMU data.
 * @throws std::runtime_error If there is an error reading the file or the IMU data is not valid.
 */
ImuData loadImuData(const std::string &fileName, int &imuModel, bool logData);

/**
 * @brief Removes lines with duplicate timestamps from IMU data.
 * 
 * @param imuData The IMU data to remove duplicate timestamps from.
 */
void removeDuplicateTimestamps(ImuData& imuData);

/**
 * @brief Logs IMU data to the console.
 * 
 * @param imuData The IMU data to log.
 * @param imuModel The IMU model used.
 */
void logImuData(const ImuData& imuData, int imuModel);

// Nova função getLogStream
std::string getLogStream(const ImuData &imuData, int imuModel);

/**
 * @brief Outputs IMU data to a file.
 * 
 * @param imuData The IMU data to output.
 * @param outputFileName The name of the file to output data to.
 * @param initialIndex The initial index of the data to output.
 * @param finalIndex The final index of the data to output.
 * @throws std::runtime_error If there is an error opening the output file.
 */
void outputImuData(const ImuData& imuData, const std::string& outputFileName, size_t initialIndex, size_t finalIndex);

#endif // IMU_DATA_HPP