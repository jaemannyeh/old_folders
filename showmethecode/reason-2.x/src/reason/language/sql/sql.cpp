
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



#include "reason/reason.h"
#include "reason/language/sql/sql.h"
#include "reason/system/format.h"
#include "reason/network/socket.h"

using namespace Reason::System;
using namespace Reason::Network;
using namespace Reason::Language::Sql;
using namespace Reason::Language::Sql::Drivers;

Identity SqlCursor::Instance;

String SqlRecordset::Null;

SqlCursor SqlRecordset::Select(const SqlCursor & cursor, char * data, int size, bool caseless)
{

	int row = cursor.Row;
	int column = cursor.Column;
	if (column > 0 || row > 0) ++column;

	for (;((row*Fields.Size)+(Fields.Size-column))<(this->Length()*Fields.Size);++column)
	{
		if (column%Fields.Size==0)
		{
			column=0;
			++row;
		}

		if (row < this->Length() && (*this)[row][column].Contains(data,size,caseless))
		{
			return SqlCursor(row,column,&(*this)[row][column]);
		}
	}

	return SqlCursor();
}

SqlDatabase::SqlDatabase(SqlDriver * driver):Driver(driver),Error(false) 
{
}

SqlDatabase::~SqlDatabase()
{
	Close();
}

bool SqlDatabase::Open(const char * data, const char * user, const char * pass, const char * host, unsigned int port)
{
	Error = false; Message = "";
	return Driver->Open(this,data,user,pass,host,port);
}

bool SqlDatabase::Close()
{
	Error = false; Message = "";
	return Driver->Close(this);
}

int SqlDatabase::Execute(SqlRecordset & recordset, const Sequence & query)
{
	Error = false; Message = "";
    recordset.Reset();

	return Driver->Execute(this,(Sequence*)&query,&recordset);
}

int SqlDatabase::Execute(SqlRecordset & recordset, const char * query, ...)
{

	va_list va;

	va_start(va, query);
	String string;
	string.Allocate(String::Length(query)*1.25);	
	Formatter::FormatVa(string,query,va);
	va_end(va);

	return Execute(recordset,string);
}

int SqlDatabase::Execute(SqlRecord & record, const Sequence & query)
{
	Error = false; Message = "";

	if (record.Database) record.Database->Driver->Close(record.Database,&record);
	if (record.Database != this) record.Database = this;
	record.Reset();

	return Driver->Execute(this,(Sequence*)&query,&record);
}

int SqlDatabase::Execute(SqlRecord & record, const char * query, ...)
{

	va_list va;

	va_start(va, query);

	String string = SqlFormatter::FormatVa(query, va);
	va_end(va);

	return Execute(record,string);
}

int SqlDatabase::Execute(const Sequence & query)
{
	Error = false; Message = "";
	return Driver->Execute(this,(Sequence*)&query);
}

int SqlDatabase::Execute(const char * query, ...)
{

	va_list va;

	va_start(va, query);

	String string = SqlFormatter::FormatVa(query, va);
	va_end(va);

	return Execute(string);
}

int SqlDatabase::Id()
{
	Error = false; Message = "";
	return Driver->Id(this);
}

bool SqlDatabase::Transaction(int transaction)
{
	Error = false; Message = "";
	return Driver->Transaction(this,transaction);
}

bool SqlDatabase::Begin() 
{
	return Transaction(SqlDriver::SQL_TRANSACTION_BEGIN);
}

bool SqlDatabase::Commit() 
{
	return Transaction(SqlDriver::SQL_TRANSACTION_COMMIT);
}

bool SqlDatabase::Rollback() 
{
	return Transaction(SqlDriver::SQL_TRANSACTION_ROLLBACK);
}

bool SqlDatabase::IsEmpty()
{
	return Driver->IsEmpty(this);
}

SqlStatement::SqlStatement(SqlDatabase & database):Database(&database),Param(-1),State(0)
{
	if (Database) Database->Driver->Open(Database,this);
}

SqlStatement::SqlStatement():Database(0),Param(-1),State(0)
{
}

SqlStatement::~SqlStatement() 
{
	if (Database) Database->Driver->Close(Database,this);
}

bool SqlStatement::Prepare(SqlDatabase & database) 
{
	if (Database) Database->Driver->Close(Database,this);
	Database=&database;
	if (Database) Database->Driver->Open(Database,this);
	Reset();
	return true;
}

bool SqlStatement::Prepare(SqlDatabase & database, char * data, int size) 
{
	if (Database) Database->Driver->Close(Database,this);
	Database=&database;
	Construct(data,size);
	if (Database) Database->Driver->Open(Database,this);
	Reset();
	return true;
}

bool SqlStatement::Prepare(char * data, int size) 
{
	if (Database) Database->Driver->Close(Database,this);
	Construct(data,size);
	if (Database) Database->Driver->Open(Database,this);
	Reset();
	return true;
}

void SqlStatement::Reset()
{
	Param = -1;
	State = 0;
	Parameters.Release();
	State = SQL_STATE_PREPARED;
}

int SqlStatement::Execute() 
{
	if (!Database) return 0;
	State = SQL_STATE_EXECUTED;
	Database->Error = false; Database->Message = "";
	return Database->Driver->Execute(Database,this);
}

int SqlStatement::Execute(SqlRecordset & recordset) 
{
	if (!Database) return 0;
	State = SQL_STATE_EXECUTED;
	Database->Error = false; Database->Message = "";
    recordset.Reset();

	return Database->Driver->Execute(Database,this,&recordset);
}

int SqlStatement::Execute(SqlRecord & record) 
{
	if (!Database) return 0;
	State = SQL_STATE_EXECUTED;
	Database->Error = false; Database->Message = "";

	if (record.Database) record.Database->Driver->Close(record.Database,&record);
	if (record.Database != Database) record.Database = Database;
	record.Reset();

	return Database->Driver->Execute(Database,this,&record);
}

SqlStatement & SqlStatement::Bind(double value)
{
	if (State == SQL_STATE_EXECUTED) Reset();
	Float(++Param,value);return *this;
}

SqlStatement & SqlStatement::Bind(int value)
{
	if (State == SQL_STATE_EXECUTED) Reset();
	Integer(++Param,value);return *this;
}

SqlStatement & SqlStatement::Bind(const Sequence & value)
{
	if (State == SQL_STATE_EXECUTED) Reset();
	Text(++Param,value);return *this;
}

SqlStatement & SqlStatement::Bind(const char * value)
{
	if (State == SQL_STATE_EXECUTED) Reset();
	Text(++Param,value);return *this;
}

SqlStatement & SqlStatement::Bind() 
{
	if (State == SQL_STATE_EXECUTED) Reset();
	Null(++Param);return *this;
}

bool SqlStatement::Integer(int param, int value)
{

	Reason::System::Superstring superstring((char*)&value,sizeof(int));

	Parameters.Insert(param, Pair<int,String>(SQL_TYPE_INTEGER,superstring));
	return true;
}

bool SqlStatement::Float(int param, double value)
{
	Reason::System::Superstring superstring((char*)&value,sizeof(double));
	Parameters.Insert(param, Pair<int,String>(SQL_TYPE_FLOAT,superstring));
	return true;
}

bool SqlStatement::Text(int param, const Sequence & value)
{
	Reason::System::Superstring superstring(value);
	int type = (superstring.Size>65535)?SQL_TYPE_BLOB:SQL_TYPE_TEXT;
	Parameters.Insert(param, Pair<int,String>(type,superstring));
	return true;
}

