// BSD 3-Clause License
//
// Copyright (c) 2020-2025, Shahriar Rezghi <shahriar.rezghi.sh@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#ifdef NEYSON_USE_SQLITE

#include <neyson/value.h>

#include <functional>
#include <memory>

namespace Neyson
{
namespace SQLite
{
enum class Error
{
    /// @brief Represents an unallocated or invalid error code (default state).
    Unallocated = -1,

    /// @brief No error occurred.
    OK = 0,

    /// @brief A generic or undefined error occurred.
    Error = 1,

    /// @brief An internal logic error.
    Internal = 2,

    /// @brief Access permission was denied.
    Perm = 3,

    /// @brief A process or operation was aborted.
    Abort = 4,

    /// @brief The database or table was busy and could not be accessed.
    Busy = 5,

    /// @brief A table, index, or record is locked and cannot be modified or read.
    Locked = 6,

    /// @brief Memory could not be allocated.
    NoMem = 7,

    /// @brief A write operation attempted on a read-only database.
    ReadOnly = 8,

    /// @brief An operation was interrupted before completion.
    Interrupt = 9,

    /// @brief A disk I/O error occurred during a read or write.
    IOErr = 10,

    /// @brief The database file or data is corrupted.
    Corrupt = 11,

    /// @brief A required table, index, or record was not found.
    NotFound = 12,

    /// @brief The database is full and cannot store additional data.
    Full = 13,

    /// @brief The database file could not be opened.
    CantOpen = 14,

    /// @brief A protocol-level error was encountered.
    Protocol = 15,

    /// @brief The database is empty (no data).
    Empty = 16,

    /// @brief The database schema has changed in a way that invalidates current statements.
    Schema = 17,

    /// @brief A string or binary object exceeds the size limit.
    TooBig = 18,

    /// @brief A constraint (unique, foreign key, etc.) was violated.
    Constraint = 19,

    /// @brief The data type of a value did not match the expected type.
    Mismatch = 20,

    /// @brief The library or wrapper was used in an invalid manner.
    Misuse = 21,

    /// @brief Large file support is not available.
    NoLFS = 22,

    /// @brief Authorization failed or was denied.
    Auth = 23,

    /// @brief The data format is invalid or incompatible.
    Format = 24,

    /// @brief An argument or parameter is out of the valid range.
    Range = 25,

    /// @brief The file is not recognized as a valid database.
    NotADB = 26,

    /// @brief A general notice-level message or minor issue was raised.
    Notice = 27,

    /// @brief A warning-level message or non-critical issue was raised.
    Warning = 28,

    /// @brief A row of data is available when stepping through query results.
    Row = 100,

