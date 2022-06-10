#include "stdafx.h"
#include "ConfigFile.h"

CConfigFile::CConfigFile( void )
{

}

CConfigFile::~CConfigFile( void )
{

}

BOOL CConfigFile::Load( std::string strFileName )
{
    FILE* pFile = fopen(strFileName.c_str(), "r+");

    if(pFile == NULL)
    {
        return FALSE;
    }

    CHAR szBuff[256] = {0};

    do
    {
        fgets(szBuff, 256, pFile);

        if(szBuff[0] == '#')
        {
            continue;       // 跳过注释的内容
        }

        CHAR* pChar = strchr(szBuff, '=');
        if(pChar == NULL)
        {
            continue;       // 跳过空行
        }

        std::string strName;
        strName.assign(szBuff, pChar - szBuff);
        std::string strValue = pChar + 1;
        CommonConvert::StringTrim(strName);
        CommonConvert::StringTrim(strValue);

        m_Values.insert(std::make_pair(strName, strValue));

    }
    while(!feof(pFile));

    fclose(pFile);

    return TRUE;
}


CConfigFile* CConfigFile::GetInstancePtr()
{
    static CConfigFile ConfigFile;

    return &ConfigFile;
}

std::string CConfigFile::GetStringValue( std::string strName )
{
    std::map<std::string, std::string>::iterator itor = m_Values.find(strName);
    if(itor != m_Values.end())
    {
        return itor->second;
    }

    // 没有配置
    CLog::GetInstancePtr()->LogError("无效的配制选项: [%s]", strName.c_str());

    return "";
}

INT32 CConfigFile::GetIntValue( std::string VarName )
{
    return atoi(GetStringValue(VarName).c_str());
}

FLOAT CConfigFile::GetFloatValue( std::string VarName )
{
    return (float)atof(GetStringValue(VarName).c_str());
}

DOUBLE CConfigFile::GetDoubleValue( std::string VarName )
{
    return atof(GetStringValue(VarName).c_str());
}

INT32 CConfigFile::GetRealNetPort(std::string VarName)
{
    INT32 nPort = GetIntValue(VarName);
    if (nPort > 0)
    {
        return nPort;
    }

    // 按区分ID返回当前服务器的端口号
    if (VarName == "logic_svr_port")
    {
        return GetIntValue("areaid") + 10000;
    }
    else if (VarName == "db_svr_port")
    {
        return GetIntValue("areaid") + 20000;
    }
    else if (VarName == "proxy_svr_port")
    {
        return GetIntValue("areaid") + 30000;
    }
    else if (VarName == "log_svr_port")
    {
        return GetIntValue("areaid") + 40000;
    }
    return GetIntValue(VarName);
}