bool SqlStatement::Text(int param, const char * value)
{
	Reason::System::Superstring superstring(value);
	int type = (superstring.Size>65535)?SQL_TYPE_BLOB:SQL_TYPE_TEXT;
	Parameters.Insert(param, Pair<int,String>(type,superstring));
	return true;
}

bool SqlStatement::Null(int param)
{
	Parameters.Insert(param, Pair<int,String>(SQL_TYPE_NULL,String()));
	return true;
}

String SqlRecord::Null;

SqlRecord::SqlRecord():Database(0) 
{

}

SqlRecord::~SqlRecord()
{
	if (Database) Database->Driver->Close(Database,this);
}

bool SqlRecord::Step()
{
	return Database && Database->Driver->Step(Database,this);
}

#ifdef REASON_USING_POSTGRES

Postgres::Postgres(int options):
	Options(options)
{

}

Postgres::~Postgres()
{
	{
		SqlMap<SqlDatabase *, void *>::Iterator iterator = Results.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			PGresult * res = (PGresult*) iterator().Value();
			if (res) PQclear(res);
		}
	}

	{
		SqlMap<SqlDatabase *, void *>::Iterator iterator = Databases.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			PGconn * conn = (PGconn*) iterator().Value();
			if (conn) PQfinish(conn);
		}
	}

}

bool Postgres::Open(SqlDatabase * database, const char * data, const char * user, const char * pass, const char * host, unsigned int port)
{

	String info;

	info.Format("hostaddr='%s',port='%d',dbname='%s',user='%s',password='%s'",SocketLibrary::ResolveIp(host),port,data,user,pass);

	String ports((int)port);
	PGconn * conn = PQsetdbLogin(host,ports.Print(),"","",data,user,pass);

	int status = PQstatus(conn);

	if (status != CONNECTION_OK)
	{
		switch(status)
		{
		case CONNECTION_BAD:
			OutputError("Postgres::Open - Error, Bad connection.\n");
			break;

		}

		return 0;
	}

	Databases.Insert(database,conn);

	return conn;
}

bool Postgres::Open(SqlDatabase * database, SqlStatement * statement)
{
	if (Statements.Select(statement))
		Close(database,statement);

	PGconn * conn = (PGconn *)Databases[database];

	String query;

	String key;
	key << "statement_" << PQtty(conn) << (int)conn << (int)statement;

	int count = 0;

	{

		query << "PREPARE " << key << "(";	
		Reason::Structure::Map< int,Reason::Structure::Pair<int,String> >::Iterator iterator = statement->Parameters.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			if (count > 0)
				query << ", ";

			int type = iterator().Value().First();

			switch (type)
			{
			case SqlStatement::SQL_TYPE_INTEGER: query << "int"; break;			
			case SqlStatement::SQL_TYPE_FLOAT: query << "real"; break;
			case SqlStatement::SQL_TYPE_TEXT: query << "text"; break;
			case SqlStatement::SQL_TYPE_BLOB: query << "bytea"; break;

			case SqlStatement::SQL_TYPE_NULL: query << "bit"; break;
			}

			++count;
		}

		query << ") AS ";
	}

	{

		count = 0;
		StringParser parser(*statement);
		while (!parser.Eof())
		{
			if (parser.Is("?"))
				query << "$" << ++count;
			else
				query << *parser.At();

			parser.Next();
		}
	}

	query.Trim();

	int rows = 0;
	int status = 0;
	PGresult *& res = (PGresult*&) Results[database];
	if (res) PQclear(res);

	Run(database,&query,status,rows,res);

	if (status == PGRES_COMMAND_OK)
	{
		Statements.Insert(statement,key);
		return true;
	}

	return false;
}

bool Postgres::Open(SqlDatabase * database, SqlRecord * record) 
{
	return true;
}

bool Postgres::Close(SqlDatabase * database)
{

	Iterand< Mapped<SqlDatabase*,void*> > iterand = Databases.Select(database);
	if (iterand)
	{
		PGconn * conn = (PGconn*) iterand().Value();
		PQfinish(conn);
		conn = 0;

		Databases.Remove(database);
	}

	return true;
}

bool Postgres::Close(SqlDatabase * database, SqlStatement * statement)
{
	PGconn * conn = (PGconn*)Databases[database];

	String key;
	key << "statement_" << PQtty(conn) << (int)conn << (int)statement;

	String query;
	query << "DEALLOCATE " << key << ";";

	int status = 0;
	int rows = 0;
	PGresult *& res = Result(database);
	Run(database,&query,status,rows,res);

	if (res)
	{
		Statements.Delete(statement);
		return true;
	}
	else
	{
		OutputError("Postgres::Close - Error, Failed to deallocate prepared statement\n");
	}

	return false;
}

bool Postgres::Close(SqlDatabase * database, SqlRecord * record)
{
	SqlDriver::Close(database,record);

	record->Reset();

	Iterand< Mapped<SqlRecord*,PostgreSqlRecord> > iterand = Records.Select(record);
	if (iterand)
	{
		PGresult * res = iterand().Value().Result;
		if (res)
		{
			PQclear(res);
			res = 0;
		}

		Records.Remove(record);
	}

	return false;
}

PGresult *& Postgres::Result(SqlDatabase * database)
{
	Iterand< Mapped<SqlDatabase*,void*> > iterand = Results.Select(database);
	if (!iterand)
		iterand = Results.Insert(database,0);

    PGresult *& res = (PGresult*&) iterand().Value();

	if (res) 
	{
		PQclear(res);
		res = 0;
	}

	return res;
}

bool Postgres::Run(SqlDatabase * database, Sequence * query, int & status, int & rows, PGresult *& res)
{

	status = 0;
	rows = 0;

	PGconn * conn = (PGconn*)Databases[database];
	status = PQstatus(conn);

	if (conn && status == CONNECTION_OK)
	{

		res = PQexec(conn, query->Print());
		status = PQresultStatus(res);

		if (status == PGRES_COMMAND_OK)
		{

			char * cmd = PQcmdTuples(res);
			rows = String(cmd).Integer();
		}
		else
		if (status == PGRES_TUPLES_OK)
		{
			rows = PQntuples(res);
		}
		else
		{

			database->Error = true;
			database->Message = PQresultErrorMessage(res);
			OutputError("Postgres::Run - Error, %s\n",database->Message.Print());

			PQclear(res);
			res = 0;
			return false;
		}
	}

	return true;
}

int Postgres::Execute(SqlDatabase * database, Sequence * query)
{
	int status = 0;
	int rows = 0;
	PGresult *& res = Result(database);
	Run(database,query,status,rows,res);

	return rows;
}

int Postgres::Execute(SqlDatabase * database, Sequence * query, SqlRecord * record)
{

	int status = 0;
	int rows = 0;
	PGresult *& res = Result(database);
	Run(database,query,status,rows,res);

	if (res)
	{

		PostgreSqlRecord psr;
		psr.Index = 0;

		psr.Result = res;
		Results.Remove(database);

		int fields = 0;

		if (status == PGRES_TUPLES_OK)
		{
			fields = PQnfields(res);

			record->Index = 0;

			for (int f=0;f<fields;++f)
			{
				record->Fields.Append(PQfname(res,f));

				int len = PQgetlength(res,psr.Index,f);
				char * val = PQgetvalue(res,psr.Index,f);
				record->Append(Superstring(val,len));
			}

			psr.Rows = rows;
			psr.Fields = fields;
			Records.Insert(record,psr);
		}
	}

	return rows;
}

