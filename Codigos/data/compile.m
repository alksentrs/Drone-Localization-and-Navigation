% setenv('MW_MINGW64_LOC','C:\msys64\ucrt64')
% %%
% 
% mex -setup:'C:\Program Files\MATLAB\R2022a\bin\win64\mexopts\mingw64_g++.xml' C++
% %%
% ipath = ['-I' pwd];
% % Compilar ImuData
% mex('-v', 'CXXFLAGS="$CXXFLAGS -Wall -Wextra"', ipath, '-R2018a', ...
%     '-output', 'mex/loadImuData_mexbin', ...
%     'imuData.cpp', 'mex/imuData_mex.cpp')
% 
% % Compilar GnssData
% mex('-v', 'CXXFLAGS="$CXXFLAGS -Wall -Wextra"', ipath, '-R2018a', ...
%     '-output', 'mex/loadGnssData_mexbin', ...
%     'gnssData.cpp', 'mex/gnssData_mex.cpp')
% 
% %%
% 

ipath = ['-I' pwd];
mex(ipath,'-v','-output', 'mex/loadImuData_mexbin', 'imuData.cpp', 'mex/imuData_mex.cpp')