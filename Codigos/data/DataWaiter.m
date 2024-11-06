classdef DataWaiter
    %DATAWAITER Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        filenames cell      % Cell array of strings
        dataTypes double    % Array of integers (1=IMU, 2=GNSS, -1=no data)
        data cell           % Cell array to store data matrices
        dataPtr double      % Array of current positions for each sensor
        sortedQueue double  % Nx2 matrix of [type, index] pairs
        queuePtr double     % Current position in sorted queue
    end
    
    methods
        function obj = DataWaiter(filenames, dataTypes)
            %DATAWAITER Construct an instance of this class
            %   OBJ = DATAWAITER(FILENAMES, DATATYPES) initializes the
            %   DataWaiter object with the given filenames and data types.
            %
            %   INPUT:
            %   - FILENAMES: Cell array of strings specifying the paths to
            %     the data files.
            %   - DATATYPES: Array of integers specifying the type of each
            %     data file (1=IMU, 2=GNSS).
            %
            %   OUTPUT:
            %   - OBJ: Initialized DataWaiter object.
            
            % Validate inputs
            assert(length(filenames) == length(dataTypes), ...
                'Number of filenames must match number of data types');
            
            obj.filenames = filenames;
            obj.dataTypes = dataTypes;
            obj.data = cell(1, length(filenames));
            obj.dataPtr = ones(1, length(filenames)); % Initialize pointers to 1
        end
        
        function obj = loadData(obj)
            %LOADDATA Load data from files
            %   OBJ = LOADDATA(OBJ) loads the data from the files specified
            %   in the filenames property and stores them in the data
            %   property.
            %
            %   OUTPUT:
            %   - OBJ: DataWaiter object with loaded data.
            
            for i = 1:length(obj.filenames)
                switch obj.dataTypes(i)
                    case 1 % IMU data
                        obj.data{i} = loadImuData_mex(obj.filenames{i});
                    case 2 % GNSS data
                        obj.data{i} = loadGnssData_mex(obj.filenames{i});
                    otherwise
                        error('Unknown data type: %d', obj.dataTypes(i));
                end
            end
        end
        
        function [measurement, sensorType, obj] = popNext(obj)
            %POPNEXT Return the oldest measurement and its type
            %   [MEASUREMENT, SENSORTYPE, OBJ] = POPNEXT(OBJ) returns the
            %   oldest measurement based on the timestamp and its type.
            %   The data is not removed, but the pointer for the respective
            %   sensor is incremented.
            %
            %   OUTPUT:
            %   - MEASUREMENT: The oldest measurement data.
            %   - SENSORTYPE: The type of the sensor (1=IMU, 2=GNSS). Or -1 if no data.
            %   - OBJ: Updated DataWaiter object.
            
            % Check if streaming should be stopped
            if obj.shouldStop()
                measurement = [];
                sensorType = -1;
                return;
            end
            
            % Initialize variables to track minimum timestamp
            minTime = inf;
            minIdx = 0;
            
            % Find oldest measurement across all sensors
            for i = 1:length(obj.data)
                % Check if we still have data to read from this sensor
                if obj.dataPtr(i) <= size(obj.data{i}, 1)
                    % Get timestamp from current position
                    currentTime = obj.data{i}(obj.dataPtr(i), 1);
                    
                    % Update if this is the oldest timestamp seen
                    if currentTime < minTime
                        minTime = currentTime;
                        minIdx = i;
                    end
                end
            end
            
            % Check if we found any valid measurement
            if minIdx == 0
                measurement = [];
                sensorType = -1;
                return;
            end
            
            % Get the measurement data
            measurement = obj.data{minIdx}(obj.dataPtr(minIdx), :);
            sensorType = obj.dataTypes(minIdx);
            
            % Increment the pointer for the sensor we just read
            obj.dataPtr(minIdx) = obj.dataPtr(minIdx) + 1;
        end
        
        function [measurements, sensorTypes, obj] = popSorted(obj, k)
            %POPSORTED Return the oldest k measurements and their types
            %   [MEASUREMENTS, SENSORTYPES, OBJ] = POPSORTED(OBJ, K)
            %   returns the oldest K measurements based on the timestamp
            %   and their types. The data is not removed, but the pointers
            %   for the respective sensors are incremented.
            %
            %   INPUT:
            %   - K: Number of measurements to return (default is 1).
            %
            %   OUTPUT:
            %   - MEASUREMENTS: Cell array of the oldest K measurement data.
            %   - SENSORTYPES: Array of the types of the sensors (1=IMU, 2=GNSS).
            %   - OBJ: Updated DataWaiter object.
            
            % Default to single measurement if k not specified
            if nargin < 2
                k = 1;
            end
            
            % Check if streaming should be stopped
            if obj.shouldStop()
                measurements = {};
                sensorTypes = -1;
                return;
            end
            
            % Initialize return arrays
            measurements = cell(1, k);
            sensorTypes = zeros(1, k);
            
            % Limit k to remaining measurements
            remainingMeasurements = size(obj.sortedQueue, 1) - obj.queuePtr + 1;
            k = min(k, remainingMeasurements);
            
            % Return empty if no measurements available
            if k <= 0
                measurements = {};
                sensorTypes = [];
                return;
            end
            
            % Get k measurements
            for i = 1:k
                % Get type and index from queue
                sensorTypes(i) = obj.sortedQueue(obj.queuePtr, 1);
                dataIndex = obj.sortedQueue(obj.queuePtr, 2);
                
                % Find sensor index from type
                sensorIdx = find(obj.dataTypes == sensorTypes(i), 1);
                
                % Get measurement
                measurements{i} = obj.data{sensorIdx}(dataIndex, :);
                
                % Increment both pointers
                obj.queuePtr = obj.queuePtr + 1;
                obj.dataPtr(sensorIdx) = obj.dataPtr(sensorIdx) + 1;
            end
        end
        
        function obj = buildQueue(obj)
            %BUILDQUEUE Build a sorted queue of measurements
            %   OBJ = BUILDQUEUE(OBJ) builds a sorted queue of all
            %   measurements based on their timestamps. The queue is stored
            %   in the sortedQueue property.
            %
            %   OUTPUT:
            %   - OBJ: DataWaiter object with built queue.
            
            % Initialize variables to store all timestamps and corresponding info
            totalSize = 0;
            for i = 1:length(obj.data)
                totalSize = totalSize + size(obj.data{i}, 1);
            end
            
            % Pre-allocate arrays
            allTimestamps = zeros(totalSize, 1);
            typeIndexPairs = zeros(totalSize, 2);
            
            % Fill arrays with data
            currentIndex = 1;
            for i = 1:length(obj.data)
                n = size(obj.data{i}, 1);
                indices = currentIndex:(currentIndex + n - 1);
                
                % Get timestamps
                allTimestamps(indices) = obj.data{i}(:, 1);
                
                % Store sensor type and original index
                typeIndexPairs(indices, :) = [repmat(obj.dataTypes(i), n, 1), (1:n)'];
                
                currentIndex = currentIndex + n;
            end
            
            % Sort based on timestamps
            [~, sortIdx] = sort(allTimestamps);
            
            % Store sorted type-index pairs
            obj.sortedQueue = typeIndexPairs(sortIdx, :);
            obj.queuePtr = 1;  % Initialize queue pointer
        end
        
        function stop = shouldStop(~)
            %SHOULDSTOP Dummy method to determine if streaming should stop
            %   STOP = SHOULDSTOP() returns true if streaming should stop.
            %
            %   OUTPUT:
            %   - STOP: Logical value indicating if streaming should stop.
            
            stop = true; % Dummy implementation, always returns true
        end
    end
end