int Postgres::Execute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset)
{
	recordset->Release();

	int status = 0;
	int rows = 0;
	PGresult *& res = Result(database);
	Run(database,query,status,rows,res);

	if (res)
	{
		int fields = 0;

		if (status == PGRES_TUPLES_OK)
		{
			fields = PQnfields(res);

			for (int f=0;f<fields;++f)
				recordset->Fields.Append(PQfname(res,f));

			for (int r=0;r<rows;++r)
			{
				recordset->Append(Array<String>());
				for (int f=0;f<fields;++f)
				{

					int len = PQgetlength(res,r,f);
					char * val = PQgetvalue(res,r,f);
					(*recordset)[r].Append(Superstring(val,len));
				}
			}
		}
	}

	return rows;
}

bool Postgres::Run(SqlDatabase * database, SqlStatement * statement, int & status, int & rows, PGresult *& res)
{

	Iterand< Mapped<SqlStatement *, String> > iterand;

	iterand = Statements.Select(statement);
	if (!iterand) 
	{
		Open(database,statement);
		iterand = Statements.Select(statement);
	}

	if (iterand)
	{
		String key = iterand().Value();

		int count = 0;
		String query;

		query << "EXECUTE " << key << "(";
		Reason::Structure::Map< int,Reason::Structure::Pair<int,String> >::Iterator iterator = statement->Parameters.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			if (count > 0)
				query << ", ";

			int type = iterator().Value().First();

			switch (type)
			{
			case SqlStatement::SQL_TYPE_TEXT: 
			case SqlStatement::SQL_TYPE_BLOB: query << "'" << iterator().Value().Second() << "'"; break;
			case SqlStatement::SQL_TYPE_FLOAT: 
				{
					OutputAssert(iterator().Value().Second().Size == sizeof(double));
					double value = *((double*)iterator().Value().Second().Data);
					query << value;
				}
				break;
			case SqlStatement::SQL_TYPE_INTEGER:
				{
					OutputAssert(iterator().Value().Second().Size == sizeof(int));
					int value = *((int*)iterator().Value().Second().Data);
					query << value;
				}
				break;

			default: query << ""; break;
			}

			++count;
		}
		query << ");";

		return Run(database,&query,status,rows,res);
	}

	return false;
}

int Postgres::Execute(SqlDatabase * database, SqlStatement * statement)
{
	int rows = 0;
	int status = 0;
	PGresult *& res = Result(database);

	Run(database,statement,status,rows,res);
	return rows;
}

int Postgres::Execute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record)
{
	record->Release();

	int status = 0;
	int rows = 0;
	PGresult *& res = Result(database);
	Run(database,statement,status,rows,res);

	if (res)
	{
		PostgreSqlRecord psr;
		psr.Index = 0;
		psr.Result = res;

		Results.Remove(database);

		int fields = 0;

		if (status == PGRES_TUPLES_OK)
		{
			fields = PQnfields(res);

			record->Index = 0;

			for (int f=0;f<fields;++f)
			{
				record->Fields.Append(PQfname(res,f));

				int len = PQgetlength(res,psr.Index,f);
				char * val = PQgetvalue(res,psr.Index,f);
				record->Append(Superstring(val,len));
			}

			psr.Rows = rows;
			psr.Fields = fields;
			Records.Insert(record,psr);
		}
	}

	return rows;
}

int Postgres::Execute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset)
{
	recordset->Release();

	int status = 0;
	int rows = 0;
	PGresult *& res = Result(database);

	Run(database,statement,status,rows,res);

	if (res)
	{
		int fields = 0;

		if (status == PGRES_TUPLES_OK)
		{
			fields = PQnfields(res);

			for (int f=0;f<fields;++f)
				recordset->Fields.Append(PQfname(res,f));

			for (int r=0;r<rows;++r)
			{
				recordset->Append(Array<String>());
				for (int f=0;f<fields;++f)
				{

					int len = PQgetlength(res,r,f);
					char * val = PQgetvalue(res,r,f);
					(*recordset)[r].Append(Superstring(val,len));
				}
			}
		}
	}

	return rows;
}

int Postgres::Step(SqlDatabase * database, SqlRecord * record)
{
	record->Release();

	PostgreSqlRecord & psr = Records[record];
	PGresult * res = psr.Result;

	if (res)
	{
		psr.Index++;

		if (psr.Index < psr.Rows)
		{
			int rows = psr.Rows;
			int fields = psr.Fields;

			record->Index ++;

			for (int f=0;f<fields;++f)
			{

				int len = PQgetlength(res,psr.Index,f);
				char * val = PQgetvalue(res,psr.Index,f);
				record->Append(Superstring(val,len));
			}

			return 1;
		}
	}

	return 0;
}

int Postgres::Id(SqlDatabase * database)
{

	String query = "select lastval()";
	SqlRecord record;
	if (Execute(database,&query,&record) || !database->Error)
		return record[0].Integer();
	else
		return 0;
}

bool Postgres::Transaction(SqlDatabase * database, int transaction)
{
	String string;
	switch(transaction)
	{
	case SqlDriver::SQL_TRANSACTION_BEGIN:
		{
			string = "begin";
			return Execute(database,&string) || !database->Error;	
		}
	case SqlDriver::SQL_TRANSACTION_COMMIT:
		{
			string = "commit";
			return Execute(database,&string) || !database->Error;
		}
	case SqlDriver::SQL_TRANSACTION_ROLLBACK:
		{
			string = "rollback";
			return Execute(database,&string) || !database->Error;
		}
	}

	return false;
}

bool Postgres::IsEmpty(SqlDatabase * database)
{
	return false;
}

#endif

#ifdef REASON_USING_SQLITE

#ifdef SQLITE_DRIVER_LIBRARY

SqLite::SqLiteLibrary & SqLite::Library()
{
	static SqLiteLibrary library;
	return library;
}
#endif

void SqLite::SqLiteLibrary::Process(SqLite::SqLiteMessage * message)
{
	Mutex.Enter();
	Messages.Push(message);
	Mutex.Leave();

	if (!Running.Wait() && !Running.Owned())
	{
		Running.Lock();
		Start();

	}

	Ready.Signal();

	switch(message->Type)
	{
	case SqLiteMessage::MESSAGE_OPEN:
		if (message->Sequence)
			message->Wait();
		break;
	case SqLiteMessage::MESSAGE_CLOSE:
		break;
	default:
		message->Wait();
	}

}

unsigned int SqLite::SqLiteLibrary::Run(Thread::Pointer * pointer)
{
	Ready.Wait();

	while(Running.Wait())
	{
		Mutex.Enter();
		while (Messages.Count == 0)
		{			
			Mutex.Leave();
			Ready.Wait();
			Mutex.Enter();
		}

		SqLiteMessage * message = Messages.Peek();
		Messages.Pop();
		Mutex.Leave();

		switch(message->Type)
		{
		case SqLiteMessage::MESSAGE_OPEN:
			if (message->Statement)
			{
				message->Result = message->Driver->ProcessOpen(message->Database,message->Statement);
				delete message;
				continue;
			}
			else
			if (message->Record)
			{
				message->Result = message->Driver->ProcessOpen(message->Database,message->Record);
				delete message;
				continue;
			}
			else
			{
				message->Result = message->Driver->ProcessOpen(message->Database,message->Sequence->Data);
			}
		break;
		case SqLiteMessage::MESSAGE_CLOSE:
			if (message->Statement)
			{
				message->Result = message->Driver->ProcessClose(message->Database,message->Statement);
				delete message;
				continue;

			}
			else
			if (message->Record)
			{
				message->Result = message->Driver->ProcessClose(message->Database,message->Record);
				delete message;
				continue;				
			}
			else
			{
				message->Result = message->Driver->ProcessClose(message->Database);
				delete message;
				continue;				
			}
		break;
		case SqLiteMessage::MESSAGE_EXECUTE:
			if (message->Recordset)
			{
				if (message->Statement)
					message->Result = message->Driver->ProcessExecute(message->Database,message->Statement,message->Recordset);
				else
					message->Result = message->Driver->ProcessExecute(message->Database,message->Sequence,message->Recordset);
			}
			else
			if (message->Record)
			{
				if (message->Statement)
					message->Result = message->Driver->ProcessExecute(message->Database,message->Statement,message->Record);
				else
					message->Result = message->Driver->ProcessExecute(message->Database,message->Sequence,message->Record);			
			}
			else
			{
				if (message->Statement)
					message->Result = message->Driver->ProcessExecute(message->Database,message->Statement);
				else
					message->Result = message->Driver->ProcessExecute(message->Database,message->Sequence);				
			}
		break;
		case SqLiteMessage::MESSAGE_STEP:
			message->Result = message->Driver->ProcessStep(message->Database,message->Record);
		default:
			OutputError("SqLiteLibrary - Invalid message type.\n");
		}

		message->Signal();
	}

	return 0;
}

