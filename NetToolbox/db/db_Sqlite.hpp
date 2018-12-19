#ifndef __DB_SQLITE_HPP__
#define __DB_SQLITE_HPP__

#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <functional>

#include <sqlite3.h>



class SqliteValue: public std::string {
public:
	SqliteValue (const char *val = nullptr): std::string (val ? val : ""), m_is_null (!val) {}
	SqliteValue (SqliteValue &o): std::string (o), m_is_null (o.m_is_null) {}
	void operator= (SqliteValue &o) { __super::assign (o.cbegin (), o.cend ()); m_is_null = o.m_is_null; }
	void assign (const char *val) { __super::assign (val ? val : ""); m_is_null = !val; }
	bool empty () { return m_is_null || empty (); }
	bool is_null () const { return m_is_null; }

private:
	bool m_is_null = false;
};

using SqliteRow = std::vector<SqliteValue>;
using SqliteColumn = SqliteRow;
using SqliteTable = std::vector<SqliteRow>;

class db_Sqlite {
public:
	db_Sqlite (std::string_view path) {
		if (sqlite3_open (path.data (), &m_db)) {
			if (m_db) { sqlite3_close (m_db); m_db = nullptr; }
			return;
		}
	}
	~db_Sqlite () { if (m_db) sqlite3_close (m_db); }

	std::tuple<std::string, SqliteTable> execute (std::string_view sql) {
		std::string info = "执行完毕。";
		char *err_msg = nullptr;
		SqliteTable data;
		std::function<void (int, char**, char**)> _proc_data = [&data] (int argc, char **argv, char **col_name) {
			if (data.size () == 0) {
				SqliteRow v_hdr;
				for (size_t i = 0; i < argc; ++i)
					v_hdr.push_back (col_name[i]);
				data.push_back (v_hdr);
			}
			SqliteRow v_data;
			for (size_t i = 0; i < argc; ++i)
				v_data.push_back (argv[i]);
			data.push_back (v_data);
		};
		if (SQLITE_OK != sqlite3_exec (m_db, sql.data (), exec_callback, (void*) &_proc_data, &err_msg)) {
			info = err_msg;
			sqlite3_free (err_msg);
		}
		return { info, data };
	}

	SqliteColumn execute_list (std::string_view sql) {
		char *err_msg = nullptr;
		SqliteColumn data;
		std::function<void (int, char**, char**)> _proc_data = [&data] (int argc, char **argv, char **col_name) {
			data.push_back (argv[0]);
		};
		if (SQLITE_OK != sqlite3_exec (m_db, sql.data (), exec_callback, (void*) &_proc_data, &err_msg)) {
			sqlite3_free (err_msg);
		}
		return data;
	}

	SqliteRow execute_row (std::string_view sql) {
		char *err_msg = nullptr;
		SqliteRow data;
		std::function<void (int, char**, char**)> _proc_data = [&data] (int argc, char **argv, char **col_name) {
			if (data.empty ()) {
				for (size_t i = 0; i < argc; ++i) {
					data.push_back (argv[i]);
				}
			}
		};
		if (SQLITE_OK != sqlite3_exec (m_db, sql.data (), exec_callback, (void*) &_proc_data, &err_msg)) {
			sqlite3_free (err_msg);
		}
		return data;
	}

	SqliteValue execute_scalar (std::string_view sql) {
		char *err_msg = nullptr;
		SqliteValue data;
		std::function<void (int, char**, char**)> _proc_data = [&data] (int argc, char **argv, char **col_name) {
			if (data.empty ())
				data.assign (argv[0]);
		};
		if (SQLITE_OK != sqlite3_exec (m_db, sql.data (), exec_callback, (void*) &_proc_data, &err_msg)) {
			sqlite3_free (err_msg);
		}
		return data;
	}

	SqliteColumn get_table_list () { return execute_list ("select tbl_name from sqlite_master where type = 'table';"); }

private:
	static int exec_callback (void *p_arg, int argc, char **argv, char **col_name) {
		auto data = (std::function<void (int, char**, char**)>*) p_arg;
		(*data) (argc, argv, col_name);
		return 0;
	}

	sqlite3 *m_db = nullptr;
};



#endif //__DB_SQLITE_HPP__
