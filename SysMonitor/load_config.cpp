#include "load_config.h"
#include "func.h"

CLoadConfig::CLoadConfig()
{
	m_monitor_config = new MonitorConfig;
	//memset(m_monitor_config, 0, sizeof(MonitorConfig));
}

CLoadConfig::~CLoadConfig()
{
	for (int i = 0; i < m_monitor_config->counter_num; i++)
		TDELARRAY(m_monitor_config->counter_name[i]);
	TDELARRAY(m_monitor_config->counter_name);

	for (int i = 0; i < m_monitor_config->web_counter_num; i++)
		TDELARRAY(m_monitor_config->web_counter_name[i]);
	TDELARRAY(m_monitor_config->web_counter_name);

	for (int i = 0; i < m_monitor_config->process_num; i++)
		TDELARRAY(m_monitor_config->process_name[i]);
	TDELARRAY(m_monitor_config->process_name);
	for (int i = 0; i < m_monitor_config->mysql_database_num; i++)
		TDELARRAY(m_monitor_config->mysql_connstr[i]);
	TDELARRAY(m_monitor_config->mysql_connstr);
	for (int i = 0; i < m_monitor_config->ndb_file_num; i++)
		TDELARRAY(m_monitor_config->ndb_log_file_name[i]);
	TDELARRAY(m_monitor_config->ndb_log_file_name);
	TDEL(m_monitor_config);

}

CLoadConfig* CLoadConfig::_instance = NULL;
CLoadConfig* CLoadConfig::CreateInstance()
{
	if (_instance == NULL)
		_instance = new CLoadConfig;
	return _instance;
}

