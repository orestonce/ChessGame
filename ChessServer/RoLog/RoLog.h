#ifndef ROLOG_H
#define ROLOG_H

#include "RoSpace/RoSingleton.h"
#include <iostream>
#include <iomanip>

namespace RoLog
{

class CRoLog
        : public RoSpace::CSingleton<CRoLog>
{
public:
    enum ELogFlags
    {
        ELogFlags_COUT,  // 普通输出
        ELogFlags_CWRM,// 警告输出
        ELogFlags_CERR   // 错误信息
    };

    template<typename T>
    CRoLog& OutPut (const T& t)
    {
        std::cout << t << " ";

        return *this;
    }

    CRoLog& OutPut(const char* p)
    {
        std::cout << p << " ";

        return *this;
    }

    CRoLog& OutPut(bool b)
    {
        std::cout << std::boolalpha << b << " ";
        return *this;
    }

    CRoLog& EndLog()
    {
        std::cout << std::endl;

        return *this;
    }

    CRoLog& StartLog(const char* pFile,
                     int iLine,
                     const char* pFunc,
                     ELogFlags eLogFlags)
    {
        const char* pHeader;

        switch ( eLogFlags )
        {
        case ELogFlags_COUT:
            pHeader = "[KCOUT] : ";
            break;
        case ELogFlags_CWRM:
            pHeader = "[KWRM ]:";
            break;
        case ELogFlags_CERR:
            pHeader = "[KCERR]:";
            break;
        default:
            pHeader = "[KUNK] :";
            break;
        }

        std::cout << pHeader  << pFile << ":" << iLine << ":" << pFunc << ":" ;

        return *this;
    }
};

#define START_LOG( x ) (*RoLog::CRoLog::Instance ()).StartLog( __FILE__, __LINE__, __func__, x )
#define OUT_LOG(x) OutPut(x)
#define BUILD_LOG( x ) OutPut( #x ).OutPut( " = ").OutPut( x )
#define END_LOG() EndLog()

#define RO_COUT     RoLog::CRoLog::ELogFlags_COUT
#define RO_CWARM RoLog::CRoLog::ELogFlags_CWRM
#define RO_CERR      RoLog::CRoLog::ELogFlags_CERR

}

#endif // ROLOG_H
