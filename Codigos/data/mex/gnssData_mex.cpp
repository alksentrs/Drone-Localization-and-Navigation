#include "mex.h"
#include "gnssData.hpp"
#include <string>
#include <stdexcept>

// Declaração da função logGnssMex
void logGnssMex(const GnssData &gnssData);

// Implementação da função logGnssMex
void logGnssMex(const GnssData &gnssData) {
    // Obter a string de log
    std::string logStr = getLogStream(gnssData);

    // Exibir a string usando mexPrintf
    mexPrintf("%s", logStr.c_str());
}

// mexFunction é o ponto de entrada da função MEX
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    // Verificar número de argumentos de entrada
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("gnssData_mex:nrhs", "Um argumento de entrada é necessário.");
    }

    // Verificar número de argumentos de saída
    if (nlhs > 1) {
        mexErrMsgIdAndTxt("gnssData_mex:nlhs", "Apenas um argumento de saída é permitido.");
    }

    // Verificar se a entrada é uma string
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("gnssData_mex:inputNotString", "O argumento de entrada deve ser uma string (nome do arquivo).");
    }

    // Obter o nome do arquivo
    char* filename_c = mxArrayToString(prhs[0]);
    if (filename_c == nullptr) {
        mexErrMsgIdAndTxt("gnssData_mex:conversionFailed", "Falha ao converter o nome do arquivo.");
    }
    std::string filename(filename_c);
    mxFree(filename_c);

    // Carregar os dados GNSS
    GnssData gnssData;
    try {
        gnssData = loadGnssData(filename, false);
        logGnssMex(gnssData);
    } catch (const std::exception& e) {
        mexErrMsgIdAndTxt("gnssData_mex:runtimeError", e.what());
    }

    // Número de amostras
    size_t numSamples = gnssData.time.size();

    // Verificar se os vetores têm o mesmo tamanho
    if (numSamples == 0 ||
        numSamples != gnssData.x.size() ||
        numSamples != gnssData.y.size() ||
        numSamples != gnssData.z.size() ||
        numSamples != gnssData.lat.size() ||
        numSamples != gnssData.lon.size() ||
        numSamples != gnssData.alt.size() ||
        numSamples != gnssData.fix.size()) {
        mexErrMsgIdAndTxt("gnssData_mex:sizeMismatch", "Tamanhos dos vetores não correspondem.");
    }

    // Criar a matriz de saída com 8 colunas
    // Colunas: time, x, y, z, lat, lon, alt, fix
    mwSize dims[2] = { static_cast<mwSize>(numSamples), 8 };
    plhs[0] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
    double* outData = mxGetPr(plhs[0]);

    // Copiar dados para a matriz de saída
    for (size_t i = 0; i < numSamples; ++i) {
        outData[i] = gnssData.time[i];
        outData[i + numSamples] = gnssData.x[i];
        outData[i + 2 * numSamples] = gnssData.y[i];
        outData[i + 3 * numSamples] = gnssData.z[i];
        outData[i + 4 * numSamples] = gnssData.lat[i];
        outData[i + 5 * numSamples] = gnssData.lon[i];
        outData[i + 6 * numSamples] = gnssData.alt[i];
        outData[i + 7 * numSamples] = static_cast<double>(gnssData.fix[i]);
    }
}