const char * SqLite::Memory = ":memory:";
const char * SqLite::Temporary = "";

SqLite::~SqLite()
{

	{
		Mutexes.DestroyValues();
	}

	{

		Map<void *, void *>::Iterator iterator = Statements.Iterate(); 
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			sqlite3_stmt * st = (sqlite3_stmt*) iterator().Value();
			if (st) sqlite3_finalize(st);
		}
	}

	{

		Map<void *, void *>::Iterator iterator = Records.Iterate(); 
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			sqlite3_stmt * st = (sqlite3_stmt*) iterator().Value();
			if (st) sqlite3_finalize(st);
		}
	}

	{

		Map<void *, void *>::Iterator iterator = Databases.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			sqlite3 * db = (sqlite3*) iterator().Value();
			if (db) sqlite3_close(db);
		}
	}

}

bool SqLite::Open(SqlDatabase * database, const char * data, const char * user, const char * pass, const char * host, unsigned int port)
{
#ifdef SQLITE_DRIVER_LIBRARY
	String string(data);
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_OPEN,this,database,&string);
	Library().Process(message);
	int result = message->Result;
	delete message;
	return result;
#else
	return ProcessOpen(database,data,user,pass,host,port);
#endif
}

bool SqLite::ProcessOpen(SqlDatabase * database, const char * data, const char * user, const char * pass, const char * host, unsigned int port)
{

	Mutex::Auto mutexAuto;

	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	if (!mutex) 
	{
		Mutexes.Insert((int*)database,(mutex = new Mutex()));
		mutexAuto = mutex;
	}

	MutexesMutex.Leave();

	if (!mutexAuto.Mutex)
		mutexAuto = mutex;

	sqlite3 * db = (sqlite3*)Databases[(int*)database];

	{

		if ( db != 0 && sqlite3_close(db)!=SQLITE_OK )
		{
			ProcessError(database,db);
			return false;
		}
		else
		if( sqlite3_open(data, &db)!=SQLITE_OK )
		{
			ProcessError(database,db);
			sqlite3_close(db);
			return false;
		}

		Databases.Update((int*)database,(int*)db);

	}

	sqlite3_busy_handler(db,Busy,db);

	{
		String sql;

		if (Options&ENCODING_UTF16)
			sql << "PRAGMA encoding=\"UTF-16\";";
		else

			sql << "PRAGMA encoding=\"UTF-8\";";

		if (Options&PAGE_SIZE_1024)
			sql << "PRAGMA page_size=1024;";
		if (Options&PAGE_SIZE_2048)
			sql << "PRAGMA page_size=2048;";
		else

			sql << "PRAGMA page_size=4096;";

		if (Options&CACHE_SIZE_1024)
			sql << "PRAGMA cache_size=1024;";
		if (Options&CACHE_SIZE_2048)
			sql << "PRAGMA cache_size=2048;";
		else
		if (Options&CACHE_SIZE_4096)
			sql << "PRAGMA cache_size=4096;";

		if (Options&AUTO_VACUUM_ON)
			sql << "PRAGMA auto_vacuum=1;";
		if (Options&AUTO_VACUUM_OFF)
			sql << "PRAGMA auto_vacuum=0;";

		if (Options&SYNCHRONISE_OFF)
			sql << "PRAGMA synchronous=OFF;";
		else
		if (Options&SYNCHRONISE_NORMAL)
			sql << "PRAGMA synchronous=NORMAL;";
		if (Options&SYNCHRONISE_FULL)
			sql << "PRAGMA synchronous=FULL;";

		if (Options&LOCKING_MODE_NORMAL)
			sql << "PRAGMA locking_mode=NORMAL;";
		else
		if (Options&LOCKING_MODE_EXCLUSIVE)
			sql << "PRAGMA locking_mode=EXCLUSIVE;";

		if (!sql.IsEmpty())
		{
			ProcessExecute(database,&sql);
			sql.Release();
		}

	}

	return true;
}

bool SqLite::Open(SqlDatabase * database, SqlStatement * statement)
{

	return true;
}

bool SqLite::ProcessOpen(SqlDatabase * database, SqlStatement * statement)
{

	return true;
}

bool SqLite::Close(SqlDatabase * database)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_CLOSE,this,database);
	Library().Process(message);

	return true;
#else
	return ProcessClose(database);
#endif
}

bool SqLite::ProcessClose(SqlDatabase * database)
{

	sqlite3 * db = 0;

	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(void*)database];
	Mutexes.Remove((void*)database);

	Iterand< Mapped<void *,void *> > iterand = Databases.Select((void*)database);
	if (iterand) 
	{
		db = (sqlite3*)iterand().Value();

		Databases.Remove((int*)database);
	}

	MutexesMutex.Leave();

	if (mutex) mutex->Enter();

	if (db)
	{

		if ( db != 0 && sqlite3_close(db) != SQLITE_OK )
		{
			ProcessError(database,db);

			OutputError("SqLite::ProcessClose - Database could not be closed.\n");
			OutputAssert(false);

			Databases.Insert((int*)database,(int*)db);

			if (mutex)
			{
				MutexesMutex.Enter();
				Mutexes.Insert((int*)database,mutex);
				MutexesMutex.Leave();				
			}

			if (mutex) mutex->Leave();

			return false;
		}		
	}

	if (mutex) mutex->Leave();

	if (mutex) delete mutex;

	return true;	
}

bool SqLite::Close(SqlDatabase * database, SqlStatement * statement)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_CLOSE,this,database,statement);
	Library().Process(message);

	return true;
#else

	return ProcessClose(database,statement);
#endif

}

bool SqLite::ProcessClose(SqlDatabase * database, SqlStatement * statement)
{

	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(void*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	Iterand< Mapped<void *,void *> > iterand = Statements.Select((void*)statement);
	if (iterand)
	{
		sqlite3_stmt * st = (sqlite3_stmt*)iterand().Value();
		if (st) sqlite3_finalize(st);

		Statements.Remove((void*)statement);
	}
	return true;
}

bool SqLite::Close(SqlDatabase * database, SqlRecord * record)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_CLOSE,this,database,record);
	Library().Process(message);

	return true;

#else

	return ProcessClose(database,record);

#endif
}

