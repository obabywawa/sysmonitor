#include "simple_mysql.h"
#include <errmsg.h>
# define CHECK_MYSQL_API(x)
# define ASSERT_MYSQL_API
# define MYSQL_CAPI_CALL(x)   mysql_ ## x
# define MYSQL_ERRNO          mysql_errno


//////////////////////////////////////////////////////////////////////////
//
#define CHECK_FIELD_EX  \
	ASSERT(m_row); \
if (idx > m_filed_count) { return RC_E_INDEX; }\

#define CHECK_FIELD \
	CHECK_FIELD_EX  \
if (NULL == *(m_row + idx)) { return RC_S_NULL_VALUE; }\

rc_t CMysqlRecord::get_data(UINT32_T idx, UINT8_T** val, UINT32_T* len) {

	CHECK_FIELD_EX;
	if (NULL == m_lengths || *(m_lengths + idx) > kuint32max) { return RC_E_NOMEM; }

	(*val) = (UINT8_T *)(*(m_row + idx));
	(*len) = (UINT32_T)*(m_lengths + idx);
	return RC_S_OK;
}

rc_t CMysqlRecord::get_data(UINT32_T idx, UCHAR_T** val) {

	CHECK_FIELD;
	(*val) = (UINT8_T*)(*(m_row + idx));
	return RC_S_OK;
}

rc_t CMysqlRecord::get_data(UINT32_T idx, UINT16_T* val) {

	CHECK_FIELD;
	(*val) = (UINT16_T)ATOI(*(m_row + idx));
	return RC_S_OK;
}

rc_t CMysqlRecord::get_data(UINT32_T idx, UINT32_T* val) {

	CHECK_FIELD;
	(*val) = (UINT32_T)ATOI(*(m_row + idx));
	return RC_S_OK;
}

rc_t CMysqlRecord::get_data(UINT32_T idx, UINT64_T* val) {

	CHECK_FIELD;
	(*val) = (UINT64_T)_atoi64(*(m_row + idx));
	return RC_S_OK;
}

rc_t CMysqlRecord::get_data(UINT32_T idx, FLOAT_T* val) {

	CHECK_FIELD;
	(*val) = (FLOAT_T)ATOI(*(m_row + idx));
	return RC_S_OK;
}

rc_t CMysqlRecord::get_data(UINT32_T idx, DOUBLE_T* val) {

	CHECK_FIELD;
	(*val) = (DOUBLE_T)atof(*(m_row + idx));
	return RC_S_OK;
}


void CMysqlRecord::Release() {}

CMysqlRecord::CMysqlRecord()
: m_row(NULL)
, m_filed_count(0)
, m_lengths(NULL)
{}

CMysqlRecord::~CMysqlRecord() {}

void CMysqlRecord::set_mysql_row(const MYSQL_ROW row, UINT32_T filed_count, const unsigned long* lengths) {

	m_row = row;
	m_filed_count = filed_count;
	m_lengths = lengths;
}

//////////////////////////////////////////////////////////////////////////
//
rc_t CMysqlRecordSet::fetch() {

	ASSERT(m_res);
	ASSERT_MYSQL_API;
	//m_row = MYSQL_CAPI_CALL(fetch_row)(m_res);
	return m_row ? RC_S_OK : RC_S_CURSOR_END;
}

rc_t CMysqlRecordSet::close() { ASSERT(m_res); m_res = NULL; return RC_S_OK; }

rc_t CMysqlRecordSet::first() {

	ASSERT(m_res);
	ASSERT_MYSQL_API;
	m_curr_row_no = 0;
	MYSQL_CAPI_CALL(data_seek)(m_res, m_curr_row_no);

	m_row = MYSQL_CAPI_CALL(fetch_row)(m_res);
	return m_row ? RC_S_OK : RC_S_CURSOR_END;
}

rc_t CMysqlRecordSet::last() {

	ASSERT(m_res);
	ASSERT_MYSQL_API;
	m_curr_row_no = MYSQL_CAPI_CALL(num_rows)(m_res);
	MYSQL_CAPI_CALL(data_seek)(m_res, m_curr_row_no);

	m_row = MYSQL_CAPI_CALL(fetch_row)(m_res);
	return m_row ? RC_S_OK : RC_S_CURSOR_END;
}

rc_t CMysqlRecordSet::next() {

	ASSERT(m_res);
	++m_curr_row_no;
	ASSERT_MYSQL_API;
	m_row = MYSQL_CAPI_CALL(fetch_row)(m_res);
	return m_row ? RC_S_OK : RC_S_CURSOR_END;
}