    /// @brief The query has finished executing, and no more data is available.
    Done = 101,
};

enum class Extended
{
    // clang-format off
    ErrorMissingCollSeq    = int(Error::Error)      |  (1<<8),
    ErrorRetry             = int(Error::Error)      |  (2<<8),
    ErrorSnapshot          = int(Error::Error)      |  (3<<8),
    IOErrRead              = int(Error::IOErr)      |  (1<<8),
    IOErrShortRead         = int(Error::IOErr)      |  (2<<8),
    IOErrWrite             = int(Error::IOErr)      |  (3<<8),
    IOErrFSync             = int(Error::IOErr)      |  (4<<8),
    IOErrDirFSync          = int(Error::IOErr)      |  (5<<8),
    IOErrTruncate          = int(Error::IOErr)      |  (6<<8),
    IOErrFStat             = int(Error::IOErr)      |  (7<<8),
    IOErrUnlock            = int(Error::IOErr)      |  (8<<8),
    IOErrRDLock            = int(Error::IOErr)      |  (9<<8),
    IOErrDelete            = int(Error::IOErr)      | (10<<8),
    IOErrBlocked           = int(Error::IOErr)      | (11<<8),
    IOErrNoMem             = int(Error::IOErr)      | (12<<8),
    IOErrAccess            = int(Error::IOErr)      | (13<<8),
    IOErrCheckReservedLock = int(Error::IOErr)      | (14<<8),
    IOErrLock              = int(Error::IOErr)      | (15<<8),
    IOErrClose             = int(Error::IOErr)      | (16<<8),
    IOErrDirClose          = int(Error::IOErr)      | (17<<8),
    IOErrSHMOpen           = int(Error::IOErr)      | (18<<8),
    IOErrSHMSize           = int(Error::IOErr)      | (19<<8),
    IOErrSHMLock           = int(Error::IOErr)      | (20<<8),
    IOErrSHMMap            = int(Error::IOErr)      | (21<<8),
    IOErrSeek              = int(Error::IOErr)      | (22<<8),
    IOErrDeleteNoEnt       = int(Error::IOErr)      | (23<<8),
    IOErrMMap              = int(Error::IOErr)      | (24<<8),
    IOErrGetTempPath       = int(Error::IOErr)      | (25<<8),
    IOErrConvPath          = int(Error::IOErr)      | (26<<8),
    IOErrVNode             = int(Error::IOErr)      | (27<<8),
    IOErrAuth              = int(Error::IOErr)      | (28<<8),
    IOErrBeginAtomic       = int(Error::IOErr)      | (29<<8),
    IOErrCommitAtomic      = int(Error::IOErr)      | (30<<8),
    IOErrRollbackAtomic    = int(Error::IOErr)      | (31<<8),
    IOErrData              = int(Error::IOErr)      | (32<<8),
    IOErrCorruptFS         = int(Error::IOErr)      | (33<<8),
    LockedSharedCache      = int(Error::Locked)     |  (1<<8),
    LockedVTab             = int(Error::Locked)     |  (2<<8),
    BusyRecovery           = int(Error::Busy)       |  (1<<8),
    BusySnapshot           = int(Error::Busy)       |  (2<<8),
    BusyTimeout            = int(Error::Busy)       |  (3<<8),
    CantOpenNoTempDir      = int(Error::CantOpen)   |  (1<<8),
    CantOpenIsDir          = int(Error::CantOpen)   |  (2<<8),
    CantOpenFullPath       = int(Error::CantOpen)   |  (3<<8),
    CantOpenConvPath       = int(Error::CantOpen)   |  (4<<8),
    CantOpenDirtywal       = int(Error::CantOpen)   |  (5<<8),
    CantOpenSymlink        = int(Error::CantOpen)   |  (6<<8),
    CorruptVTab            = int(Error::Corrupt)    |  (1<<8),
    CorruptSequence        = int(Error::Corrupt)    |  (2<<8),
    CorruptIndex           = int(Error::Corrupt)    |  (3<<8),
    ReadOnlyRecovery       = int(Error::ReadOnly)   |  (1<<8),
    ReadOnlyCantLock       = int(Error::ReadOnly)   |  (2<<8),
    ReadOnlyRollback       = int(Error::ReadOnly)   |  (3<<8),
    ReadOnlyDBMoved        = int(Error::ReadOnly)   |  (4<<8),
    ReadOnlyCantInit       = int(Error::ReadOnly)   |  (5<<8),
    ReadOnlyDirectory      = int(Error::ReadOnly)   |  (6<<8),
    AbortRollback          = int(Error::Abort)      |  (2<<8),
    ConstraintCheck        = int(Error::Constraint) |  (1<<8),
    ConstraintCommitHoot   = int(Error::Constraint) |  (2<<8),
    ConstraintForeignKey   = int(Error::Constraint) |  (3<<8),
    ConstraintFunction     = int(Error::Constraint) |  (4<<8),
    ConstraintNotNull      = int(Error::Constraint) |  (5<<8),
    ConstraintPrimaryKey   = int(Error::Constraint) |  (6<<8),
    ConstraintTrigger      = int(Error::Constraint) |  (7<<8),
    ConstraintUnique       = int(Error::Constraint) |  (8<<8),
    ConstraintVTab         = int(Error::Constraint) |  (9<<8),
    ConstraintRowID        = int(Error::Constraint) | (10<<8),
    ConstraintPinned       = int(Error::Constraint) | (11<<8),
    ConstraintDataType     = int(Error::Constraint) | (12<<8),
    NoticeRecoveryWal      = int(Error::Notice)     |  (1<<8),
    NoticeRecoverRollback  = int(Error::Notice)     |  (2<<8),
    NoticeRBU              = int(Error::Notice)     |  (3<<8),
    WarningAutoIndex       = int(Error::Warning)    |  (1<<8),
    AuthUser               = int(Error::Auth)       |  (1<<8),
    OKLoadPermanently      = int(Error::OK)         |  (1<<8),
    OKSymlink              = int(Error::OK)         |  (2<<8),
    // clang-format on
};

/// @brief Contains the outcome of a SQLite operation, including the main and
///        extended error codes, as well as an optional message.
struct Result
{
    /// @brief The main SQLite error code (OK, Row, Done, etc.).
    Error error;

