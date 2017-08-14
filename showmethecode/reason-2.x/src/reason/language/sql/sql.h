
/**
 * Reason
 * Copyright (C) 2008  Emerson Clarke
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef LANGUAGE_SQL_H
#define LANGUAGE_SQL_H

#include "reason/platform/thread.h"
#include "reason/system/string.h"
#include "reason/system/path.h"
#include "reason/system/time.h"

#include "reason/structure/list.h"
#include "reason/structure/array.h"
#include "reason/structure/map.h"
#include "reason/structure/stack.h"

#include "reason/reason.h"

#ifdef REASON_PLATFORM_WINDOWS

#undef TRANSACTION_COMMIT
#undef TRANSACTION_ROLLBACK

#endif

#ifdef REASON_USING_POSTGRES

#include <libpq-fe.h>
#endif

#ifdef REASON_USING_SQLITE
struct sqlite3;
struct sqlite3_stmt;
#include <stdio.h>
#include <sqlite3.h>

#endif

#ifdef REASON_USING_MYSQL
#include <mysql.h>
#endif

using namespace Reason::System;
using namespace Reason::Structure;


namespace Reason { namespace Language { namespace Sql {

class SqlTime : public String
{
public:

	SqlTime(const Time & time)
	{
		Format("%04d-%02d-%02d %02d:%02d:%02d",((Time&)time).Year(),((Time&)time).Month(),((Time&)time).Day(),((Time&)time).Hour(),((Time&)time).Minute(),((Time&)time).Second());
	}

	SqlTime()
	{
		Time time;
		Format("%04d-%02d-%02d %02d:%02d:%02d",time.Year(),time.Month(),time.Day(),time.Hour(),time.Minute(),time.Second());
	}

	using String::Construct;
	void Construct(const Time & time)
	{
		Format("%04d-%02d-%02d %02d:%02d:%02d",((Time&)time).Year(),((Time&)time).Month(),((Time&)time).Day(),((Time&)time).Hour(),((Time&)time).Minute(),((Time&)time).Second());
	}

	void Construct(const Calendar & calendar)
	{
		Format("%04d-%02d-%02d %02d:%02d:%02d",calendar.Year,calendar.Month,calendar.Day,calendar.Hour,calendar.Minute,calendar.Second);
	}

	using String::operator =;
	SqlTime & operator = (const Time & time) {Construct(time);return *this;}
	SqlTime & operator = (const Calendar & calendar) {Construct(calendar);return *this;}
};

class SqlFormatter
{
public:

	static Superstring Format(const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);
		FormatVa(superstring,format,va);
		va_end(va);
		return superstring;
	}

	static Superstring FormatVa(const char * format, va_list va)
	{
		Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);
		FormatVa(superstring,format,va);
		return superstring;
	}

	static void Format(String & string, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		FormatVa(string,format,va);
		va_end(va);
	}

	static void FormatVa(String & string, const char * format, va_list va)
	{

		int index=0;
		int count=0;
		Formatter formatter(format,va);

		String sub;
		StringParser parser(format);
		parser.Mark();
		while (!parser.Eof())
		{
			if (parser.Is("%s"))
			{
				parser.Trap();

				if (!parser.Token.IsEmpty())
				{
					sub = parser.Token;
					Formatter::AppendVa(string,sub.Print(),va);

					for (int i=index;i<count;++i)
						formatter.FieldVa(i,&va);
				}

				sub = Formatter::FormatVa("%s",va);										
				Encode(sub);
				string.Append(sub);

				formatter.FieldVa(count,&va);

				parser.Skip(2);
				parser.Mark();
				++count;
				index=count;
			}
			else
			if (parser.Is("%%"))
			{
				parser.Skip(2);
				++count;
			}				
			else
			if (parser.Is("%"))
			{
				parser.Next();
				++count;
			}
			else
			{
				parser.Next();
			}
		}
		parser.Trap();

		if (!parser.Token.IsEmpty())
		{
			sub = parser.Token;
			Formatter::AppendVa(string,sub.Print(),va);
		}
	}

	static void Encode(String & string)
	{
		string.Replace("\\","\\\\");
		string.Replace("'","''");
	}

	static void Decode(String & string)
	{
		string.Replace("\\\\","\\");
		string.Replace("''","'");
	}

};

class SqlDriver;

class SqlQuery
{
public:

	struct SqlClause
	{

	};

	struct SqlClauseSelect
	{

	};

	struct SqlClauseFrom
	{

	};

};

class SqlCursor : public Object
{
public:

	static Identity Instance;
	virtual Identity& Identify() {return Instance;}

public:

	int Row;
	int Column;
	String * Cell;

	SqlCursor():Row(0),Column(0),Cell(0){}
	SqlCursor(int row, int column):Row(row),Column(column),Cell(0){}
	SqlCursor(int row, int column, String * cell):Row(row),Column(column),Cell(cell){}
	SqlCursor(const SqlCursor & cursor):Row(cursor.Row),Column(cursor.Column),Cell(cursor.Cell){}

	SqlCursor & operator = (const SqlCursor & cursor)
	{
		Row = cursor.Row;
		Column = cursor.Column;
		Cell = cursor.Cell;
		return *this;
	}

	bool operator == (const SqlCursor & cursor)
	{
		return Row == cursor.Row && Column == cursor.Column;
	}

	bool operator != (const SqlCursor & cursor)
	{
		return Row != cursor.Row || Column != cursor.Column;
	}

	int Compare(Object * object, int comparitor = COMPARE_GENERAL)
	{
		SqlCursor * cursor = (SqlCursor*)object;
		int result = Row - cursor->Row;
		return (result==0)?Column-cursor->Column:result;
	}

	bool IsNull() {return Cell==0;}
	bool IsEmpty() {return Cell==0 || Cell->IsEmpty();}
};

class SqlRecordset : public Reason::Structure::List< Reason::Structure::Array<String> >
{
private:

	static String Null;
public:

	Reason::Structure::Array<String> Fields;

	SqlRecordset(){}
	~SqlRecordset(){}

	void Reset()
	{
		Release();
		Fields.Release();

	}

	String * Cell(int row, const char * column) {return Cell(row,Fields.IndexOf(column));}
	String * Cell(int row, const Sequence & column) {return Cell(row,Fields.IndexOf(column));}
	String * Cell(int row, int column)
	{

		OutputAssert(Null.IsEmpty());

		if (column >= 0 && column < Fields.Size)
		{

			if (row >= 0 && row < this->Count)
			{
				return &(*this)[row][column];
			}
		}
		return 0;
	}

	SqlCursor Cursor(int row, int column) {return SqlCursor(row,column, Cell(row,column));}
	SqlCursor Cursor(int row, const char * column) {return SqlCursor(row,Fields.IndexOf(column), Cell(row,column));}
	SqlCursor Cursor(int row, const Sequence & column) {return SqlCursor(row,Fields.IndexOf(column), Cell(row,column));}

	SqlCursor Select(const char * data, bool caseless = false) {return Select(SqlCursor(),(char*)data,String::Length(data),caseless);}
	SqlCursor Select(const Sequence & sequence, bool caseless = false) {return Select(SqlCursor(),sequence.Data,sequence.Size,caseless);}
	SqlCursor Select(char * data, int size, bool caseless = false) {return Select(SqlCursor(),data,size,caseless);}

	SqlCursor Select(const SqlCursor & cursor, const char * data, bool caseless = false) {return Select(cursor,(char*)data,String::Length(data),caseless);}
	SqlCursor Select(const SqlCursor & cursor, const Sequence & sequence, bool caseless = false) {return Select(cursor,sequence.Data,sequence.Size,caseless);}
	SqlCursor Select(const SqlCursor & cursor, char * data, int size, bool caseless = false);

	String & operator () (int row, int column)
	{
		String * string = Cell(row,column);
		return (string)?(String&)*string:Null;
	}

	String & operator () (int row, const char * column)
	{
		String * string = Cell(row,column);
		return (string)?(String&)*string:Null;
	}

	String & operator () (int row, const Sequence & column)
	{
		String * string = Cell(row,column);
		return (string)?(String&)*string:Null;
	}

	String & operator () (const char * column)
	{
		String * string = Cell(0,column);
		return (string)?(String&)*string:Null;

	}

	String & operator () (const Sequence & column)
	{
		String * string = Cell(0,column);
		return (string)?(String&)*string:Null;
	}

	String & operator () (int column)
	{
		String * string = Cell(0,column);
		return (string)?(String&)*string:Null;
	}

	bool IsEmpty() {return this->Count <= 0;}
	int Length() {return this->Count;}

};

class SqlRecord;

class SqlDatabase
{
private:

	SqlDriver * Driver;

public:

	SqlDatabase(SqlDriver * driver);
	~SqlDatabase();

	bool Error;
	String Message;

	bool Open(const char * data, const char * user=0, const char * pass=0, const char * host=0, unsigned int port=0);
	bool Close();

	int Execute(SqlRecordset & recordset, const Sequence & query);
	int Execute(SqlRecordset & recordset, const char * query, ...);

	int Execute(SqlRecord & record, const Sequence & query);
	int Execute(SqlRecord & record, const char * query, ...);

	int Execute(const Sequence & query);
	int Execute(const char * query, ...);

	bool Transaction(int transaction);

	bool Begin();
	bool Commit();
	bool Rollback();

	int Id();

	bool IsEmpty();

friend class SqlStatement;
friend class SqlRecord;
};

class SqlStatement : public String
{
private:

	enum SqlState
	{
		SQL_STATE_EXECUTED,
		SQL_STATE_PREPARED,
	};

	int Param;
	int State;

	SqlDatabase * Database;

public:

	enum SqlType
	{
		SQL_TYPE_INTEGER	=1,
		SQL_TYPE_FLOAT		=2,
		SQL_TYPE_TEXT		=3,
		SQL_TYPE_BLOB		=4,
		SQL_TYPE_NULL		=5,
	};

	SqlStatement(SqlDatabase & database);
	SqlStatement();
	~SqlStatement();

	bool Prepare(SqlDatabase & database);
	bool Prepare(SqlDatabase & database, const char * data) {return Prepare(database,(char*)data,String::Length(data));}
	bool Prepare(SqlDatabase & database, const Sequence & sequence) {return Prepare(database,sequence.Data,sequence.Size);}
	bool Prepare(SqlDatabase & database, char * data, int size);
	bool Prepare(const char * data) {return Prepare((char*)data,String::Length(data));}
	bool Prepare(const Sequence & sequence) {return Prepare(sequence.Data,sequence.Size);}
	bool Prepare(char * data, int size);

	SqlStatement & Bind(int value);
	SqlStatement & Bind(double value);
	SqlStatement & Bind(const Sequence & value);
	SqlStatement & Bind(const char * value);
	SqlStatement & Bind();

	virtual bool Integer(int param, int value);
	virtual bool Float(int param, double value);

	virtual bool Text(int param, const Sequence & value);
	virtual bool Text(int param, const char * value);
	virtual bool Null(int param);

	virtual int Execute();
	virtual int Execute(SqlRecordset & recordset);
	virtual int Execute(SqlRecord & record);

	virtual int IndexOf(const Sequence & var) {return 0;}
	virtual int IndexOf(const char * var) {return 0;}
	virtual int TypeOf(int param) {return Parameters[param].First();}

	Reason::Structure::Map< int,Reason::Structure::Pair<int,String> > Parameters;

	void Reset();

friend class SqlDatabase;
friend class SqlRecord;
friend class SqlDriver;
};

class SqlRecord : public Reason::Structure::Array<String>
{
private:
	static String Null;
	SqlDatabase * Database;

public:

	Reason::Structure::Array<String> Fields;
	int Index;

	SqlRecord();
	~SqlRecord();

	bool Step();

	String & operator () (int column)
	{
		return operator [] (column);
	}

	String & operator [] (int column)
	{
		OutputAssert(column >= 0 && column < Length());
		OutputAssert(Null.IsEmpty());
		if ( column < 0 || column >= Length() )
			return Null;
		else
			return Array<String>::operator [] (column);
	}

	void Reset()
	{
		Release();
		Fields.Release();
		Index = -1;
	}

friend class SqlDatabase;
friend class SqlStatement;
friend class SqlDriver;
};

class SqlData
{
public:

	SqlDatabase & Database;

	SqlData(SqlDatabase & database):Database(database)
	{
	}

	bool SelectDistinct(SqlRecordset & recordset, const char * table, const char * field)
	{
		return Database.Execute(recordset,"select %s from %s group by %s having (count(%s) = 1)",field,table,field,field);
	}

	bool SelectDuplicates(SqlRecordset & recordset, const char * table, const char * field)
	{
		return Database.Execute(recordset,"select %s from %s group by %s having (count(%s) > 1)",field,table,field,field);
	}

	bool SelectRandom(SqlRecordset & recordset, const char * table)
	{
		return Database.Execute(recordset,"select * from %s order by random() limit 1",table);
	}

	bool SelectTop(SqlRecordset & recordset, const char * table, int count)
	{
		return Database.Execute(recordset,"select * from %d limit %d",table,count);
	}

	bool DeleteDuplicates(SqlRecordset & recordset, const char * table, const char * field)
	{

	}

	bool SelectNow(Time & time)
	{

		SqlRecordset recordset;
		Database.Execute(recordset,"select now()");
		time = recordset(0,0);
		return true;
	}

	int SelectCount(const char * table)
	{
		SqlRecordset recordset;
		if (Database.Execute(recordset, "select count() from %s",table))
		{	
			return recordset[0][0].Integer();
		}
		return 0;
	}

};

class SqlDriver
{
public:

	enum SqlTransaction
	{

		SQL_TRANSACTION_COMMIT		=(0),
		SQL_TRANSACTION_BEGIN		=(1),
		SQL_TRANSACTION_ROLLBACK	=(2),
	};

	virtual bool Open(SqlDatabase * database, const char * data, const char * user=0, const char * pass=0, const char * host=0, unsigned int port=0)=0;
	virtual bool Open(SqlDatabase * database, SqlStatement * statement)=0;
	virtual bool Open(SqlDatabase * database, SqlRecord * record)=0;

	virtual bool Close(SqlDatabase * database)=0;
	virtual bool Close(SqlDatabase * database, SqlStatement * statement) {statement->Database = 0; return true;}
	virtual bool Close(SqlDatabase * database, SqlRecord * record) {record->Database = 0; return true;};

	virtual int Execute(SqlDatabase * database, Sequence * query)=0;
	virtual int Execute(SqlDatabase * database, Sequence * query, SqlRecord * record)=0;
	virtual int Execute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset)=0;

	virtual int Execute(SqlDatabase * database, SqlStatement * statement)=0;
	virtual int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record)=0;
	virtual int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset)=0;

	virtual int Step(SqlDatabase * database, SqlRecord * record)=0;

	virtual int Id(SqlDatabase * database)=0;

	virtual bool Transaction(SqlDatabase * database, int transaction)=0;
	virtual bool IsEmpty(SqlDatabase * database)=0;

	template <typename _Key_, typename _Value_> 
	class SqlMap : public Reason::Structure::Map<_Key_,_Value_>
	{
	public:

		Reason::Platform::Mutex Mutex;

		Iterand< Mapped<_Key_,_Value_> > Insert(Iterand< Mapped<_Key_,_Value_> > iterand)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Insert(iterand);
		}

		Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Insert(key,value);
		}

		Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Insert(key);
		}

		Iterand< Mapped<_Key_,_Value_> > Select(Iterand< Mapped<_Key_,_Value_> > iterand)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Select(iterand);
		}

		Iterand< Mapped<_Key_,_Value_> > Select(typename Template<_Key_>::ConstantReference key)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Select(key);
		}

		Iterand< Mapped<_Key_,_Value_> > Update(Iterand< Mapped<_Key_,_Value_> > iterand)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Update(iterand);
		}

		Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Update(key,value);
		}

		Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Update(key);
		}

		Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Remove(key);
		}

		Iterand< Mapped<_Key_,_Value_> > Delete(Iterand< Mapped<_Key_,_Value_> > iterand)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Delete(iterand);
		}

		Iterand< Mapped<_Key_,_Value_> > Delete(typename Template<_Key_>::ConstantReference key)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Delete(key);
		}

		Iterand< Mapped<_Key_,_Value_> > Contains(Iterand< Mapped<_Key_,_Value_> > iterand)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Contains(iterand);
		}

		Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::Contains(key);
		}

		typename Template<_Value_>::Reference operator [] (typename Template<_Key_>::ConstantReference key)
		{
			Mutex::Auto mutex(Mutex);
			return Map<_Key_,_Value_>::operator [] (key);
		}

	};

};

class SqlTransaction
{
public:

	SqlDatabase & Database;

	SqlTransaction(SqlDatabase & database):Database(database)
	{
		Database.Transaction(SqlDriver::SQL_TRANSACTION_BEGIN);
	}

	~SqlTransaction()
	{
		Database.Transaction(SqlDriver::SQL_TRANSACTION_COMMIT);
	}

};

namespace Drivers {

#ifdef REASON_USING_POSTGRES

class Postgres : public SqlDriver
{
public:

	int Options;

	struct PostgreSqlRecord
	{
		int Index;
		int Rows;
		int Fields;
		PGresult * Result;

		PostgreSqlRecord():Index(0),Rows(0),Fields(0),Result(0) {}
	};

	SqlMap<SqlDatabase *, void *> Databases;
	SqlMap<SqlDatabase *, void *> Results;
	SqlMap<SqlStatement *, String> Statements;
	SqlMap<SqlRecord *, PostgreSqlRecord> Records;

	Postgres(int options=0);	
	~Postgres();

	bool Open(SqlDatabase * database, const char * data, const char * user=0, const char * pass=0, const char * host=0, unsigned int port=0);
	bool Open(SqlDatabase * database, SqlStatement * statement);
	bool Open(SqlDatabase * database, SqlRecord * record);

	bool Close(SqlDatabase * database);
	bool Close(SqlDatabase * database, SqlStatement * statement);
	bool Close(SqlDatabase * database, SqlRecord * record);

	int Execute(SqlDatabase * database, Sequence * query);
	int Execute(SqlDatabase * database, Sequence * query, SqlRecord * record);
	int Execute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset);

	int Execute(SqlDatabase * database, SqlStatement * statement);
	int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record);
	int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset);

	int Step(SqlDatabase * database, SqlRecord * record);

	int Id(SqlDatabase * database);

	bool Transaction(SqlDatabase * database, int transaction);
	bool IsEmpty(SqlDatabase * database);

private:

	bool Run(SqlDatabase * database, Sequence * query, int & status, int & rows, PGresult *& res);
	bool Run(SqlDatabase * database, SqlStatement * statement, int & status, int & rows, PGresult *& res);

	PGresult *& Result(SqlDatabase * database);
	PGresult *& Record(SqlDatabase * database);

};

#endif

#ifdef REASON_USING_SQLITE

class SqLite : public SqlDriver
{
public:

	static const char * Memory;
	static const char * Temporary;
	int Options;

	enum Option
	{
		SYNCHRONISE_OFF			=(1),
		SYNCHRONISE_NORMAL		=(1)<<1,
		SYNCHRONISE_FULL		=(1)<<2,
		PAGE_SIZE_1024			=(1)<<3,
		PAGE_SIZE_2048			=(1)<<4,
		PAGE_SIZE_4096			=(1)<<5,
		LOCKING_MODE_NORMAL		=(1)<<6,
		LOCKING_MODE_EXCLUSIVE	=(1)<<7,
		AUTO_VACUUM_ON			=(1)<<8,
		AUTO_VACUUM_OFF			=(1)<<9,
		CACHE_SIZE_1024			=(1)<<10,
		CACHE_SIZE_2048			=(1)<<11,
		CACHE_SIZE_4096			=(1)<<12,
		ENCODING_UTF8			=(1)<<13,
		ENCODING_UTF16			=(1)<<14,
	};

	class SqLiteMessage : public Condition  
	{
	public:

		enum Messages
		{
			MESSAGE_OPEN,
			MESSAGE_CLOSE,
			MESSAGE_EXECUTE,
			MESSAGE_STEP,
		};

		int Type;
		SqLite * Driver;
		SqlDatabase * Database;
		SqlStatement * Statement;
		SqlRecordset * Recordset;
		SqlRecord * Record;

		Reason::System::Sequence * Sequence;
		int Result;

		SqLiteMessage():Type(0),Driver(0),Database(0),Statement(0),Recordset(0),Record(0),Sequence(0),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database):Type(type),Driver(driver),Database(database),Statement(0),Recordset(0),Record(0),Sequence(0),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, Reason::System::Sequence * sequence):Type(type),Driver(driver),Database(database),Statement(0),Recordset(0),Record(0),Sequence(sequence),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, SqlRecordset * recordset):Type(type),Driver(driver),Database(database),Statement(0),Recordset(recordset),Record(0),Sequence(0),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, Reason::System::Sequence * sequence, SqlRecordset * recordset):Type(type),Driver(driver),Database(database),Statement(0),Recordset(recordset),Record(0),Sequence(sequence),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, SqlRecord * record):Type(type),Driver(driver),Database(database),Statement(0),Recordset(0),Record(record),Sequence(0),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, Reason::System::Sequence * sequence, SqlRecord * record):Type(type),Driver(driver),Database(database),Statement(0),Recordset(0),Record(record),Sequence(sequence),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, SqlStatement * statement):Type(type),Driver(driver),Database(database),Statement(statement),Recordset(0),Record(0),Sequence(0),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset):Type(type),Driver(driver),Database(database),Statement(statement),Recordset(recordset),Record(0),Sequence(0),Result(0) {}
		SqLiteMessage(int type, SqLite * driver, SqlDatabase * database, SqlStatement * statement, SqlRecord * record):Type(type),Driver(driver),Database(database),Statement(statement),Recordset(0),Record(record),Sequence(0),Result(0) {}

	};

	class SqLiteLibrary : private Thread
	{
	public:

		Reason::Structure::Stack<SqLiteMessage*> Messages;

		Locked Running;
		Condition Ready;

		void Process(SqLiteMessage * message);
		unsigned int Run(Thread::Pointer * pointer);
	};

#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteLibrary & Library();
#endif

	SqlMap<void *, void *> Databases;	
	SqlMap<void *, void *> Statements;
	SqlMap<void *, void *> Records;

	Map<void *, Reason::Platform::Mutex *> Mutexes;

	Reason::Platform::Mutex MutexesMutex;

	SqLite(int options=0):Options(options)

	{
	}

	~SqLite();

	bool Open(SqlDatabase * database, const char * data, const char * user=0, const char * pass=0, const char * host=0, unsigned int port=0);
	bool Open(SqlDatabase * database, SqlStatement * statement);
	bool Open(SqlDatabase * database, SqlRecord * record) {return false;}

	bool Close(SqlDatabase * database);
	bool Close(SqlDatabase * database, SqlStatement * statement);
	bool Close(SqlDatabase * database, SqlRecord * record);

	int Execute(SqlDatabase * database, Sequence * query);
	int Execute(SqlDatabase * database, Sequence * query, SqlRecord * record);
	int Execute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset);

	int Execute(SqlDatabase * database, SqlStatement * statement);
	int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record);
	int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset);

	int Step(SqlDatabase * database, SqlRecord * record);

	int Id(SqlDatabase * database);

	bool Transaction(SqlDatabase * database, int transaction);
	bool IsEmpty(SqlDatabase * database);

protected:

	bool ProcessOpen(SqlDatabase * database, const char * data, const char * user=0, const char * pass=0, const char * host=0, unsigned int port=0);
	bool ProcessOpen(SqlDatabase * database, SqlStatement * statement);
	bool ProcessOpen(SqlDatabase * database, SqlRecord * record) {return false;}

	bool ProcessClose(SqlDatabase * database);
	bool ProcessClose(SqlDatabase * database, SqlStatement * statement);
	bool ProcessClose(SqlDatabase * database, SqlRecord * record);

	int ProcessExecute(SqlDatabase * database, Sequence * query);
	int ProcessExecute(SqlDatabase * database, Sequence * query, SqlRecord * record);
	int ProcessExecute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset);

	int ProcessExecute(SqlDatabase * database, SqlStatement * statement);
	int ProcessExecute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record);
	int ProcessExecute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset);

	int ProcessStep(SqlDatabase * database, SqlRecord * record);

	bool ProcessTransaction(SqlDatabase * database, int transaction);
	bool ProcessIsEmpty(SqlDatabase * database);

	sqlite3 *& Database(SqlDatabase * database);
	sqlite3_stmt *& Statement(SqlStatement * statemnet);

	void ProcessError(SqlDatabase * database, char * msg);
	void ProcessError(SqlDatabase * database, sqlite3 *db);
	void ProcessError(SqlDatabase * database, sqlite3 * db, sqlite3_stmt * st, int state);

	int Run(SqlDatabase * database, SqlStatement * statement, sqlite3 *& db, sqlite3_stmt *& st);
	int Changes(sqlite3 * db, Sequence * query, int count);

	static int Count(void * argument, int columns, char **values, char **fields);
	static int Record(void * argument, int columns, char **values, char **fields);
	static int Busy(void * arg, int count);

};

#endif

#ifdef REASON_USING_MYSQL

class MySql : public SqlDriver
{
public:

	SqlMap<SqlDatabase *, MYSQL *> Databases;
	SqlMap<SqlDatabase *, MYSQL_RES *> Results;
	SqlMap<SqlStatement *, MYSQL_STMT *> Statements;
	SqlMap<SqlRecord *, MYSQL_STMT *> Records;

	MySql() {}
	~MySql();

	bool Open(SqlDatabase * database, const char * data, const char * user=0, const char * pass=0, const char * host=0, unsigned int port=0);
	bool Open(SqlDatabase * database, SqlStatement * statement);
	bool Open(SqlDatabase * database, SqlRecord * record) {return false;}

	bool Close(SqlDatabase * database);
	bool Close(SqlDatabase * database, SqlStatement * statement);
	bool Close(SqlDatabase * database, SqlRecord * record);

	int Execute(SqlDatabase * database, Sequence * query);
	int Execute(SqlDatabase * database, Sequence * query, SqlRecord * record);
	int Execute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset);

	int Execute(SqlDatabase * database, SqlStatement * statement);
	int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record);
	int Execute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset);

	int Step(SqlDatabase * database, SqlRecord * record);

	bool Transaction(SqlDatabase * database, int transaction);

	bool IsEmpty(SqlDatabase * database);

	int Id(SqlDatabase * database);

protected:

	int BindFieldLength(MYSQL_FIELD field);
	Superstring BindField(MYSQL_BIND bind, char * data, int size);

public:

	class MySqlLibrary
	{
	public:

		MySqlLibrary();
		~MySqlLibrary();
	};

	static MySqlLibrary Library;

};

#endif

#ifdef REASON_USING_ODBC

class Odbc : public SqlDriver
{

};

#endif

}

}}}

#endif

