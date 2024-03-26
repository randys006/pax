#include "pax.h"

using namespace std;
using namespace sss::pax;

int main()
{
    // This is a subset of basicWriteReadModify from unittest1.cpp, useful for debugging.
    // Please keep them synchronized.
    vector<float> floatData{ 158.98166f, 171.61903f, 160.06989f, 148.83504f };
    floatRasterFile floatFile{ 2, 2, static_cast<void*>(floatData.data()) };
    float piVal = 3.1416f;
    floatFile.addMetaVal("pi", piVal);

    string floatFileName{ "basic_floatFile.pax" };
    paxBufPtr floatBuf;
    floatFile.writeToBuffer(floatBuf);
    auto floatFileSize = floatBuf->size();
    rasterFileBase::writeToFile(floatBuf, floatFileName);   // usually writes many extra digits

    paxBufPtr floatInBuf = rasterFileBase::readFile(floatFileName);
    auto floatInFileSize = floatInBuf->size();

    floatRasterFile floatInFile;
    auto importRet = floatInFile.import(floatInBuf);
    float inpi = floatInFile.getMetaFloat("pi");

    // tweak values and verify again
    double piPrecise = 3.1415926535897932;
    floatInFile.addMetaVal("pi", piPrecise);    // overwrites the old pi with double type

    string doubleFileName{ "basic_doubleFile.pax" };
    floatInFile.writeToFile(doubleFileName);
}