    /// @brief The extended error code for more specific failure or success states.
    Extended extended;

    /// @brief A descriptive message for additional context, if applicable.
    String message;

    /// @brief Implicit boolean conversion indicating success.
    ///
    /// @details Returns true if the error code is one of OK, Row, or Done,
    ///          signifying a generally successful or complete operation.
    ///          Otherwise, returns false.
    inline operator bool() const { return error == Error::OK || error == Error::Row || error == Error::Done; }
};

/// @brief A wrapper for storing or interpreting a Value as a binary large object (BLOB).
///
/// @details This struct references an external Value, assumed to contain
///          data suitable for a SQLite BLOB field. It does not copy or own
///          the data, only references it.
struct Blob
{
    /// @brief A reference to the underlying Value to be treated as a BLOB.
    const Value &value;

    /// @brief Constructs a Blob wrapper referencing the given Value.
    ///
    /// @param value A reference to a Value object containing the BLOB data.
    inline Blob(const Value &value) : value(value) {}
};

/// @brief Represents a prepared SQLite statement.
class Statement;

/// @brief Represents a SQLite transaction scope.
class Transaction;

/// @brief Represents a SQLite backup operation.
class Backup;

/// @brief Encapsulates a connection to a SQLite database, including operations
///        for opening, executing queries, transactions, and backups.
///
/// @details Instances of this class manage the lifetime of a SQLite connection
///          via a shared pointer. The connection can be opened with various flags
///          (e.g., read-only, read-write, create) and closed automatically once
///          the Database object goes out of scope or when explicitly closed.
class Database
{
public:
    /// @brief Flags controlling how the database is opened.
    ///
    /// @details These flags typically map to SQLite's open flags, possibly combined
    ///          via a bitwise OR. Adjust as needed for your specific environment or needs.
    enum Flags
    {
        /// @brief Open the database in read-only mode.
        ReadOnly = 0x00000001,

        /// @brief Open the database in read-write mode.
        ReadWrite = 0x00000002,

        /// @brief Create the database if it does not exist.
        Create = 0x00000004,

        /// @brief Interpret the given path as a URI.
        URI = 0x00000040,

        /// @brief Create an in-memory database.
        Memory = 0x00000080,

        /// @brief Open the database in multi-thread or single-thread mode (no internal mutexes).
        NoMutex = 0x00008000,

        /// @brief Open the database in serialized mode (fully threaded, with mutexes).
        FullMutex = 0x00010000,

        /// @brief Enable shared cache mode.
        SharedCache = 0x00020000,

        /// @brief Enable private cache mode.
        PrivateCache = 0x00040000,

        /// @brief Disables following symbolic links when opening databases.
        NoFollow = 0x01000000,

        /// @brief Return extended result codes where applicable.
        ExResCode = 0x02000000,
    };

    /// @brief Flags controlling statement preparation behavior.
    ///
    /// @details These flags map to optional behaviors when preparing a SQLite statement.
    enum Prepare
    {
        /// @brief No special preparation flags.
        NoPreparation = 0x00,

        /// @brief Hint that the prepared statement may be reused many times.
        Persistent = 0x01,

        /// @brief Request that SQLite normalize SQL statements.
        Normalize = 0x02,

        /// @brief Disable creation of virtual tables.
        NoVTab = 0x04,
    };

    /// @brief Opens a connection to the database specified by @p path using the given @p flags.
    ///
    /// @details The flags may be combined (e.g., ReadWrite | Create) to customize the connection.
    ///          If the open operation fails, this method returns false and the internal connection remains closed.
    ///
    /// @param path  The file path (or URI) to the SQLite database.
    /// @param flags A bitwise combination of Flags enumerators. Defaults to ReadWrite | Create.
    /// @return True if the database was successfully opened, otherwise false.
    bool open(const String &path, int flags = Flags::ReadWrite | Flags::Create);

