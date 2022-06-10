#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

/* 配置文件
 * 以键值对的方式配置，
 * '#'作为注释
 * */
class CConfigFile
{
private:
	CConfigFile(void);
	~CConfigFile(void);

public:
	static CConfigFile* GetInstancePtr();

public:
    // 加载配置文件
	BOOL Load(std::string strFileName);

    // 读取配置
	std::string GetStringValue(std::string strName);

	INT32 GetIntValue( std::string VarName);

	FLOAT GetFloatValue( std::string VarName);

	DOUBLE GetDoubleValue( std::string VarName);

	INT32  GetRealNetPort(std::string VarName);

private:
	std::map<std::string, std::string> m_Values;
};

#endif