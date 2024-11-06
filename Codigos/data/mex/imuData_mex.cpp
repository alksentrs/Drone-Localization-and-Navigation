#include "mex.h"
#include "imuData.hpp"
#include <string>
#include <stdexcept>

// Declaração da função logImuMex
void logImuMex(const ImuData &imuData, int imuModel);

// Implementação da função logImuMex
void logImuMex(const ImuData &imuData, int imuModel) {
    // Obter a string de log
    std::string logStr = getLogStream(imuData, imuModel);

    // Exibir a string usando mexPrintf
    mexPrintf("%s", logStr.c_str());
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    // Verificar número de argumentos de entrada
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("imuData_mex:nrhs", "Um argumento de entrada é necessário.");
    }

    // Verificar número de argumentos de saída
    if (nlhs > 1) {
        mexErrMsgIdAndTxt("imuData_mex:nlhs", "Apenas um argumento de saída é permitido.");
    }

    // Verificar se a entrada é uma string
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("imuData_mex:inputNotString", "O argumento de entrada deve ser uma string (nome do arquivo).");
    }

    // Obter o nome do arquivo
    char* filename_c = mxArrayToString(prhs[0]);
    if (filename_c == nullptr) {
        mexErrMsgIdAndTxt("imuData_mex:conversionFailed", "Falha ao converter o nome do arquivo.");
    }
    std::string filename(filename_c);
    mxFree(filename_c);

    // Carregar os dados IMU
    int imuModel = 0;
    ImuData imuData;
    try {
        imuData = loadImuData(filename, imuModel, false);
        logImuMex(imuData, imuModel);
    } catch (const std::exception& e) {
        mexErrMsgIdAndTxt("imuData_mex:runtimeError", e.what());
    }

    // Número de amostras
    size_t numSamples = imuData.timeStamp.size();

    mexPrintf("Número de amostras: %d\n", numSamples);
    mexPrintf("Sizes: %d %d %d %d %d %d\n", imuData.accx.size(), imuData.accy.size(), imuData.accz.size(), imuData.gx.size(), imuData.gy.size(), imuData.gz.size());

    // Verificar se os vetores têm o mesmo tamanho
    if (numSamples == 0 ||
        numSamples != imuData.accx.size() ||
        numSamples != imuData.accy.size() ||
        numSamples != imuData.accz.size() ||
        numSamples != imuData.gx.size() ||
        numSamples != imuData.gy.size() ||
        numSamples != imuData.gz.size()) {
        mexErrMsgIdAndTxt("imuData_mex:sizeMismatch", "Tamanhos dos vetores não correspondem.");
    }

    // Criar a matriz de saída com 7 colunas
    mwSize dims[2] = { static_cast<mwSize>(numSamples), 7 };
    plhs[0] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
    double* outData = mxGetPr(plhs[0]);

    // Copiar dados para a matriz de saída
    for (size_t i = 0; i < numSamples; ++i) {
        outData[i] = imuData.timeStamp[i];
        outData[i + numSamples] = imuData.accx[i];
        outData[i + 2 * numSamples] = imuData.accy[i];
        outData[i + 3 * numSamples] = imuData.accz[i];
        outData[i + 4 * numSamples] = imuData.gx[i];
        outData[i + 5 * numSamples] = imuData.gy[i];
        outData[i + 6 * numSamples] = imuData.gz[i];
    }
}