    /// @brief Executes a SQL query without returning a result set, or with optional row-by-row callback.
    ///
    /// @details This method is useful for simple statements like CREATE TABLE or INSERT/UPDATE/DELETE.
    ///          For queries returning results, you can provide a callback function which will be invoked
    ///          for each row. The callback receives an Object containing the row's data; if it returns false,
    ///          the iteration stops.
    ///
    /// @param query The SQL query to execute.
    /// @param step  An optional callback function called for each row in the result.
    ///              The callback is passed an Object containing column data.
    /// @return True if the query executed successfully, otherwise false.
    bool execute(const String &query, std::function<bool(Object &)> step = {});

    /// @brief Begins a new SQLite transaction.
    ///
    /// @details Returns a Transaction object that manages COMMIT or ROLLBACK automatically
    ///          when it is destroyed, if not done explicitly. Useful for ensuring atomic operations.
    ///
    /// @return A Transaction object associated with this database connection.
    Transaction transaction();

    /// @brief Prepares a SQL statement for execution, possibly with special flags.
    ///
    /// @details Returns a Statement object for parameter binding and step execution.
    ///          If preparation fails, the returned Statement may be invalid.
    ///
    /// @param query   The SQL query string to prepare.
    /// @param prepare Optional flags from Prepare enum (e.g., Persistent, Normalize).
    /// @return A Statement object representing the compiled query.
    Statement prepare(const String &query, int prepare = Prepare::NoPreparation);

    /// @brief Creates a backup object to backup this database from a source database.
    ///
    /// @details This method initializes a Backup object, which can then be used
    ///          to copy data between the source and destination databases.
    ///
    /// @param source A reference to the source Database to backup.
    /// @return A Backup object representing the backup operation.
    Backup backup(Database &source);

    /// @brief Returns the number of rows modified by the most recent statement.
    ///
    /// @details Corresponds to sqlite3_changes() or a similar API. If no rows were modified
    ///          or no modifying statement was executed, returns 0.
    ///
    /// @return The number of rows changed by the last INSERT, UPDATE, or DELETE.
    ssize_t changes() const;

    /// @brief Returns the rowid of the most recent successful INSERT.
    ///
    /// @details Corresponds to sqlite3_last_insert_rowid(). If no row was inserted,
    ///          this will typically be 0 or -1, depending on the implementation.
    ///
    /// @return The rowid of the last inserted row, or 0/-1 if none.
    ssize_t rowid() const;

    /// @brief Retrieves the most recent error or status information as a Result.
    ///
    /// @details If the last operation generated an error, the Result will contain
    ///          the appropriate error code, extended code, and message.
    /// @return A Result struct indicating the status of the last operation.
    Result error() const;

    /// @brief Closes the database connection.
    ///
    /// @details Resets the internal shared pointer, effectively closing the database
    ///          connection if it is open. Further operations on this Database object
    ///          (except open) will fail or be no-ops.
    inline void close() { _impl = {}; }

    /// @brief Checks if the database connection is currently valid.
    ///
    /// @details Returns true if _impl holds a valid pointer, meaning the database
    ///          is open or at least partially initialized.
    ///
    /// @return True if the database is open, otherwise false.
    inline operator bool() const { return bool(_impl); }

private:
    /// @brief A shared pointer to implementation details (e.g., sqlite3*).
    ///
    /// @details This allows for safe resource management and copy semantics
    ///          while ensuring the SQLite connection is closed when this
    ///          object is destroyed or reset.
    std::shared_ptr<void> _impl;
};

/// @brief Represents a prepared SQL statement associated with a SQLite database.
///
/// @details A Statement is created by calling Database::prepare() with an SQL
/// query string. You can bind parameters, step through results, and retrieve
/// column data via this class. Once you have finished using a Statement, you
/// can call close() to release resources explicitly; otherwise, it will be
/// automatically released when the Statement object goes out of scope.
class Statement
{
    /// @brief Default constructor that creates an invalid Statement.
    ///
    /// @details This constructor is used internally and typically not called
    ///          directly. An invalid Statement has no underlying SQLite
    ///          resources, so calls to its member functions will fail or be
    ///          no-ops.
    inline Statement() {}

