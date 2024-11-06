#include "imuData.hpp"

ImuData loadImuData(const std::string &fileName, int &imuModel, bool logData)
{
    // Load Raw Data
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file into a buffer
    std::vector<uint32_t> buffer(size / sizeof(uint32_t));
    if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    {
        throw std::runtime_error("Error reading file");
    }

    // Determine the number of samples
    size_t numSamples = buffer.size() / 8;
    std::vector<std::vector<uint32_t>> A(numSamples, std::vector<uint32_t>(8));
    for (size_t i = 0; i < numSamples; ++i)
    {
        std::copy(buffer.begin() + i * 8, buffer.begin() + (i + 1) * 8, A[i].begin());
    }

    bool imuOK = false;
    double anglfak, accelfak;

    // Initialize vectors to store IMU data
    std::vector<double> timeStamp(numSamples);
    std::vector<double> gx(numSamples), gy(numSamples), gz(numSamples);
    std::vector<double> accx(numSamples), accy(numSamples), accz(numSamples);

    // Iterate over possible IMU models
    for (int k : {16495, 16490})
    {
        if (imuModel != 0)
        {
            k = imuModel;
        }

        // Set scaling factors based on IMU model
        if (k == 16495)
        {
            anglfak = 6.25e-3 / (1 << 16);
            accelfak = 2.5e-4 / (1 << 16);
        }
        else if (k == 16490)
        {
            anglfak = 5e-3 / (1 << 16);
            accelfak = 5e-4 / (1 << 16);
        }
        else
        {
            std::cerr << "Modelo não implementado." << std::endl;
            continue;
        }

        // Process each sample
        for (size_t i = 0; i < numSamples; ++i)
        {
            uint64_t t = (static_cast<uint64_t>(A[i][1]) << 32) | A[i][0];
            timeStamp[i] = static_cast<double>(t) / 1e9;
            gx[i] = static_cast<double>(static_cast<int32_t>(A[i][2])) * anglfak;
            gy[i] = static_cast<double>(static_cast<int32_t>(A[i][3])) * anglfak;
            gz[i] = static_cast<double>(static_cast<int32_t>(A[i][4])) * anglfak;
            accx[i] = static_cast<double>(static_cast<int32_t>(A[i][5])) * accelfak;
            accy[i] = static_cast<double>(static_cast<int32_t>(A[i][6])) * accelfak;
            accz[i] = static_cast<double>(static_cast<int32_t>(A[i][7])) * accelfak;
        }

        // Check if the accelerometer data corresponds to gravity
        std::vector<double> acc(10);
        for (int i = 0; i < 10; ++i)
        {
            acc[i] = std::sqrt(accx[i] * accx[i] + accy[i] * accy[i] + accz[i] * accz[i]);
        }
        double g0 = std::accumulate(acc.begin(), acc.end(), 0.0) / acc.size();
        if (std::abs(g0 - 1) < 0.05)
        {
            // If the static acceleration is close to 1g, set the IMU model and exit the loop
            imuOK = true;
            imuModel = k;
            break;
        }
        else if (imuModel)
        {
            break;
        }
    }

    // Throw an error if IMU data is not valid (i.e., the accelerometer data does not correspond to gravity)
    if (!imuOK)
    {
        throw std::runtime_error("Escala dos acelerômetros da IMU não correspondem a gravidade.");
    }

    // Create and populate the ImuData object
    ImuData imuData;
    imuData.timeStamp = timeStamp;
    imuData.accx = accx;
    imuData.accy = accy;
    imuData.accz = accz;
    imuData.gx = gx;
    imuData.gy = gy;
    imuData.gz = gz;

    removeDuplicateTimestamps(imuData);

    // Log IMU data if requested
    if (logData)
    {
        logImuData(imuData, imuModel);
    }

    return imuData;
}