rc_t CMysqlRecordSet::prev() {

	ASSERT(m_res);
	--m_curr_row_no;
	ASSERT_MYSQL_API;
	MYSQL_CAPI_CALL(data_seek)(m_res, m_curr_row_no);

	m_row = MYSQL_CAPI_CALL(fetch_row)(m_res);
	return m_row ? RC_S_OK : RC_S_CURSOR_END;
}

rc_t CMysqlRecordSet::get_field_count(UINT32_T* count) {

	ASSERT(m_res);

	(*count) = m_filed_count;
	return RC_S_OK;
}

rc_t CMysqlRecordSet::get_field_name(CHAR_T** name, UINT32_T idx) {
	
	ASSERT(m_fields);	
	*name = m_fields[idx].name;
	return RC_E_UNSUPPORTED;
}

rc_t CMysqlRecordSet::get_field_idx(UINT32_T* idx, const CHAR_T* name) {

	UNUSED_PARAM(name);
	UNUSED_PARAM(idx);
	return RC_E_UNSUPPORTED;
}

rc_t CMysqlRecordSet::get_row_count(UINT32_T* count) {

	ASSERT(count);

	if (m_row_count > kuint32max) { return RC_E_NOMEM; }

	(*count) = static_cast<UINT32_T>(m_row_count);
	return RC_S_OK;
}

rc_t CMysqlRecordSet::get_row_count(UINT64_T* count) {

	(*count) = m_row_count;
	return RC_S_OK;
}

CMysqlRecord* CMysqlRecordSet::get_record() {

	ASSERT(m_res);
	if (NULL == m_row) { return NULL; }

	ASSERT_MYSQL_API;
	m_lengths = MYSQL_CAPI_CALL(fetch_lengths)(m_res);
	m_rec.set_mysql_row(m_row, m_filed_count, m_lengths);
	return &m_rec;
}

void CMysqlRecordSet::Release() {}

CMysqlRecordSet::CMysqlRecordSet()
: m_res(NULL)
, m_row(NULL)
, m_rec()
, m_curr_row_no(0)
, m_lengths(NULL)
, m_row_count(0)
, m_filed_count(0)
{}

CMysqlRecordSet::~CMysqlRecordSet() {}

void CMysqlRecordSet::set_mysql_res(MYSQL_RES* res) {

	ASSERT(res);
	m_res = res;
	m_curr_row_no = 0;

	ASSERT_MYSQL_API;
	m_filed_count = MYSQL_CAPI_CALL(num_fields)(m_res);
	m_row_count = MYSQL_CAPI_CALL(num_rows)(m_res);
	m_fields = MYSQL_CAPI_CALL(fetch_fields)(m_res);

	// open first
	m_row = MYSQL_CAPI_CALL(fetch_row)(m_res);
}

//////////////////////////////////////////////////////////////////////////
//
const CHAR_T*     CMysqlConnection::m_gcStrName = _STR("MYSQLCI");

CMysqlConnection* CMysqlConnection::CreateInstance(const CHAR_T*) {

	CHECK_MYSQL_API(NULL);
	return new CMysqlConnection();
}

void CMysqlConnection::Release() { delete this; }

CMysqlConnection::CMysqlConnection()
: m_nStatus(RC_S_UNKNOWN)
, m_myres(NULL)
, m_RecordSet()
, m_strDB(NULL)
, m_dbCon(NULL)
{
	ASSERT_MYSQL_API;
	if (MYSQL_CAPI_CALL(init)(&m_mysql)) {
		/* disable mysql auto reconnect */
		m_mysql.reconnect = 0;
		m_nStatus = RC_S_INIT;
	}
}

CMysqlConnection::~CMysqlConnection() {

	if (RC_S_UNKNOWN != m_nStatus) {
		disconnect();
	}
}

rc_t CMysqlConnection::connect(const CHAR_T* strParam) {

	if (RC_S_INIT != m_nStatus) { return RC_S_STATUS; }

	ASSERT(strParam);

	db_conn_str_t dbConStr;
	if (RC_S_OK != db_parse_cs(&dbConStr, strParam)
		|| RC_S_OK != STRCMP(dbConStr.db_driver, db_cs_driver[DB_MYSQL])
		) {

		return RC_E_ILLEGAL_PARAM;
	}

	m_strDB = strParam;
	return connect(&dbConStr);
}