    /// @brief Constructs a Statement with the given internal implementation pointer
    ///        and associated Database.
    ///
    /// @details This constructor is used internally by Database::prepare().
    ///          The shared pointer @p impl manages the lifetime of the compiled
    ///          SQLite statement. The @p database is the Database object that
    ///          created this Statement.
    ///
    /// @param impl     A shared pointer to the internal SQLite statement object (sqlite3_stmt*).
    /// @param database The Database object that created this statement.
    inline Statement(std::shared_ptr<void> impl, Database database) : _impl(impl), _database(database) {}

public:
    /// @brief Executes the next step of the prepared statement.
    ///
    /// @details If the statement is a SELECT query, each call to step() advances
    ///          to the next row, if any. For statements like INSERT or UPDATE, step()
    ///          runs the statement. After the last row or completion, step() will
    ///          return false, and finished() becomes true.
    ///
    /// @return True if there is another row of data available, otherwise false.
    bool step();

    /// @brief Resets the statement to its initial state.
    ///
    /// @details This allows the statement to be executed again from the beginning,
    ///          with any bound parameters retained unless explicitly unbound by an
    ///          implementation detail. Returns true on success, false otherwise.
    ///
    /// @return True if the reset was successful, otherwise false.
    bool reset();

    /// @brief Retrieves the number of columns in the result set of this statement.
    ///
    /// @details Useful for understanding how many columns to fetch using column()
    ///          or value() when stepping through a query result.
    ///
    /// @return The total number of columns in the result set.
    size_t columns();

    /// @brief Retrieves the name of the column at the specified index.
    ///
    /// @details This typically returns the column name as defined in the query or
    ///          the underlying table schema. If the index is out of range,
    ///          behavior may be undefined or an empty string may be returned.
    ///
    /// @param index Zero-based index of the column.
    /// @return The name of the column at @p index.
    String column(size_t index);

    /// @brief Retrieves the Value object from the column at the specified index.
    ///
    /// @details Each call to step() may yield a row of data. value() extracts a
    ///          typed representation (e.g., integer, string, etc.) from the
    ///          column at @p column. If @p column is out of range, behavior may
    ///          be undefined.
    ///
    /// @param column Zero-based index of the column.
    /// @return A Value representing the data in that column.
    Value value(size_t column);

    /// @brief Retrieves all column data for the current row as an Object.
    ///
    /// @details The returned Object typically maps column names to their Value.
    ///          If the query has N columns, the returned Object will have N entries.
    ///
    /// @return An Object containing column name-value pairs for the current row.
    Object values();

    /// @brief Binds a Value to the parameter at the specified 1-based index.
    ///
    /// @details SQLite binds parameters by index (1-based) in the SQL statement.
    ///          For example, the first parameter `?1` has an index of 1, `?2` has 2, etc.
    ///          Returns false if binding fails or the index is out of range.
    ///
    /// @param index The 1-based parameter index in the SQL statement.
    /// @param value The Value to bind to the statement parameter.
    /// @return True if the binding was successful, otherwise false.
    bool bind(size_t index, const Value &value);

    /// @brief Binds a Value to the named parameter in the SQL statement.
    ///
    /// @details Named parameters in SQL typically appear as :name or @name.
    ///          If the named parameter cannot be found, binding fails.
    ///
    /// @param name The parameter name (including any prefix like ':').
    /// @param value The Value to bind to the statement parameter.
    /// @return True if the binding was successful, otherwise false.
    bool bind(const String &name, const Value &value);

    /// @brief Binds a Blob to the parameter at the specified 1-based index.
    ///
    /// @details Similar to bind(size_t, const Value&), but treats the data as a BLOB.
    ///          Returns false if binding fails or the index is out of range.
    ///
    /// @param index The 1-based parameter index in the SQL statement.
    /// @param value The Blob wrapper containing binary data.
    /// @return True if the binding was successful, otherwise false.
    bool bind(size_t index, const Blob &value);

    /// @brief Binds a Blob to the named parameter in the SQL statement.
    ///
    /// @details Named parameters in SQL typically appear as :name or @name.
    ///          If the named parameter cannot be found, binding fails.
    ///
    /// @param name The parameter name (including any prefix like ':').
    /// @param value The Blob wrapper containing binary data.
    /// @return True if the binding was successful, otherwise false.
    bool bind(const String &name, const Blob &value);

    /// @brief Returns the number of rows changed by the most recent statement on this connection.
    ///
    /// @details Equivalent to calling Database::changes() on the associated database.
    ///
    /// @return The number of rows changed by the last INSERT, UPDATE, or DELETE.
    inline ssize_t changes() const { return _database.changes(); }