void CLoadConfig::LoadConfig()
{

	char filebuf[256] = "";
	GetCurrentPath(filebuf, "config.ini");
	
	//service
	m_monitor_config->listen_port = GetIniKeyInt("service", "port", filebuf);
	m_monitor_config->log_flag = GetIniKeyInt("service", "logflag", filebuf);
	//monitortype
	{
		//m_monitor_config->object_num = GetIniKeyInt("monitortype", "num", filebuf);
		m_monitor_config->object_type.resize(OBJECT_NUM/*m_monitor_config->object_num*/);
		for (int i = 0; i < OBJECT_NUM; i++){
			char objectkey[6] = "";
			sprintf_s(objectkey, sizeof(objectkey), "%s%d", "type", i + 1);
			m_monitor_config->object_type[i] = GetIniKeyInt("monitortype", objectkey, filebuf);
		}
	}
	{
		for (int i = 0; i < OBJECT_NUM; i++){
			if (m_monitor_config->object_type[i] == CONFIG_SYSTEM){
				m_monitor_config->counter_num = GetIniKeyInt("system", "counter_num", filebuf);
				m_monitor_config->counter_name = new char*[m_monitor_config->counter_num];
				for (int i = 0; i < m_monitor_config->counter_num; i++){
					char counter_key[20] = "";
					sprintf_s(counter_key, sizeof(counter_key), "%s%d", "counter_name", i + 1);
					char* counter_name = GetIniKeyString("system", counter_key, filebuf);
					m_monitor_config->counter_name[i] = new char[strlen(counter_name)+1];
					memcpy(m_monitor_config->counter_name[i], counter_name, strlen(counter_name) + 1);
				}
			}
			else if (m_monitor_config->object_type[i] == CONFIG_MYSQL){
				m_monitor_config->mysql_type = GetIniKeyInt("mysql", "dbtype", filebuf);
				m_monitor_config->mysql_database_num = GetIniKeyInt("mysql", "dbcount", filebuf);
				m_monitor_config->mysql_connstr = new char*[m_monitor_config->mysql_database_num];
				for (int i = 0; i < m_monitor_config->mysql_database_num;i++){
					char connstr_key[20] = "";
					sprintf_s(connstr_key, sizeof(connstr_key), "%s%d", "connectonstr", i + 1);
					char* con_str = GetIniKeyString("mysql", connstr_key, filebuf);
					m_monitor_config->mysql_connstr[i] = new char[strlen(con_str) + 1];
					strcpy(m_monitor_config->mysql_connstr[i], con_str);
				}
				
			}
			else if (m_monitor_config->object_type[i] == CONFIG_WEB){
				m_monitor_config->web_counter_num = GetIniKeyInt("web", "counter_num", filebuf);
				m_monitor_config->web_counter_name = new char*[m_monitor_config->web_counter_num];
				for (int i = 0; i < m_monitor_config->web_counter_num; i++){
					char counter_key[20] = "";
					sprintf_s(counter_key, sizeof(counter_key), "%s%d", "counter_name", i + 1);
					char* counter_name = GetIniKeyString("system", counter_key, filebuf);
					m_monitor_config->web_counter_name[i] = new char[strlen(counter_name) + 1];
					memcpy(m_monitor_config->web_counter_name[i], counter_name, strlen(counter_name) + 1);
				}
			}
			else if (m_monitor_config->object_type[i] == CONFIG_PROCESS){
				m_monitor_config->process_num = GetIniKeyInt("process", "process_num", filebuf);
				m_monitor_config->process_name = new char*[m_monitor_config->process_num];
				for (int i = 0; i < m_monitor_config->process_num; i++){
					char process_key[20] = "";
					sprintf_s(process_key, sizeof(process_key), "%s%d", "process_name", i + 1);
					char* process_name = GetIniKeyString("process", process_key, filebuf);
					m_monitor_config->process_name[i] = new char[strlen(process_name)+1];
					memcpy(m_monitor_config->process_name[i], process_name, strlen(process_name) + 1);
				}
			}
			else if (m_monitor_config->object_type[i] == CONFIG_MSSQL){
				m_monitor_config->db_count = GetIniKeyInt("mssql", "dbcount", filebuf);
				m_monitor_config->db_default_sel = GetIniKeyInt("mssql", "dbsel", filebuf);
				for (int i = 0; i < m_monitor_config->db_count; i++){
					char db_source_key[20] = "";
					char db_key[20] = "";
					char db_user_key[20] = "";
					char db_password_key[20] = "";
					sprintf_s(db_source_key, sizeof(db_source_key), "%s%d", "data_source", i + 1);
					sprintf_s(db_key, sizeof(db_key), "%s%d", "data_base", i + 1);
					sprintf_s(db_user_key, sizeof(db_user_key), "%s%d", "user_name", i + 1);
					sprintf_s(db_password_key, sizeof(db_password_key), "%s%d", "password", i + 1);
					strcpy(m_monitor_config->db_config[i].data_source, GetIniKeyString("mssql", db_source_key, filebuf));
					strcpy(m_monitor_config->db_config[i].data_base, GetIniKeyString("mssql", db_key, filebuf));
					strcpy(m_monitor_config->db_config[i].user_name, GetIniKeyString("mssql", db_user_key, filebuf));
					strcpy(m_monitor_config->db_config[i].password, GetIniKeyString("mssql", db_password_key, filebuf));
				}
			}
			else if (m_monitor_config->object_type[i] == CONFIG_NDB){
				m_monitor_config->ndb_file_num = GetIniKeyInt("ndb", "num", filebuf);
				m_monitor_config->ndb_log_file_name = new char*[m_monitor_config->ndb_file_num];
				for (int i = 0; i < m_monitor_config->ndb_file_num; i++){
					char process_key[20] = "";
					sprintf_s(process_key, sizeof(process_key), "%s%d", "name", i + 1);
					char* process_name = GetIniKeyString("ndb", process_key, filebuf);
					m_monitor_config->ndb_log_file_name[i] = new char[strlen(process_name) + 1];
					memcpy(m_monitor_config->ndb_log_file_name[i], process_name, strlen(process_name) + 1);
				}
			}
			
		}
	}

}