rc_t CMysqlConnection::connect(const db_conn_str_t* pDBConnStr) {

	if (NULL == pDBConnStr) { return RC_S_NULL_VALUE; }

	m_nStatus = RC_S_OPEN;
	m_dbCon = pDBConnStr;

	ASSERT_MYSQL_API;
	if (NULL == MYSQL_CAPI_CALL(real_connect)(&m_mysql,
		pDBConnStr->db_host,
		pDBConnStr->db_user,
		pDBConnStr->db_pwd,
		pDBConnStr->db_name,
		pDBConnStr->db_port,
		pDBConnStr->db_socket,
		pDBConnStr->db_flag)
		) {
		return RC_S_FAILED;
	}

	// character_set
	int error = MYSQL_CAPI_CALL(set_character_set)(&m_mysql, pDBConnStr->db_charset);
	if (error) {
		TRACE(_STR("mysql_set_character_set failed!"));
		RC_RETURN(RC_S_FAILED + error);
	}

	// disable mysql autocommit
	if (MYSQL_CAPI_CALL(autocommit)(&m_mysql, 0)) {
		TRACE(_STR("disable mysql autocommit failed!"));
	}

	return RC_S_OK;
}

rc_t CMysqlConnection::disconnect() {

	clear_all_result_set();

	/* Close & free connection */
	ASSERT_MYSQL_API;
	MYSQL_CAPI_CALL(close)(&m_mysql);
	m_nStatus = RC_S_UNKNOWN;
	return RC_S_OK;
}

rc_t CMysqlConnection::reconnect() {

	if (RC_S_OPEN != m_nStatus) { return RC_S_STATUS; }

	/* get mysql err */
	UINT32_T mysql_err_no;

	clear_all_result_set();

	ASSERT_MYSQL_API;
	mysql_err_no = MYSQL_ERRNO(&m_mysql);
	if (0 == mysql_err_no && TRUE == didConnected()) { return RC_S_OK; }

	if (CR_SERVER_GONE_ERROR == mysql_err_no
		|| CR_SERVER_LOST == mysql_err_no
		) {

		my_bool old_reconnect = m_mysql.reconnect;

		/* Set Automatic reconnect */
		/* default wait_timeout set 28800 (8h) */
		/* or mysqld kill idle conenct */
		m_mysql.reconnect = 1;
		if (0 == MYSQL_CAPI_CALL(ping)(&m_mysql)) {

			m_mysql.reconnect = old_reconnect;
			/* connection was down, but mysqld is now alive */
			return RC_S_OK;
		}

		/* reconnec failed */
		m_mysql.reconnect = old_reconnect;
	}
	else {

		// it's never goto here 
		// clear_all_result_set  2003 ==> 2013
		if (m_strDB) { return connect(m_strDB); }
		else {
			if (m_dbCon) { return connect(m_dbCon); }
		}
	}

	return RC_S_FAILED;
}

rc_t CMysqlConnection::check_connect(BOOL_T bKeepAlive, BOOL_T bFix) {

	if (RC_S_OPEN != m_nStatus) { return RC_S_STATUS; }

	if (TRUE == bFix) {

		clear_all_result_set();
		ASSERT_MYSQL_API;
		int error = MYSQL_CAPI_CALL(ping)(&m_mysql);
		if (0 == error) { return RC_S_OK; }

		// fix
		return reconnect();
	}

	if (TRUE == bKeepAlive) {

		clear_all_result_set();
		ASSERT_MYSQL_API;
		int error = MYSQL_CAPI_CALL(ping)(&m_mysql);
		if (error) { RC_RETURN(RC_S_FAILED + error); }
	}

	return RC_S_OK;
}

static rc_t exchange_error(int error) {

	if (0 == error) { return RC_S_OK; }

	if (CR_SERVER_GONE_ERROR == error
		|| CR_SERVER_LOST == error
		)
	{
		return RC_E_DB_CONNECT;
	}

	return RC_E_DB_QUERY;
}

rc_t CMysqlConnection::execute(const INT8_T *strSQL, BOOL_T bNeedResult) {

	if (RC_S_OPEN != m_nStatus/* || FALSE == didConnected()*/) { return RC_S_STATUS; }

	if (RC_S_OK != reconnect()) { return RC_S_FAILED; }

	// clear prev res.
	clear_all_result_set();
	//reset_error();

	ASSERT_MYSQL_API;
	int error = MYSQL_CAPI_CALL(query)(&m_mysql, strSQL);
	if (error) {

		if (RC_S_OK == reconnect()) {

			error = MYSQL_CAPI_CALL(query)(&m_mysql, strSQL);
			if (error) {

				error = MYSQL_ERRNO(&m_mysql);
				//TRACE(_STR("[MYSQL] ERR_NO=%u, MSG=%s"), error, MYSQL_CAPI_CALL(error)(&m_mysql));
				//RC_RETURN(RC_S_FAILED + error);
				return exchange_error(error);
			}
			// ok
		}
		else {

			TRACE(_STR("[MYSQL] ERR_NO=%u, MSG=%s"), error, MYSQL_CAPI_CALL(error)(&m_mysql));
			//RC_RETURN(RC_S_FAILED + error);
			return exchange_error(error);
		}
		/*
		TRACE(_STR("[MYSQL] ERR_NO=%u, MSG=%s"), error, MYSQL_CAPI_CALL(error)(&m_mysql));
		RC_RETURN(RC_S_FAILED + error);
		*/
	}

	// clear reslut
	if (FALSE == bNeedResult) { return clear_all_result_set(); }
	return RC_S_OK;
}