bool SqLite::ProcessClose(SqlDatabase * database, SqlRecord * record)
{

	SqlDriver::Close(database,record);

	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	record->Reset();

	Iterand< Mapped<void *,void *> > iterand = Records.Select((int*)record);
	if (iterand)
	{
		sqlite3_stmt * st = (sqlite3_stmt*)iterand().Value();
		if (st) sqlite3_finalize(st);

		Records.Remove((void*)record);

		return true;
	}

	return false;
}

int SqLite::Execute(SqlDatabase * database, Sequence * query)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_EXECUTE,this,database,query);
	Library().Process(message);
	int result = message->Result;
	delete message;
	return result;
#else
	return ProcessExecute(database,query);
#endif
}

int SqLite::ProcessExecute(SqlDatabase * database, Sequence * query)
{
	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	String string = Superstring::Literal(*query);
	string.Trim();

	sqlite3 * db = (sqlite3*)Databases[(int*)database];

	int count=0;

	char * msg=0;	
	if( (db == 0) || sqlite3_exec(db, string.Data, Count, &count, &msg)!=SQLITE_OK )
	{
		if (db)
		{
			ProcessError(database,msg);
		}
		else
		{
			database->Error = true;
			OutputError("SqLite::Execute - Error, database not opened.\n");
		}
		return 0;
	}

	return Changes(db,query,count);
}

int SqLite::Execute(SqlDatabase * database, Sequence * query, SqlRecord * record)
{
#ifdef SQLITE_DRIVER_LIBRARY

	return ProcessExecute(database,query,record);
#else

	return ProcessExecute(database,query,record);
#endif
}

int SqLite::ProcessExecute(SqlDatabase * database, Sequence * query, SqlRecord * record)
{

	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	SqlStatement statement(*database);
	statement.Prepare(*query);

	return ProcessExecute(database,&statement,record);
}

int SqLite::Execute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_EXECUTE,this,database,query,recordset);
	Library().Process(message);
	int result = message->Result;
	delete message;
	return result;
#else
	return ProcessExecute(database,query,recordset);
#endif
}

int SqLite::ProcessExecute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset)
{

	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	String string = Superstring::Literal(*query);
	string.Trim();

	recordset->Release();

	SqlStatement statement(*database);
	statement.Prepare(*query);
	return ProcessExecute(database,&statement,recordset);
}

int SqLite::Run(SqlDatabase * database, SqlStatement * statement, sqlite3 *& db, sqlite3_stmt *& st)
{
	if (!db) return 0;
	if (statement->IsEmpty()) return 0;

	if (st == 0)
	{

		if (sqlite3_prepare(db, statement->Data, statement->Size, &st, 0)!=0)
		{
			ProcessError(database,db);
			return 0;
		}

	}
	else
	{
		sqlite3_clear_bindings(st);
		sqlite3_reset(st);
	}

	if (st)
	{
		int parameters = sqlite3_bind_parameter_count(st);

		if (statement->Parameters.Count > 0)
		{
			int index=0;
			Reason::Structure::Map< int,Reason::Structure::Pair<int,String> >::Iterator iterator = statement->Parameters.Iterate();
			for (iterator.Forward();iterator.Has();iterator.Move())
			{	
				int ok=0;
				int type = iterator().Value().First();
				switch (type)
				{
				case SqlStatement::SQL_TYPE_INTEGER:
					ok=sqlite3_bind_int(st,index+1,*(int*) iterator().Value().Second().Data ); break;
				case SqlStatement::SQL_TYPE_FLOAT: 
					ok=sqlite3_bind_double(st,index+1,*(double*) iterator().Value().Second().Data ); break;
				case SqlStatement::SQL_TYPE_TEXT:
				case SqlStatement::SQL_TYPE_BLOB:
					ok=sqlite3_bind_text(st,index+1,iterator().Value().Second().Data,iterator().Value().Second().Size,SQLITE_TRANSIENT);break;
				default: 
					ok=sqlite3_bind_null(st,index+1);
				}

				if (ok!=SQLITE_OK)
				{
					ProcessError(database,db);
					return 0;
				}

				++index;
			}
		}

		return sqlite3_step(st);
	}

	return 0;
}

int SqLite::Execute(SqlDatabase * database, SqlStatement * statement)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_EXECUTE,this,database,statement);
	Library().Process(message);
	int result = message->Result;
	delete message;
	return result;
#else
	return ProcessExecute(database,statement);
#endif
}

int SqLite::ProcessExecute(SqlDatabase * database, SqlStatement * statement)
{
	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	int count=0;

	sqlite3 *& db = Database(database);
	sqlite3_stmt *& st = Statement(statement);

	int state = Run(database,statement,db,st);

	if (state == SQLITE_ROW)
	{

		int parameters = sqlite3_bind_parameter_count(st);

		int state = SQLITE_ROW;
		while (state == SQLITE_ROW)
		{
			++count;

			state = sqlite3_step(st);
			if (state!=SQLITE_ROW && state!=SQLITE_DONE)
			{
				ProcessError(database,db,st,state);
				return 0;
			}
		}

	}
	else
	if (state == SQLITE_DONE)
	{

		String string = *statement;
		string.Trim();
		count = Changes(db,&string,count);
	}
	else
	{
		ProcessError(database,db,st,state);
	}

	return count;
}

int SqLite::Execute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_EXECUTE,this,database,statement,record);
	Library().Process(message);
	int result = message->Result;
	delete message;
	return result;
#else
	return ProcessExecute(database,statement,record);
#endif
}

int SqLite::ProcessExecute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record)
{
	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	int count=0;

	sqlite3 *& db = Database(database);
	sqlite3_stmt *& st = Statement(statement);

	int state = Run(database,statement,db,st);

	if (state == SQLITE_ROW)
	{

		Iterand< Mapped<void *,void *> > iterand = Records.Select((void*)record);
		if (iterand)
		{

			sqlite3_stmt * ts = (sqlite3_stmt*)iterand().Value();
			if (ts) sqlite3_finalize(ts);
		}

		Records.Update((void*)record,(void*)st);

		record->Index = 0;

		++count;
		int columns = sqlite3_data_count(st);
		for (int col=0;col<columns;++col)
		{
			record->Fields.Append((const char*)sqlite3_column_name(st,col));

			int size = sqlite3_column_bytes(st,col);
			char * data = (char*)sqlite3_column_text(st,col);

			record->Append(Superstring(data,size));	
		}

		Statements.Remove((int*)statement);
	}
	else
	if (state == SQLITE_DONE)
	{
		String string = *statement;
		string.Trim();
		count = Changes(db,&string,count);

		ProcessClose(database,record);
	}
	else
	{
		ProcessError(database,db,st,state);
		ProcessClose(database,record);
	}

	return count;
}

int SqLite::Execute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_EXECUTE,this,database,statement,recordset);
	Library().Process(message);
	int result = message->Result;
	delete message;
	return result;
#else
	return ProcessExecute(database,statement,recordset);
#endif
}

int SqLite::ProcessExecute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset)
{
	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	recordset->Release();

	int count=0;

	sqlite3 *& db = Database(database);
	sqlite3_stmt *& st = Statement(statement);

	int state = Run(database,statement,db,st);

	if (state == SQLITE_ROW)
	{

		int parameters = sqlite3_bind_parameter_count(st);
		int columns = sqlite3_data_count(st);

        if (recordset->IsEmpty())
            for (int col=0;col<columns;++col)
                recordset->Fields.Append((const char*)sqlite3_column_name(st,col));

		int state = SQLITE_ROW;
		while (state == SQLITE_ROW)
		{
			++count;

			int row = recordset->Length();
			recordset->Append(Array<String>());

			for (int col=0;col<columns;++col)
			{
				int size = sqlite3_column_bytes(st,col);
				char * data = (char*)sqlite3_column_text(st,col);

				(*recordset)[row].Append(Superstring(data,size));	
			}

			state = sqlite3_step(st);
			if (state!=SQLITE_ROW && state!=SQLITE_DONE)
			{
				ProcessError(database,db,st,state);
				return 0;
			}
		}

	}
	else
	if (state == SQLITE_DONE)
	{
		String string = *statement;
		string.Trim();
		count = Changes(db,&string,count);
	}
	else
	{
		ProcessError(database,db,st,state);
	}

	return count;
}

