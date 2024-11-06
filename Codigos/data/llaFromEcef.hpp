#ifndef LLA_FROM_ECEF_HPP
#define LLA_FROM_ECEF_HPP

#include <vector>
#include <cmath>
#include <stdexcept>

// Definir M_PI se não estiver definida
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Converts ECEF coordinates to geodetic coordinates.
 * 
 * @param x The X coordinates in meters (ECEF).
 * @param y The Y coordinates in meters (ECEF).
 * @param z The Z coordinates in meters (ECEF).
 * @param lat The latitude in degrees.
 * @param lon The longitude in degrees.
 * @param alt The altitude in meters.
 */
inline void llaFromEcef(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                        std::vector<double>& lat, std::vector<double>& lon, std::vector<double>& alt) {
    const double a = 6.378137e6; // Semi-major axis
    const double e = 0.0818191908425; // Eccentricity
    const double l = e * e / 2;
    const double Hmin = std::pow(e, 12) / 4;

    size_t nPoints = x.size();
    lat.resize(nPoints);
    lon.resize(nPoints);
    alt.resize(nPoints);

    // Set precision to 9 decimal places and scientific notation
    std::cout << std::fixed << std::setprecision(18);

    for (size_t j = 0; j < nPoints; ++j) {
        double w2 = x[j] * x[j] + y[j] * y[j];
        double m = w2 / (a * a);
        double n = z[j] * z[j] * (1 - e * e) / (a * a);
        double p = (m + n - 4 * l * l) / 6;
        double G = m * n * l * l;
        double H = 2 * p * p * p + G;

        if (H < Hmin) {
            throw std::runtime_error("H < Hmin.. not feasible");
        }

        double C = std::pow((H + G + 2 * std::sqrt(H * G)), 1.0 / 3) / std::pow(2, 1.0 / 3);
        double i = -(2 * l * l + m + n) / 2;
        double P = p * p;
        double beta = i / 3 - C - P / C;
        double k = l * l * (l * l - m - n);
        double t = std::sqrt(std::sqrt(beta * beta - k) - (beta + i) / 2) - std::copysign(std::sqrt(std::abs((beta - i) / 2)), m - n);
        double F = t * t * t * t + 2 * i * t * t + 2 * l * (m - n) * t + k;
        double dF = 4 * t * t * t + 4 * i * t + 2 * l * (m - n);
        double dt = -F / dF;
        double u = t + dt + l;
        double v = t + dt - l;
        double w = std::sqrt(w2);
        double latRad = std::atan2(z[j] * u, w * v);

        double dw = w * (1 - 1 / u);
        // This causes floating point problem as z is very large and the other terms are very small
        double dz = z[j] - z[j] * ((1 - e * e)/v);
        // Output
        // std::cout << v << std::endl;
        // std::cout << (v - (1 - e * e)) / v << std::endl;

        alt[j] = std::copysign(std::sqrt(dw * dw + dz * dz), u - 1);
        double lonRad = std::atan2(y[j], x[j]);
        lat[j] = latRad * 180.0 / M_PI;
        lon[j] = lonRad * 180.0 / M_PI;
    }
}

#endif // LLA_FROM_ECEF_HPP