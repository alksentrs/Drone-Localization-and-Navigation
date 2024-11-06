% Example usage of DataWaiter class

% Define filenames and data types
filenames = {'imu_logger_dat-2024-08-07-02-32-42.bin', 'gnss_logger_dat-2024-08-07-02-32-42.pos'};
dataTypes = [1, 2];  % 1=IMU, 2=GNSS

% Create DataWaiter object
dw = DataWaiter(filenames, dataTypes);

% Load data from files
dw = dw.loadData();

% Build the sorted queue
dw = dw.buildQueue();

% Get the first 270000 oldest measurements (skip gnss gap)
[measurements, types, dw] = dw.popSorted(270000);

% Display the next oldest measurements
for i = 1:120
    [measurements, types, dw] = dw.popSorted(1);
    if types == 1 && mod(i, 10) == 0
        fprintf('IMU: dataPtr = %d, timestamp = %.3f\n', dw.dataPtr(1) - 1, measurements{1}(1));
    elseif types == 2
        fprintf('GNSS: dataPtr = %d, timestamp = %.3f\n', dw.dataPtr(2) - 1, measurements{1}(1));
    end
end