int SqLite::Step(SqlDatabase * database, SqlRecord * record)
{
#ifdef SQLITE_DRIVER_LIBRARY
	SqLiteMessage * message = new SqLiteMessage(SqLiteMessage::MESSAGE_STEP,this,database,record);
	Library().Process(message);
	int result = message->Result;
	delete message;
	return result;
#else
	return ProcessStep(database,record);
#endif
}

int SqLite::ProcessStep(SqlDatabase * database, SqlRecord * record)
{
	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	record->Release();

	int count=0;

	sqlite3 * db = (sqlite3*)Databases[(int*)database];
	sqlite3_stmt * st = (sqlite3_stmt*)Records[(int*)record];

	if (st)
	{

		int state = sqlite3_step(st);

		if (state == SQLITE_ROW)
		{

			record->Index ++;

			++count;
			int columns = sqlite3_data_count(st);
			for (int col=0;col<columns;++col)
			{

				int size = sqlite3_column_bytes(st,col);
				char * data = (char*)sqlite3_column_text(st,col);

				record->Append(Superstring(data,size));	
			}
		}		
		else
		{
			if (state != SQLITE_DONE)
			{
				ProcessError(database,db,st,state);

				OutputError("SqLite::Step - Code, %d.\n",state);
				record->Reset();
				count = 0;
			}

			ProcessClose(database,record);
		}
	}

	return count;
}

sqlite3 *& SqLite::Database(SqlDatabase * database)
{
	Iterand< Mapped<void*,void*> > iterand = Databases.Select((void*)database);
	if (!iterand)
		iterand = Databases.Insert((void*)database,0);

    sqlite3 *& db = (sqlite3*&)iterand().Value();
	return db;
}

sqlite3_stmt *& SqLite::Statement(SqlStatement * statement)
{
	Iterand< Mapped<void*,void*> > iterand = Statements.Select((void*)statement);
	if (!iterand)
		iterand = Statements.Insert((int*)statement,0);

    sqlite3_stmt *& st = (sqlite3_stmt*&)iterand().Value();
	return st;
}

void SqLite::ProcessError(SqlDatabase * database, sqlite3 *db)
{
	database->Error = true;
	database->Message = sqlite3_errmsg(db);
	OutputError("SqLite::ProcessError - Error, %s.\n",database->Message.Print());
}

void SqLite::ProcessError(SqlDatabase * database, sqlite3 * db, sqlite3_stmt * st, int state)
{

	if (state != SQLITE_BUSY && state != SQLITE_MISUSE)
		sqlite3_reset(st);

	database->Error = true;
	database->Message = sqlite3_errmsg(db);
	OutputError("SqLite::ProcessError - Error, %s.\n",database->Message.Print());

}

void SqLite::ProcessError(SqlDatabase * database, char * msg)
{
	database->Error = true;
	database->Message = msg;
	OutputError("SqLite::ProcessError - Error, %s.\n", database->Message.Print());
	sqlite3_free(msg);
}

int SqLite::Changes(sqlite3 * db, Sequence * query, int count)
{

	if (count == 0 && (query->StartsWith("insert") || query->StartsWith("update") || query->StartsWith("delete") ||
						query->StartsWith("begin") || query->StartsWith("commit") || query->StartsWith("rollback")))

	{
		count = sqlite3_changes(db);
	}

	return count;
}

int SqLite::Busy(void * arg, int count)
{
	sqlite3 * db = (sqlite3*)arg;
	OutputError("SqLite::Busy - Error, Database was locked.\n");

	return (count < 30)?1:0;
}

int SqLite::Count(void * argument, int columns, char **values, char **fields)
{
	int & counter = *(int*)argument;
	++counter;
	return 0;
}

int SqLite::Record(void * argument, int columns, char **values, char **fields)
{
	if (!argument) return 0;

	SqlRecordset * recordset = (SqlRecordset*)argument;

	if (recordset->Fields.IsEmpty())
		for(int i=0; i<columns; i++)
			recordset->Fields.Append(fields[i]);

	int row=recordset->Length()?recordset->Length():0;
	int column=0;
	recordset->Append(Array<String>());
	(*recordset)[row].Reserve(recordset->Fields.Size);

	for(int i=0; i<columns; i++)
	{
		(*recordset)[row].Append(values[i]);

	}

	return 0;
}

int SqLite::Id(SqlDatabase * database)
{

	MutexesMutex.Enter();
	Mutex * mutex = Mutexes[(int*)database];
	MutexesMutex.Leave();
	Mutex::Auto mutexAuto(mutex);

	sqlite3 * db = (sqlite3*)Databases[(int*)database];

	if (db) return sqlite3_last_insert_rowid(db);

	return 0;
}

bool SqLite::Transaction(SqlDatabase * database, int transaction)
{

	String string;
	switch(transaction)
	{
	case SqlDriver::SQL_TRANSACTION_BEGIN:
		{
			string = "begin immediate";

			return Execute(database,&string);	
		}
	case SqlDriver::SQL_TRANSACTION_COMMIT:
		{
			string = "commit";
			return Execute(database,&string);
		}
	case SqlDriver::SQL_TRANSACTION_ROLLBACK:
		{
			string = "rollback";
			return Execute(database,&string);
		}
	}

	return false;
}

bool SqLite::IsEmpty(SqlDatabase * database)
{

	String string("select * from sqlite_master where tbl_name not null");
	return Execute(database,&string) == 0;
}

#endif

#ifdef REASON_USING_MYSQL

MySql::MySqlLibrary::MySqlLibrary()
{
	mysql_server_init(0,0,0);
}

MySql::MySqlLibrary::~MySqlLibrary()
{
	mysql_server_end();
}

MySql::MySqlLibrary MySql::Library;

MySql::~MySql()
{
	{
		Map<SqlDatabase *, MYSQL *>::Iterator iterator = Databases.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			MYSQL * db = iterator().Value();
			if (db) mysql_close(db);
		}
	}

	{
		Map<SqlDatabase *, MYSQL_RES *>::Iterator iterator = Results.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			MYSQL_RES * res = iterator().Value();
			if (res) mysql_free_result(res);
		}
	}

	{
		Map<SqlStatement *, MYSQL_STMT *>::Iterator iterator = Statements.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			MYSQL_STMT * st = iterator().Value();
			if (st) mysql_stmt_close(st);
		}
	}
}

bool MySql::Open(SqlDatabase * database, const char * data, const char * user, const char * pass, const char * host, unsigned int port)
{
	MYSQL * db = Databases[database];

	db = (db)?mysql_init(db):mysql_init(0);

	if (db)
	{

		if (!mysql_real_connect(db, host,user,pass,data,port,0,0))
		{
			database->Error = true;
			database->Message = mysql_error(db);
			OutputError("MySql::Open - Failed to connect to database, error: %s\n", database->Message.Print());
			mysql_close(db);
			return false;
		}

		Databases.Insert(database,db);
	}

	return true;
}

