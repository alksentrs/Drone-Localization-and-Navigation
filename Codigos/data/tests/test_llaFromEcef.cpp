#include <iostream>
#include <vector>
#include <iomanip>
#include "llaFromEcef.hpp"

int main() {
    // Conjunto de coordenadas XYZ
    std::vector<double> x = {3330604.0836, 3330604.1142};
    std::vector<double> y = {4774361.826, 4774361.8661};
    std::vector<double> z = {2597886.0697, 2597886.059};

    // Vetores para armazenar as coordenadas geodésicas resultantes
    std::vector<double> lat, lon, alt;

    // Converter as coordenadas ECEF para geodésicas
    llaFromEcef(x, y, z, lat, lon, alt);

    // Imprimir os resultados com precisão de 9 casas decimais
    std::cout << std::fixed << std::setprecision(16);
    for (size_t i = 0; i < lat.size(); ++i) {
        std::cout << "Point " << i + 1 << ":\n";
        std::cout << "Latitude: " << lat[i] << " degrees\n";
        std::cout << "Longitude: " << lon[i] << " degrees\n";
        std::cout << "Altitude: " << alt[i] << " meters\n";
        std::cout << std::endl;
    }

    return 0;
}