int CLoadConfig::get_port()
{
	if (m_monitor_config)
		return m_monitor_config->listen_port;
	return 0;
}

int CLoadConfig::get_log_flag()
{
	if (m_monitor_config)
		return m_monitor_config->log_flag;
	return 0;
}

vector< short > CLoadConfig::get_object_type()
{
	if (m_monitor_config)
		return m_monitor_config->object_type;
	return vector< short >(0);
}

int CLoadConfig::get_object_num()
{
/*
	if (m_monitor_config)
		return m_monitor_config->object_num;
	return 0;
*/
	return OBJECT_NUM;
}

int CLoadConfig::get_counter_num()
{
	if (m_monitor_config)
		return m_monitor_config->counter_num;
	return 0;
}

char** CLoadConfig::get_counter_name()
{
	if (m_monitor_config)
		return m_monitor_config->counter_name;
	return NULL;
}

int CLoadConfig::get_web_counter_num()
{
	if (m_monitor_config)
		return m_monitor_config->web_counter_num;
	return 0;
}

char** CLoadConfig::get_web_counter_name()
{
	if (m_monitor_config)
		return m_monitor_config->web_counter_name;
	return NULL;
}

char** CLoadConfig::get_ndb_log_file_name()
{
	if (m_monitor_config)
		return m_monitor_config->ndb_log_file_name;
	return NULL;
}

int CLoadConfig::get_ndb_log_num()
{
	if (m_monitor_config)
		return m_monitor_config->ndb_file_num;
	return -1;
}

void CLoadConfig::get_sys_os_info()
{	
#ifdef WIN32
	FILE *ppipe = NULL;
	char* pbuffer = new char[1000];
	int nread_line = 0;
	int len = 0;
	ppipe = _popen("systeminfo /FO CSV /NH ", "rt");
	fgets(pbuffer, 1000, ppipe);
	char* tembufer = pbuffer;
	while (nread_line < 3){
		char* tempstr = strchr(tembufer, ',');
		if (tempstr){
			len = tempstr - tembufer;
			char* tempvalue = new char[len + 1];
			strncpy(tempvalue, tembufer, len);
			tempvalue[len] = '\0';
			if (nread_line == 1)
				strcpy_s(m_os_name, tempvalue);
			else if (nread_line == 2)
				strcpy_s(m_os_version, tempvalue);
			TDELARRAY(tempvalue);
			tembufer = tembufer + len + 1;
		}
		nread_line++;
	}
	TDELARRAY(pbuffer);
	if (feof(ppipe))
		_pclose(ppipe);
#endif
}


char* CLoadConfig::get_os_name()
{
	return m_os_name;
}

char* CLoadConfig::get_os_version()
{
	return m_os_version;
}
int CLoadConfig::get_process_num()
{
	if (m_monitor_config)
		return m_monitor_config->process_num;
	return 0;
}
char** CLoadConfig::get_process_name()
{
	if (m_monitor_config)
		return m_monitor_config->process_name;
	return NULL;
}
short CLoadConfig::get_db_count()
{
	if (m_monitor_config)
		return m_monitor_config->db_count;
	return 0;
}
short CLoadConfig::get_db_default_sel()
{
	if (m_monitor_config)
		return m_monitor_config->db_default_sel;
	return 0;
}
LPDBCONFIG CLoadConfig::get_db_config()
{
	if (m_monitor_config)
		return m_monitor_config->db_config;
	return NULL;
}

const char* CLoadConfig::get_mysql_connection_string(int con_index)
{
	if (m_monitor_config)
		return m_monitor_config->mysql_connstr[con_index];
	return NULL;
}

int CLoadConfig::get_mysql_type()
{
	if (m_monitor_config)
		return m_monitor_config->mysql_type;
	return 0;
}

int CLoadConfig::get_mysql_dbcount()
{
	if (m_monitor_config)
		return m_monitor_config->mysql_database_num;
	return 0;
}