bool MySql::Close(SqlDatabase * database)
{
	MYSQL * db = Databases[database];
	if (db)
	{
		mysql_close(db);

		Databases.Delete(database);
	}

	MYSQL_RES * res = Results[database];
	if (res)
	{
		mysql_free_result(res);
		Results.Delete(database);
	}

	return true;
}

bool MySql::Open(SqlDatabase * database, SqlStatement * statement)
{
	MYSQL * db = Databases[database];
	if (db)
	{
		MYSQL_STMT * st = Statements[statement];
		if (st) mysql_stmt_close(st);

		st = mysql_stmt_init(db);

		Statements.Insert(statement,st);

		return true;
	}

	return false;
}

bool MySql::Close(SqlDatabase * database, SqlStatement * statement)
{
	MYSQL_STMT * st = Statements[statement];
	if (st)
	{
		mysql_stmt_close(st);
		Statements.Delete(statement);
		return true;
	}

	return false;

}

bool MySql::Close(SqlDatabase * database, SqlRecord * record)
{
	SqlDriver::Close(database,record);

	record->Reset();

	MYSQL_STMT * st = Records[record]; 
	if (st)
	{
		mysql_stmt_close(st);
		Records.Delete(record);
		return true;
	}

	return false;
}

int MySql::Execute(SqlDatabase * database, Sequence * query)
{
	String string = Superstring::Literal(*query);

	MYSQL * db = Databases[database];
	if( (db == 0) || mysql_real_query(db, string.Data,string.Size)!=0)
	{
		if (db)
		{
			database->Error = true;
			database->Message = mysql_error(db);
			OutputError("MySql::Execute - Error, %s.\n", database->Message.Print());
		}
		else
		{
			database->Error = true;
			OutputError("MySql::Execute - Error, database not opened.\n");
		}

		return 0;
	}

	unsigned int fields=0;
	unsigned int rows=0;

	MYSQL_RES * res = Results[database];
	if (res) 
	{
		mysql_free_result(res);
		Results.Delete(database);
	}

	res = mysql_store_result(db);
	if (res)  
	{

		Results.Insert(database,res);
		rows = mysql_num_rows(res);
	}
	else  
	{

		if((fields=mysql_field_count(db))==0)
		{

			rows = mysql_affected_rows(db);
		}
		else 
		{

			database->Error = true;
			database->Message = mysql_error(db);
			OutputError("MySql::Execute - Error, %s.\n", database->Message.Print());
		}
	}

	return rows;
}

int MySql::Execute(SqlDatabase * database, Sequence * query, SqlRecord * record)
{

	SqlStatement statement(*database);
	statement.Prepare(*query);
	return Execute(database,&statement,record);
}

int MySql::Execute(SqlDatabase * database, Sequence * query, SqlRecordset * recordset)
{
	recordset->Release();

	if (Execute(database,query))
	{
		unsigned int fields=0;
		unsigned int rows=0;

		MYSQL * db = Databases[database];
		MYSQL_RES * res = Results[database];

		if (res)
		{
			fields = mysql_num_fields(res);
			rows = mysql_num_rows(res);

			MYSQL_FIELD *field = mysql_fetch_fields(res);
			for (int i=0;i<fields;++i)
				recordset->Fields.Append(field[i].name);

			MYSQL_ROW row;		
			while ((row=mysql_fetch_row(res)))
			{
				recordset->Append(Array<String>());
				(*recordset)[recordset->Length()-1].Reserve(fields);

				unsigned long * lengths = mysql_fetch_lengths(res);
				for (int i=0;i<fields;++i)
				{
					(*recordset)[recordset->Length()-1].Append(Substring(row[i],lengths[i]));
				}
			}

			if (recordset->Count != rows)
			{
				database->Error = true;
				database->Message = mysql_error(db);
				OutputError("MySql::Execute - Error, %s.\n", database->Message.Print());
			}

			mysql_free_result(res);
			Results.Delete(database);
		}
		else
		{
			if((fields=mysql_field_count(db))==0)
			{

				rows = mysql_affected_rows(db);
			}
			else 
			{

				database->Error = true;
				database->Message = mysql_error(db);
				OutputError("MySql::Execute - Error, %s.\n", database->Message.Print());
			}
		}

		return rows;
	}

	return 0;
}

int MySql::Execute(SqlDatabase * database, SqlStatement * statement)
{

	if (statement->IsEmpty()) return 0;

	MYSQL * db = Databases[database];
	MYSQL_STMT * st = Statements[statement];

	if (db  )
	{

		if (st == 0)
		{
			Open(database,statement);
			st = Statements[statement];

			if (mysql_stmt_prepare(st,statement->Data,statement->Size)!=0)
			{
				database->Error = true;
				database->Message = mysql_error(db);
				OutputError("MySql::Prepare - Error, %s.\n",database->Message.Print());
				return 0;
			}
		}

		int count = mysql_stmt_param_count(st);
		if (statement->Parameters.Count != count)
		{
			database->Error = true;
			OutputError("MySql::Execute - Error, expected %d parameters, received %d.\n",count,statement->Parameters.Count);
			return 0;
		}

		MYSQL_BIND * bind = new MYSQL_BIND[count];
		memset(bind, 0, sizeof(MYSQL_BIND)*count);

		if (statement->Parameters.Count > 0)
		{
			int index=0;
			Reason::Structure::Map< int,Reason::Structure::Pair<int,String> >::Iterator iterator = statement->Parameters.Iterate();
			for (iterator.Forward();iterator.Has();iterator.Move())
			{						
				int type = iterator().Value().First();
				switch (type)
				{
				case SqlStatement::SQL_TYPE_INTEGER: type = MYSQL_TYPE_LONG; break;
				case SqlStatement::SQL_TYPE_FLOAT: type = MYSQL_TYPE_FLOAT; break;
				case SqlStatement::SQL_TYPE_TEXT: type = MYSQL_TYPE_BLOB; break;
				case SqlStatement::SQL_TYPE_BLOB: type = MYSQL_TYPE_LONG_BLOB; break;
				default: type = MYSQL_TYPE_NULL;
				}

				if (type == MYSQL_TYPE_NULL)
					iterator().Value().Second().Append("0");

				bind[index].buffer_type = (enum_field_types) type;
				bind[index].buffer = iterator().Value().Second().Data;
				bind[index].buffer_length = iterator().Value().Second().Size;

				if (++index > count)
					break;
			}

			if (mysql_stmt_bind_param(st,bind))
			{
				database->Error = true;
				database->Message = mysql_stmt_error(st);
				OutputError("MySql::Execute - Error, %s.\n",database->Message.Print());
				delete [] bind;
				return 0;
			}
		}

		if (mysql_stmt_execute(st)!=0)
		{
			database->Error = true;
			database->Message = mysql_stmt_error(st);
			OutputError("MySql::Execute - Error, %s.\n",database->Message.Print());
			delete [] bind;
			return 0;
		}

		if (mysql_stmt_store_result(st)!=0)
		{
			database->Error = true;
			database->Message = mysql_stmt_error(st);
			OutputError("MySql::Execute - Error, %s.\n",database->Message.Print());
			delete [] bind;
			return 0;
		}

		delete [] bind;

		return mysql_stmt_affected_rows(st);
	}

	return 0;
}

int MySql::Execute(SqlDatabase * database, SqlStatement * statement, SqlRecord * record)
{
	int count=0;
	if (Execute(database,statement))
	{
		MYSQL_STMT * st = Statements[statement];
		Statements.Delete(statement);

		Records.Insert(record,st);
		count = Step(database,record);
	}

	return count;
}