    /// @brief Returns the rowid of the most recent successful INSERT on this connection.
    ///
    /// @details Equivalent to calling Database::rowid() on the associated database.
    ///
    /// @return The rowid of the last inserted row, or 0/-1 if none.
    inline ssize_t rowid() const { return _database.rowid(); }

    /// @brief Retrieves the last error or status result of the associated database.
    ///
    /// @details If an error occurred during the previous database operation, it
    ///          will be reflected here.
    ///
    /// @return A Result object containing the error code, extended code, and message.
    inline Result error() const { return _database.error(); }

    /// @brief Closes the Statement, releasing any SQLite resources.
    ///
    /// @details After close() is called, this Statement is no longer valid for stepping
    ///          or binding parameters. Further calls to step() or bind() will fail.
    inline void close() { _impl = {}; }

    /// @brief Indicates whether the statement has completed execution.
    ///
    /// @details A statement is considered finished once step() returns false (e.g., no more rows,
    ///          or the statement has fully executed).
    ///
    /// @return True if the statement is finished, otherwise false.
    inline bool finished() const { return _finished; }

    /// @brief Checks if this Statement is valid and the associated Database is open.
    ///
    /// @details Returns true if the underlying Database object is valid. This does
    ///          not necessarily mean the statement is in a valid state for stepping,
    ///          but simply that the database connection is open.
    ///
    /// @return True if the Statement is associated with a valid Database, otherwise false.
    inline operator bool() const { return bool(_database); }

private:
    friend class Database;  ///< Allows Database to construct and manage Statement objects.

    bool _finished = false;       ///< Indicates whether the statement has completed execution.
    std::shared_ptr<void> _impl;  ///< Internal pointer to the SQLite statement (sqlite3_stmt*).
    Database _database;           ///< The Database object that created this Statement.
};

/// @brief Represents a transactional scope for SQLite database operations.
///
/// @details A Transaction is typically started by calling Database::transaction(). Once
///          created, any subsequent database operations take place within this transaction.
///          When the Transaction object goes out of scope or is destroyed, if it has not
///          been explicitly committed, it will roll back automatically (depending on
///          implementation) to ensure data integrity.
class Transaction
{
    /// @brief Default constructor that creates an invalid Transaction.
    ///
    /// @details This constructor is used internally and typically not called
    ///          directly. An invalid Transaction is not associated with any
    ///          Database, so operations on it will not have any effect.
    inline Transaction() {}

    /// @brief Constructs a Transaction associated with the given Database.
    ///
    /// @details This constructor is used internally by Database::transaction().
    ///          If the transaction cannot start (for example, due to an error),
    ///          the returned Transaction may be invalid.
    ///
    /// @param database A reference to a valid Database object.
    inline Transaction(Database &database) : _database(database) {}

public:
    /// @brief Destructor that attempts to roll back the transaction if uncommitted.
    ///
    /// @details When the Transaction object is destroyed, if it has not been
    ///          explicitly committed with commit(), it will roll back automatically.
    ///          This behavior may vary based on implementation; some wrappers might
    ///          silently ignore uncommitted changes or always roll back.
    ~Transaction();

    /// @brief Commits the transaction, making all changes permanent.
    ///
    /// @details After commit() is called, the transaction is considered complete,
    ///          and no further operations can be performed on this Transaction object.
    ///
    /// @return True if the transaction was successfully committed, otherwise false.
    bool commit();

    /// @brief Retrieves the last error or status result of the associated database.
    ///
    /// @details If an error occurred during the transaction or related database
    ///          operations, it will be reflected here.
    ///
    /// @return A Result object containing the error code, extended code, and message.
    inline Result error() const { return _database.error(); }

    /// @brief Indicates whether this transaction has already been committed.
    ///
    /// @details Once a transaction is committed, further calls to commit() will
    ///          have no effect, and the destructor will not attempt a rollback.
    ///
    /// @return True if the transaction was committed, otherwise false.
    inline bool commited() const { return _commited; }

    /// @brief Checks if this Transaction is associated with a valid Database.
    ///
    /// @details Returns true if the underlying database reference is valid
    ///          (i.e., the database is open).
    ///
    /// @return True if the transaction is valid and the database is open, otherwise false.
    inline operator bool() const { return bool(_database); }

private:
    friend class Database;  ///< Allows Database to construct and manage Transaction objects.

