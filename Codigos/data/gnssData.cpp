#include "gnssData.hpp"


GnssData loadGnssData(const std::string& fileName, bool logData) {
    // Open the file for reading
    std::ifstream file(fileName);
    if (!file) {
        throw std::runtime_error("Error opening file: " + fileName);
    }

    // Temporary vectors to store the read data
    std::vector<double> time, x, y, z;
    std::vector<int> fix;

    std::cout << "Loading GNSS data from " << fileName << "...\n";

    // Read the data from the file
    std::string line;
    while (std::getline(file, line)) {
        // Skip lines that start with '%' or are empty
        if (line.empty() || line[0] == '%') {
            continue;
        }

        std::istringstream iss(line);
        double t, xVal, yVal, zVal;
        int fixVal;
        std::string temp;

        // Read the first column (GPST) and ignore it
        iss >> temp;

        // Read the relevant columns
        if (iss >> t >> xVal >> yVal >> zVal >> fixVal) {
            time.push_back(t);
            x.push_back(xVal);
            y.push_back(yVal);
            z.push_back(zVal);
            fix.push_back(fixVal);
        } else {
            // If the line does not have the correct number of fields, skip it
            continue;
        }
    }

    // Close the file
    file.close();

    // Vectors to store geodetic coordinates
    std::vector<double> lat, lon, alt;

    // Convert ECEF coordinates to geodetic
    llaFromEcef(x, y, z, lat, lon, alt);

    // Create and fill the GnssData object
    GnssData gnssData;
    gnssData.time = time;
    gnssData.x = x;
    gnssData.y = y;
    gnssData.z = z;
    gnssData.lat = lat;
    gnssData.lon = lon;
    gnssData.alt = alt;
    gnssData.fix = fix;

    // Log the GNSS data if requested
    if (logData) {
        logGnss(gnssData);
    }

    std::cout << std::flush;

    return gnssData;
}

void logGnss(const GnssData& gnssData) {
    // Obter a string de log
    std::string logStr = getLogStream(gnssData);

    // Exibir a string usando std::cout
    std::cout << logStr;
}


// Função getLogStream
std::string getLogStream(const GnssData &gnssData) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (!gnssData.time.empty()) {
        size_t gnssSamples = gnssData.time.size();

        // Verificar se há dados suficientes para calcular diferenças
        if (gnssSamples < 2) {
            oss << "Not enough GNSS data to calculate statistics.\n";
            return oss.str();
        }

        // Calcular diferenças de tempo
        std::vector<double> gnssDiffTime(gnssSamples);
        std::adjacent_difference(gnssData.time.begin(), gnssData.time.end(), gnssDiffTime.begin());
        gnssDiffTime.erase(gnssDiffTime.begin()); // Remover o primeiro elemento inválido

        // Calcular mediana das diferenças de tempo
        std::nth_element(gnssDiffTime.begin(), gnssDiffTime.begin() + gnssDiffTime.size() / 2, gnssDiffTime.end());
        double medianDiffTime = gnssDiffTime[gnssDiffTime.size() / 2];

        // Calcular frequência
        double gnssFreq = 1.0 / medianDiffTime;

        // Calcular gaps
        int gnssGaps = std::count_if(gnssDiffTime.begin(), gnssDiffTime.end(), [medianDiffTime](double dt) {
            return std::abs(dt - medianDiffTime) > medianDiffTime * 0.1;
        });

        // Gerar a string de log
        oss << "GNSS Samples: " << gnssSamples << " (" << (gnssData.time.back() - gnssData.time.front()) / 60.0 << " minutes)\n";
        oss << "GNSS Freq: " << gnssFreq << " Hz\n";
        oss << "GNSS Gaps: " << gnssGaps << " (" << (static_cast<double>(gnssGaps) / gnssSamples * 100.0) << "%)\n";
        oss << "GNSS Quality: " << std::count(gnssData.fix.begin(), gnssData.fix.end(), 1) / static_cast<double>(gnssSamples) * 100.0 << "%\n\n";
    } else {
        oss << "No GNSS data available.\n";
    }

    return oss.str();
}

void outputGnss(const GnssData& gnssData, const std::string& outputFileName, size_t initialIndex, size_t finalIndex) {
    // Check if indices are valid
    if (initialIndex >= finalIndex || finalIndex > gnssData.time.size()) {
        throw std::invalid_argument("Invalid indices.");
    }

    // Open the file for writing
    std::ofstream outFile(outputFileName);
    if (!outFile) {
        throw std::runtime_error("Error opening output file: " + outputFileName);
    }

    std::cout << "Writing GNSS data to " << outputFileName << "...\n";

    // Write the header
    outFile << std::fixed << std::setprecision(12);
    outFile << "Time\tX\tY\tZ\tLat\tLon\tAlt\tFix\n";

    // Write the data
    for (size_t i = initialIndex; i < finalIndex; ++i) {
        outFile << gnssData.time[i] << "\t"
                << gnssData.x[i] << "\t"
                << gnssData.y[i] << "\t"
                << gnssData.z[i] << "\t"
                << gnssData.lat[i] << "\t"
                << gnssData.lon[i] << "\t"
                << gnssData.alt[i] << "\t"
                << gnssData.fix[i] << "\n";
    }

    // Close the file
    outFile.close();
}