int MySql::Execute(SqlDatabase * database, SqlStatement * statement, SqlRecordset * recordset)
{
	recordset->Release();

	if (Execute(database,statement))
	{
		MYSQL * db = Databases[database];
		MYSQL_STMT * st = Statements[statement];

		MYSQL_RES * res = mysql_stmt_result_metadata(st);
		if (res)
		{
			int fields = mysql_num_fields(res);
			MYSQL_FIELD * field = mysql_fetch_fields(res);

			MYSQL_BIND * bind = new MYSQL_BIND[fields];
			memset(bind, 0, sizeof(MYSQL_BIND)*fields);

			Reason::Structure::Array<unsigned long> bindLengths(fields);
			Reason::Structure::Array<String> bindBuffers(fields);

			for (int i=0;i<fields;++i)
			{

				recordset->Fields.Append(field[i].name);

				bindLengths.Append(0);
				bindBuffers.Append(String());

				int length = BindFieldLength(field[i]);
				bindBuffers[i].Allocate(length);

				bind[i].buffer_length = length;
				bind[i].buffer_type = field[i].type;
				bind[i].buffer = bindBuffers[i].Data;
				bind[i].length = &bindLengths[i]; 
			}

			if (mysql_stmt_bind_result(st,bind))
			{
				database->Error = true;
				database->Message = mysql_stmt_error(st);
				OutputError("MySql::Execute - Error, %s.\n",database->Message.Print());
				delete [] bind;
				mysql_free_result(res);
				return 0;
			}

			int state=0;
			while ((state=mysql_stmt_fetch(st))==0)
			{
				int row = recordset->Length();
				recordset->Append(Array<String>());

				for (int i=0;i<fields;++i)
				{	
					OutputAssert(bindLengths[i] <= bindBuffers[i].Allocated);
					(*recordset)[row].Append(BindField(bind[i],bindBuffers[i].Data,bindLengths[i]));
				}
			}

			delete [] bind;
			mysql_free_result(res);

			if (state==1)
			{
				database->Error = true;
				database->Message = mysql_stmt_error(st);
				OutputError("MySql::Execute - Error, %s.\n",database->Message.Print());
				return 0;
			}

			return recordset->Count;
		}
		else
		{
			return mysql_stmt_affected_rows(st);
		}
	}

	return 0;
}

int MySql::BindFieldLength(MYSQL_FIELD field)
{

	switch (field.type)
	{
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_TIMESTAMP:
		{
			int length = sizeof(MYSQL_TIME);
			return length;			
		}				
		break;
		case MYSQL_TYPE_TINY: return 1;
		case MYSQL_TYPE_SHORT: return 2;
		case MYSQL_TYPE_LONG: return 4;
		case MYSQL_TYPE_LONGLONG: return 8;
		case MYSQL_TYPE_FLOAT: return 4;
		case MYSQL_TYPE_DOUBLE: return 8;
		default:
		{
			int length = (field.max_length)?field.max_length:field.length;		
			return length; 			
		}						
	}

}

Superstring MySql::BindField(MYSQL_BIND bind, char * data, int size)
{
	switch (bind.buffer_type)
	{
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_DATETIME:
	case MYSQL_TYPE_TIMESTAMP:
		{
			MYSQL_TIME time = (MYSQL_TIME)*((MYSQL_TIME*)data);
			Calendar calendar;
			calendar.Year = time.year;
			calendar.Month = time.month;
			calendar.Day = time.day;
			calendar.Hour = time.hour;
			calendar.Minute = time.minute;
			calendar.Second = time.second;
			return SqlTime(calendar);
		}
		break;
	case MYSQL_TYPE_BIT:
	case MYSQL_TYPE_LONGLONG: 
		return Superstring((long long)*((long long*)data));
		break;
	case MYSQL_TYPE_FLOAT:
		return Superstring((float)*((float*)data));
		break;
	case MYSQL_TYPE_DOUBLE:
		return Superstring((double)*((double*)data));
		break;
	case MYSQL_TYPE_LONG: 
		return Superstring((int)*((int*)data));
		break;
	case MYSQL_TYPE_SHORT:
		return Superstring((int)*((short*)data));
		break;
	case MYSQL_TYPE_TINY:
		return Superstring((int)*((char*)data));
		break;
	default:
		return Superstring(data,size);
	}
}

int MySql::Step(SqlDatabase * database, SqlRecord * record)
{
	record->Release();
	record->Index ++;

	MYSQL * db = Databases[database];
	MYSQL_STMT * st = Records[record];

	if (st)
	{

		MYSQL_RES * res = mysql_stmt_result_metadata(st);
		if (res)
		{
			int fields = mysql_num_fields(res);
			MYSQL_FIELD * field = mysql_fetch_fields(res);

			MYSQL_BIND * bind = new MYSQL_BIND[fields];
			memset(bind, 0, sizeof(MYSQL_BIND)*fields);

			Reason::Structure::Array<unsigned long> bindLengths(fields);
			Reason::Structure::Array<String> bindBuffers(fields);

			for (int i=0;i<fields;++i)
			{

				if (record->Index == 0)
					record->Fields.Append(field[i].name);

				bindLengths.Append(0);
				bindBuffers.Append(String());

				int length = BindFieldLength(field[i]);
				bindBuffers[i].Allocate(length);

				bind[i].buffer_length = length;
				bind[i].buffer_type = field[i].type;
				bind[i].buffer = bindBuffers[i].Data;
				bind[i].length = &bindLengths[i]; 
			}

			if (mysql_stmt_bind_result(st,bind))
			{
				database->Error = true;
				database->Message = mysql_stmt_error(st);
				OutputError("MySql::Execute - Error, %s.\n",database->Message.Print());
				delete [] bind;
				mysql_free_result(res);
				return 0;
			}

			int state=0;
			if ((state=mysql_stmt_fetch(st))==0)
			{
				for (int i=0;i<fields;++i)
				{	
					OutputAssert(bindLengths[i] <= bindBuffers[i].Allocated);

					record->Append(BindField(bind[i],bindBuffers[i].Data,bindLengths[i]));
				}
			}

			delete [] bind;
			mysql_free_result(res);

			if (state==1)
			{
				database->Error = true;
				database->Message = mysql_stmt_error(st);
				OutputError("MySql::Execute - Error, %s.\n",database->Message.Print());
				return 0;
			}

			if (state!=MYSQL_NO_DATA && state!=MYSQL_DATA_TRUNCATED)
				return 1;
		}
		else
		{
			return mysql_stmt_affected_rows(st);
		}
	}

	return 0;
}

int MySql::Id(SqlDatabase * database)
{
	MYSQL * db = Databases[database];
	if (db) return mysql_insert_id(db);

	return 0;
}

bool MySql::Transaction(SqlDatabase * database, int transaction)
{
	String string;
	switch(transaction)
	{
	case SqlDriver::SQL_TRANSACTION_BEGIN:
		{
			string = "begin";
			return Execute(database,&string);	
		}
	case SqlDriver::SQL_TRANSACTION_COMMIT:
		{
			string = "commit";
			return Execute(database,&string);
		}
	case SqlDriver::SQL_TRANSACTION_ROLLBACK:
		{
			string = "rollback";
			return Execute(database,&string);
		}
	}

	return false;
}

bool MySql::IsEmpty(SqlDatabase * database)
{
	MYSQL * db = Databases[database];
	MYSQL_RES * res = mysql_list_tables(db, "*");

	if (res)
	{
		int rows = mysql_num_rows(res);
		mysql_free_result(res);
		return rows == 0;
	}
	else
	{
		OutputError("MySql::IsEmpty - Error, %s.\n", mysql_error(db));
	}

	return true;
}

#endif