    bool _commited = false;  ///< Indicates whether commit() has been successfully called.
    Database _database;      ///< The Database this Transaction is associated with.
};

/// @brief Represents an ongoing backup process between two SQLite databases.
///
/// @details A Backup object is created by calling Database::backup(), specifying the
///          source and destination databases. You can then call step() to copy a number
///          of pages at a time until the backup is complete. Once finished, close()
///          or destruction of the object will release resources.
class Backup
{
    /// @brief Default constructor that creates an invalid Backup object.
    ///
    /// @details This constructor is for internal use and typically not called directly.
    ///          An invalid Backup has no underlying SQLite resources, so calls to
    ///          step() or other methods will be no-ops or fail.
    inline Backup() {}

    /// @brief Constructs a Backup object with the given internal implementation pointer
    ///        and references to the source and destination Databases.
    ///
    /// @details This constructor is called internally by Database::backup().
    ///          @p impl manages the lifetime of the SQLite backup structure.
    ///
    /// @param impl   A shared pointer to the internal SQLite backup object (sqlite3_backup*).
    /// @param source The source Database from which data is being backed up.
    /// @param destin The destination Database to which data is being backed up.
    inline Backup(std::shared_ptr<void> impl, Database &source, Database &destin)
        : _impl(impl), _source(source), _destin(destin)
    {
    }

public:
    /// @brief Copies a specified number of pages from the source database to the destination database.
    ///
    /// @details If @p pages is set to -1, it attempts to copy all remaining pages in one step.
    ///          The backup is considered complete when there are no more pages to copy
    ///          or if an error occurs. After the backup is finished, step() returns false
    ///          and finished() will be true.
    ///
    /// @param pages The maximum number of pages to copy in this step. Default is -1 (copy all remaining pages).
    /// @return True if there are still more pages to copy, false if the backup is finished or an error occurred.
    bool step(int pages = -1);

    /// @brief Retrieves the most recent error or status result from the destination database.
    ///
    /// @details If an error occurs during the backup, it will be reflected here,
    ///          as errors are typically reported on the destination database.
    ///
    /// @return A Result object containing the error code, extended code, and message.
    inline Result error() const { return _destin.error(); }

    /// @brief Indicates whether the backup has completed.
    ///
    /// @details The backup is considered finished if step() has transferred all pages
    ///          successfully or encountered a fatal error.
    ///
    /// @return True if the backup process is no longer active, otherwise false.
    inline bool finished() const { return _finished; }

    /// @brief Checks if this Backup object is associated with a valid destination Database.
    ///
    /// @details Returns true if the underlying destination Database is valid (i.e.,
    ///          the database is open). This does not necessarily mean the backup is
    ///          active or unfinished.
    ///
    /// @return True if the destination database connection is valid, otherwise false.
    inline operator bool() const { return bool(_destin); }

    /// @brief Closes the Backup, releasing any associated SQLite resources.
    ///
    /// @details After close() is called, this Backup object is invalid for further use.
    ///          If the backup was not finished, the remaining pages will not be copied.
    inline void close() { _impl = {}, _source = {}, _destin = {}; }

private:
    friend class Database;  ///< Allows Database to construct and manage Backup objects.

    bool _finished = false;       ///< Indicates if the backup process has completed or encountered an error.
    std::shared_ptr<void> _impl;  ///< Internal pointer to the SQLite backup object (sqlite3_backup*).
    Database _source;             ///< The source Database for the backup.
    Database _destin;             ///< The destination Database for the backup.
};

/// @brief Writes an Error enumerator to the provided output stream.
///
/// @details This operator prints a textual representation of the specified
///          Error enumerator. If the enumerator is recognized, it prints the
///          name in the form "Error::<EnumName>". Otherwise, it prints
///          "Error::<unknown>".
///
/// @param os     The output stream to write to.
/// @param error  The Error enumerator value to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, Error error);

/// @brief Writes an Extended enumerator to the provided output stream.
///
/// @details This operator prints a textual representation of the specified
///          Extended enumerator. If the enumerator is recognized, it prints the
///          name in the form "Extended::<EnumName>". Otherwise, it prints
///          "Extended::<unknown>".
///
/// @param os   The output stream to write to.
/// @param ext  The Extended enumerator value to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, Extended ext);
}  // namespace SQLite
}  // namespace Neyson

#endif
