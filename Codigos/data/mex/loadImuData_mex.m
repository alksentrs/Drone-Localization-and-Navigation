function imuData = loadImuData_mex(imuFile)
%LOADIMUDATA_MEX Load IMU data from a binary file using a MEX function.
%
%   IMUDATA = LOADIMUDATA_MEX(IMUFILE) reads IMU data from the binary file
%   specified by IMUFILE and returns it as a matrix. This function serves
%   as a wrapper for the compiled MEX function implemented in C++.
%
%   INPUT:
%       imuFile - String specifying the path to the IMU binary file.
%
%   OUTPUT:
%       imuData - Matrix containing the IMU data with the following columns:
%           [Timestamp, accx, accy, accz, gx, gy, gz.] the unids are in Gs
%           and deg/s
%
%   The underlying MEX function 'loadImuData_mexbin' is compiled from C++
%   code for efficient data processing.
%
%   Example:
%       data = loadImuData_mex('path/to/imuData.bin');
%       timeStamp = data(:, 1);
%       accX = data(:, 2);
%       accY = data(:, 3);
%       accZ = data(:, 4);
%       gyroX = data(:, 5);
%       gyroY = data(:, 6);
%       gyroZ = data(:, 7);
%
%   See also: loadImuData_mexbin

imuData = loadImuData_mexbin(imuFile);
end