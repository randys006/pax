// TODO: improve unit tests
// TODO: make unit tests cross-platform

#include "../../pax/pax.h"

#include <memory>

using namespace std;
using namespace sss::pax;

// MSVS-specific stuff
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<enum ValueSpace::value_space>(const enum ValueSpace::value_space& t) { return ToString((int32_t)t); }
            template<> static std::wstring ToString<enum ValueSpace::vpe>(const enum ValueSpace::vpe& t) { return ToString((int32_t)t); }
        }
    }
}

namespace paxmstest
{
    TEST_CLASS(PaxTests)
    {
    public:

        TEST_METHOD(basicWriteReadModify)
        {
            Logger::WriteMessage("Starting basicWriteReadModify");

            vector<float> floatData{ 158.98166f, 171.61903f, 160.06989f, 148.83504f };
            floatRasterFile floatFile{ 2, 2, static_cast<void*>(floatData.data()) };
            float piVal = 3.1416f;
            floatFile.addMetaVal("pi", piVal);
            Assert::AreEqual(piVal, floatFile.getMetaFloat("pi"));

            string floatFileName{ "basicRWM_floatFile.pax" };
            paxBufPtr floatBuf;
            floatFile.writeToBuffer(floatBuf);
            auto floatFileSize = floatBuf->size();
            rasterFileBase::writeToFile(floatBuf, floatFileName);   // usually writes many extra digits

            paxBufPtr floatInBuf = rasterFileBase::readFile(floatFileName);
            auto floatInFileSize = floatInBuf->size();

            Assert::AreEqual(floatFileSize, floatInFileSize);

            floatRasterFile floatInFile;
            auto importRet = floatInFile.import(floatInBuf);
            Assert::AreEqual(importRet, static_cast<int>(PAX_OK));

            // verify data was imported correctly
            Assert::AreEqual(158.98166f, floatInFile.floatValXY(0, 0));
            Assert::AreEqual(171.61903f, floatInFile.floatValXY(1, 0));
            Assert::AreEqual(160.06989f, floatInFile.floatValRC(1, 0));
            Assert::AreEqual(148.83504f, floatInFile.floatValRC(1, 1));
            Assert::AreEqual(piVal, floatInFile.getMetaFloat("pi"));

            // tweak values and verify again
            double piPrecise = 3.1415926535897932;
            floatInFile.addMetaVal("pi", piPrecise);                        // overwrites the old pi with double type
            Assert::AreNotEqual(piVal, floatInFile.getMetaFloat("pi"));     // getMetaFloat() returns garbage
            Assert::AreEqual(piPrecise, floatInFile.getMetaDouble("pi"));

            string doubleFileName{ "basicRWM_doubleFile.pax" };
            floatInFile.writeToFile(doubleFileName);
        }

        TEST_METHOD(bytesPerValue)
        {
            Logger::WriteMessage("Starting bytesPerValue");

            Assert::AreEqual(0, rasterFileBase::getBPV(paxTypes::ePAX_INVALID));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_SF_MAG_UCHAR));
            Assert::AreEqual(2, rasterFileBase::getBPV(paxTypes::ePAX_SF_MAG_PHASE_USHORT));
            Assert::AreEqual(2, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_USHORT));
            Assert::AreEqual(4, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_UINT));
            Assert::AreEqual(8, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_ULONG));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_SF_MAG_CHAR));
            Assert::AreEqual(2, rasterFileBase::getBPV(paxTypes::ePAX_SF_MAG_PHASE_SHORT));
            Assert::AreEqual(2, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_SHORT));
            Assert::AreEqual(4, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_INT));
            Assert::AreEqual(8, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_LONG));
            Assert::AreEqual(4, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_SINGLE));
            Assert::AreEqual(8, rasterFileBase::getBPV(paxTypes::ePAX_SF_COMPLEX_DOUBLE));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_SF_MAG_PHASE_UCHAR));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_SF_MAG_PHASE_CHAR));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_SF_RGB_UCHAR));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_SF_HSV_UCHAR));
            Assert::AreEqual(0, rasterFileBase::getBPV(paxTypes::ePAX_SF_UNDEFINED_PIXEL_TYPE));

            Assert::AreEqual(0, rasterFileBase::getBPV(paxTypes::ePAX_CUSTOM));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_CHAR));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_UCHAR));
            Assert::AreEqual(2, rasterFileBase::getBPV(paxTypes::ePAX_SHORT));
            Assert::AreEqual(2, rasterFileBase::getBPV(paxTypes::ePAX_USHORT));
            Assert::AreEqual(4, rasterFileBase::getBPV(paxTypes::ePAX_INT));
            Assert::AreEqual(4, rasterFileBase::getBPV(paxTypes::ePAX_UINT));
            Assert::AreEqual(8, rasterFileBase::getBPV(paxTypes::ePAX_LONG));
            Assert::AreEqual(8, rasterFileBase::getBPV(paxTypes::ePAX_ULONG));
            Assert::AreEqual(2, rasterFileBase::getBPV(paxTypes::ePAX_HALF));
            Assert::AreEqual(4, rasterFileBase::getBPV(paxTypes::ePAX_FLOAT));
            Assert::AreEqual(8, rasterFileBase::getBPV(paxTypes::ePAX_DOUBLE));
            Assert::AreEqual(6, rasterFileBase::getBPV(paxTypes::ePAX_QUADRUPLE));

            Assert::AreEqual(0, rasterFileBase::getBPV(paxTypes::ePAX_META_ONLY));
            Assert::AreEqual(4, rasterFileBase::getBPV(paxTypes::ePAX_FLOAT3));

            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_PBM_ASCII));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_PGM_ASCII));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_PPM_ASCII));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_PBM_BINARY));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_PGM_BINARY));
            Assert::AreEqual(1, rasterFileBase::getBPV(paxTypes::ePAX_PPM_BINARY));
        }

        TEST_METHOD(valuesPerElement)
        {
            Logger::WriteMessage("Starting valuesPerElement");

            Assert::AreEqual(vpe_e::eVPE_REAL,          ValueSpace::lookupVpe(value_space_e::eVS_REAL));
            Assert::AreEqual(vpe_e::eVPE_IMAGINARY,     ValueSpace::lookupVpe(value_space_e::eVS_IMAGINARY));
            Assert::AreEqual(vpe_e::eVPE_COMPLEX,       ValueSpace::lookupVpe(value_space_e::eVS_COMPLEX));
            Assert::AreEqual(vpe_e::eVPE_POLAR,         ValueSpace::lookupVpe(value_space_e::eVS_POLAR));
            Assert::AreEqual(vpe_e::eVPE_REAL2,         ValueSpace::lookupVpe(value_space_e::eVS_REAL2));
            Assert::AreEqual(vpe_e::eVPE_IMAGINARY2,    ValueSpace::lookupVpe(value_space_e::eVS_IMAGINARY2));
            Assert::AreEqual(vpe_e::eVPE_COMPLEX2,      ValueSpace::lookupVpe(value_space_e::eVS_COMPLEX2));
            Assert::AreEqual(vpe_e::eVPE_POLAR2,        ValueSpace::lookupVpe(value_space_e::eVS_POLAR2));
            Assert::AreEqual(vpe_e::eVPE_REAL3,         ValueSpace::lookupVpe(value_space_e::eVS_REAL3));
            Assert::AreEqual(vpe_e::eVPE_IMAGINARY3,    ValueSpace::lookupVpe(value_space_e::eVS_IMAGINARY3));
            Assert::AreEqual(vpe_e::eVPE_COMPLEX3,      ValueSpace::lookupVpe(value_space_e::eVS_COMPLEX3));
            Assert::AreEqual(vpe_e::eVPE_POLAR3,        ValueSpace::lookupVpe(value_space_e::eVS_POLAR3));
            Assert::AreEqual(vpe_e::eVPE_REAL4,         ValueSpace::lookupVpe(value_space_e::eVS_REAL4));
            Assert::AreEqual(vpe_e::eVPE_IMAGINARY4,    ValueSpace::lookupVpe(value_space_e::eVS_IMAGINARY4));
            Assert::AreEqual(vpe_e::eVPE_COMPLEX4,      ValueSpace::lookupVpe(value_space_e::eVS_COMPLEX4));
            Assert::AreEqual(vpe_e::eVPE_POLAR4,        ValueSpace::lookupVpe(value_space_e::eVS_POLAR4));
            Assert::AreEqual(vpe_e::eVPE_BITS,          ValueSpace::lookupVpe(value_space_e::eVS_BITS));

            Assert::AreEqual(vpe_e::eVPE_MAG,           ValueSpace::lookupVpe(value_space_e::eVS_MAG));
            Assert::AreEqual(vpe_e::eVPE_PHASE,         ValueSpace::lookupVpe(value_space_e::eVS_PHASE));
            Assert::AreEqual(vpe_e::eVPE_MAGPHASE,      ValueSpace::lookupVpe(value_space_e::eVS_MAGPHASE));
            Assert::AreEqual(vpe_e::eVPE_IQ,            ValueSpace::lookupVpe(value_space_e::eVS_IQ));

            Assert::AreEqual(vpe_e::eVPE_RGB,           ValueSpace::lookupVpe(value_space_e::eVS_RGB));
            Assert::AreEqual(vpe_e::eVPE_HSV,           ValueSpace::lookupVpe(value_space_e::eVS_HSV));

            Assert::AreEqual(vpe_e::eVPE_ONE,           ValueSpace::lookupVpe(value_space_e::eVS_ONE));
            Assert::AreEqual(vpe_e::eVPE_TWO,           ValueSpace::lookupVpe(value_space_e::eVS_TWO));
            Assert::AreEqual(vpe_e::eVPE_THREE,         ValueSpace::lookupVpe(value_space_e::eVS_THREE));
            Assert::AreEqual(vpe_e::eVPE_FOUR,          ValueSpace::lookupVpe(value_space_e::eVS_FOUR));
            Assert::AreEqual(vpe_e::eVPE_FIVE,          ValueSpace::lookupVpe(value_space_e::eVS_FIVE));
            Assert::AreEqual(vpe_e::eVPE_SIX,           ValueSpace::lookupVpe(value_space_e::eVS_SIX));
            Assert::AreEqual(vpe_e::eVPE_UNDEFINED,     ValueSpace::lookupVpe(value_space_e::eVS_UNDEFINED));
        }

    };
}