void removeDuplicateTimestamps(ImuData& imuData) {
    // Find unique timestamps and their corresponding indices
    std::vector<size_t> uniqueIndices;
    uniqueIndices.reserve(imuData.timeStamp.size());

    auto last = std::unique(imuData.timeStamp.begin(), imuData.timeStamp.end());
    for (auto it = imuData.timeStamp.begin(); it != last; ++it) {
        uniqueIndices.push_back(std::distance(imuData.timeStamp.begin(), it));
    }

    // Create new vectors with only unique elements
    std::vector<double> newTimeStamp, newaccx, newaccy, newaccz, newgx, newgy, newgz;
    newTimeStamp.reserve(uniqueIndices.size());
    newaccx.reserve(uniqueIndices.size());
    newaccy.reserve(uniqueIndices.size());
    newaccz.reserve(uniqueIndices.size());
    newgx.reserve(uniqueIndices.size());
    newgy.reserve(uniqueIndices.size());
    newgz.reserve(uniqueIndices.size());

    for (size_t idx : uniqueIndices) {
        newTimeStamp.push_back(imuData.timeStamp[idx]);
        newaccx.push_back(imuData.accx[idx]);
        newaccy.push_back(imuData.accy[idx]);
        newaccz.push_back(imuData.accz[idx]);
        newgx.push_back(imuData.gx[idx]);
        newgy.push_back(imuData.gy[idx]);
        newgz.push_back(imuData.gz[idx]);
    }

    // Replace old vectors with new vectors
    imuData.timeStamp = std::move(newTimeStamp);
    imuData.accx = std::move(newaccx);
    imuData.accy = std::move(newaccy);
    imuData.accz = std::move(newaccz);
    imuData.gx = std::move(newgx);
    imuData.gy = std::move(newgy);
    imuData.gz = std::move(newgz);
}

void outputImuData(const ImuData& imuData, const std::string& outputFileName, size_t initialIndex, size_t finalIndex) {
    // Open output file
    std::ofstream outFile(outputFileName);
    if (!outFile) {
        throw std::runtime_error("Error opening output file");
    }

    outFile << std::fixed << std::setprecision(9);
    outFile << "TimeStamp\tAccX\tAccY\tAccZ\tGx\tGy\tGz\n";
    for (size_t i = initialIndex; i < finalIndex; ++i) {
        outFile << imuData.timeStamp[i] << "\t"
                << imuData.accx[i] << "\t"
                << imuData.accy[i] << "\t"
                << imuData.accz[i] << "\t"
                << imuData.gx[i] << "\t"
                << imuData.gy[i] << "\t"
                << imuData.gz[i] << "\n";
    }
}

void logImuData(const ImuData &imuData, int imuModel) {
    // Obter a string de log
    std::string logStr = getLogStream(imuData, imuModel);

    // Exibir a string usando std::cout
    std::cout << logStr;
}

// Função getLogStream
std::string getLogStream(const ImuData &imuData, int imuModel) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (!imuData.timeStamp.empty()) {
        size_t imuSamples = imuData.timeStamp.size();

        // Verificar se há dados suficientes para calcular diferenças
        if (imuSamples < 2) {
            oss << "Not enough IMU data to calculate statistics.\n";
            return oss.str();
        }

        // Calcular diferenças de tempo
        std::vector<double> imuDiffTime(imuSamples - 1);
        std::adjacent_difference(imuData.timeStamp.begin(), imuData.timeStamp.end(), imuDiffTime.begin());
        imuDiffTime.erase(imuDiffTime.begin()); // Remover o primeiro elemento inválido

        // Calcular mediana das diferenças de tempo
        std::nth_element(imuDiffTime.begin(), imuDiffTime.begin() + imuDiffTime.size() / 2, imuDiffTime.end());
        double medianDiffTime = imuDiffTime[imuDiffTime.size() / 2];

        // Calcular frequência
        double imuFreq = 1.0 / medianDiffTime;

        // Calcular gaps
        int imuGaps = std::count_if(imuDiffTime.begin(), imuDiffTime.end(), [medianDiffTime](double dt) {
            return std::abs(dt - medianDiffTime) > medianDiffTime * 0.1;
        });

        // Gerar a string de log
        oss << "IMU Model: " << imuModel << "\n";
        oss << "IMU Samples: " << imuSamples << " (" << (imuData.timeStamp.back() - imuData.timeStamp.front()) / 60.0 << " minutes)\n";
        oss << "IMU Freq: " << imuFreq << " Hz\n";
        oss << "IMU Gaps: " << imuGaps << " (" << (static_cast<double>(imuGaps) / imuSamples * 100.0) << "%)\n\n";
    } else {
        oss << "No IMU data available.\n";
    }

    return oss.str();
}