rc_t CMysqlConnection::clear_all_result_set() {

	ASSERT_MYSQL_API;
	if (m_myres) { MYSQL_CAPI_CALL(free_result)(m_myres); m_myres = NULL; }
	do {
		MYSQL_RES   *res = MYSQL_CAPI_CALL(store_result)(&m_mysql);
		if (res) { MYSQL_CAPI_CALL(free_result)(res); }
	} while (0 == MYSQL_CAPI_CALL(next_result)(&m_mysql));

	int error = MYSQL_ERRNO(&m_mysql);
	//return 0 == error ? RC_S_OK : (rc_t)(RC_S_FAILED + error);
	return exchange_error(error);
}

void CMysqlConnection::reset_error() {
}

BOOL_T CMysqlConnection::didConnected() {

	return (m_mysql.host)
		&& (m_mysql.user)
		&& (m_mysql.passwd)
		&& (m_mysql.server_version)
		&& (m_mysql.host_info)
		&& (m_mysql.thread_id)
		&& (m_mysql.net.vio)
		&& (m_mysql.net.buff)
		? TRUE : FALSE;
}

rc_t CMysqlConnection::commit() {

	if (RC_S_OPEN != m_nStatus) { return RC_S_STATUS; }
	clear_all_result_set();

	ASSERT_MYSQL_API;
	return MYSQL_CAPI_CALL(commit)(&m_mysql) ? RC_S_FAILED : RC_S_OK;
}

rc_t CMysqlConnection::rollback() {

	if (RC_S_OPEN != m_nStatus) { return RC_S_STATUS; }
	clear_all_result_set();

	ASSERT_MYSQL_API;
	return MYSQL_CAPI_CALL(rollback)(&m_mysql) ? RC_S_FAILED : RC_S_OK;
}

const CHAR_T* CMysqlConnection::get_last_error(UINT32_T* err_no) {

	ASSERT(err_no);

	ASSERT_MYSQL_API;
	(*err_no) = MYSQL_ERRNO(&m_mysql);
	return MYSQL_CAPI_CALL(error)(&m_mysql);
}


CMysqlRecordSet* CMysqlConnection::get_record_set() {

	if (RC_S_OPEN != m_nStatus) { return NULL; }

	if (m_myres) {

		// next
		ASSERT_MYSQL_API;
		MYSQL_CAPI_CALL(free_result)(m_myres);
		m_myres = NULL;

		if (MYSQL_CAPI_CALL(next_result)(&m_mysql)) { return NULL; }
	}

	ASSERT_MYSQL_API;
	m_myres = MYSQL_CAPI_CALL(store_result)(&m_mysql);

	/*
	while (NULL == m_myres && 0 == MYSQL_CAPI_CALL(next_result)(&m_mysql)) {
	m_myres = MYSQL_CAPI_CALL(store_result)(&m_mysql);
	}
	*/

	if (NULL == m_myres) { return NULL; }
	m_RecordSet.set_mysql_res(m_myres);
	return &m_RecordSet;
}

//////////////////////////////////////////////////////////////////////////
rc_t CMysqlConnection::db_lib_init() {

	ASSERT_MYSQL_API;
	int error = MYSQL_CAPI_CALL(library_init)(0, NULL, NULL);
	if (0 != error) {
		RC_RETURN(RC_S_FAILED + error);
	}
	return RC_S_OK;
}

void CMysqlConnection::db_lib_deinit() {

	ASSERT_MYSQL_API;
	MYSQL_CAPI_CALL(library_end());
}

rc_t CMysqlConnection::db_thd_init() {

	ASSERT_MYSQL_API;
	int error = MYSQL_CAPI_CALL(thread_init)();
	if (0 != error) {
		RC_RETURN(RC_S_FAILED + error);
	}
	return RC_S_OK;
}

void CMysqlConnection::db_thd_deinit() {

	ASSERT_MYSQL_API;
	MYSQL_CAPI_CALL(thread_end)();
}