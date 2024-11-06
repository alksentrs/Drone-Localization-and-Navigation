function gnssData = loadGnssData_mex(gnssFile)
%LOADGNSSDATA_MEX Load GNSS data from a file using a MEX function.
%
%   GNSSDATA = LOADGNSSDATA_MEX(GNSSFILE) reads GNSS data from the file
%   specified by GNSSFILE and returns it as a matrix.
%
%   INPUT:
%       gnssFile - String specifying the path to the GNSS data file.
%
%   OUTPUT:
%       gnssData - Matrix containing the GNSS data with the following columns:
%           [Time, X, Y, Z, Latitude, Longitude, Altitude, FixStatus]
%
%   Example:
%       data = loadGnssData_mex('path/to/gnssData.pos');

gnssData = loadGnssData_mexbin(gnssFile);
end