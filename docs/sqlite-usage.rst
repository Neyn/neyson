SQLite Usage Tutorial
=====================

.. contents::
   :depth: 2
   :local:

Introduction
------------
The **Neyson::SQLite** namespace provides a wrapper around the standard SQLite API, offering an idiomatic C++ interface for database connections, statement preparation, transactions, and backups. Each database is encapsulated in a **Database** object, and you can manage your queries with **Statement** objects. The library also provides helpful classes for **Transaction** (to handle atomic changes) and **Backup** (to copy data between databases). Furthermore, the interface uses **Result** structures to indicate errors and statuses, and enumerations like **Error** and **Extended** to give meaningful feedback about what went wrong or succeeded.

Prerequisites
-------------
1. **NEYSON_ENABLE_SQLITE**: The SQLite interface is conditionally compiled. Make sure you enable the `NEYSON_ENABLE_SQLITE` option in CMake.
2. **SQLite Library**: You must have a valid SQLite library in your system (foundt through `find_package(SQLite3)` in the library's CMake file).
3. **Neyson Value Class**: The SQLite wrapper uses **Value** objects for parameter binding and result retrieval.

Key Components
--------------
- **Error**: Main SQLite error codes (OK, Error, Busy, etc.).
- **Extended**: Extended error codes offering more detailed reasons for failures.
- **Result**: Combines main and extended error codes plus a message for additional context.
- **Database**: Manages a connection to a single SQLite database (files, memory, or URIs).
- **Statement**: Represents a prepared SQL statement; you can bind parameters, step through results, and retrieve data.
- **Transaction**: Encapsulates a transaction scope (BEGIN, COMMIT, ROLLBACK).
- **Backup**: Copies data between two database connections (source, destination).

Setting Up and Opening a Database
---------------------------------
To use the SQLite interface, include the library's header and ensure the `NEYSON_ENABLE_SQLITE` option is enabled in CMake.

.. code-block:: c++

   #include <neyson/neyson.h>
   using namespace Neyson;

   int main() {
       SQLite::Database db;

       // Open a database in read-write mode, creating it if not found
       bool opened = db.open("example.db", SQLite::Database::ReadWrite | SQLite::Database::Create);
       if (!opened) {
           // If opening fails, retrieve the error
           auto err = db.error();
           std::cerr << "Failed to open database! Error: " << err.error
                     << ", Extended: " << err.extended
                     << ", Message: " << err.message << std::endl;
           return 1;
       }

       // If success, we can now use the db object for queries
       // ...
       return 0;
   }

Database::open
~~~~~~~~~~~~~~
.. code-block:: c++

   bool open(const String &path, int flags = Flags::ReadWrite | Flags::Create);

- **path**: The file path (or URI if `URI` flag is used) to the database.
- **flags**: Bitwise OR of **Database::Flags** (e.g., ReadWrite, Create, Memory, etc.).

Typical combinations include:
- `ReadOnly` for read-only access.
- `ReadWrite | Create` to open for read/write and create if missing.
- `Memory` for an in-memory database (not persisted to disk).

Executing Simple Queries
------------------------
You can execute arbitrary SQL statements with the **Database::execute** function. It supports both statements that return no data (such as `CREATE TABLE`) and those that produce rows (like `SELECT`), with an optional callback.

.. code-block:: c++

   bool execute(const String &query, std::function<bool(Object &)> step = {});

When a query returns rows, you can supply a lambda or function as a second parameter. Each row is delivered as an **Object** where keys are column names and values are **Value** objects.

Example: Create Table and Insert
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++

   if (!db.execute("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT);")) {
       std::cerr << "Failed to create table: " << db.error().message << std::endl;
   }

   // Insert a row
   if (!db.execute("INSERT INTO users (name) VALUES ('Alice');")) {
       std::cerr << "Insert failed: " << db.error().message << std::endl;
   }

   // Read data with callback
   db.execute("SELECT id, name FROM users;", [&](Object &row) {
       std::cout << "ID: "   << row["id"]   << ", "
                 << "Name: " << row["name"] << std::endl;
       // Return true to continue stepping through rows, false to stop
       return true;
   });

Prepared Statements
-------------------
For more complex or repeated queries, **Statement** objects are used. They allow parameter binding (both positional and named) and row-by-row stepping.

Database::prepare
~~~~~~~~~~~~~~~~~
.. code-block:: c++

   Statement prepare(const String &query, int prepare = Prepare::NoPreparation);

Use `Database::prepare` to compile a SQL statement. You can optionally supply flags like `Persistent`, `Normalize`, or `NoVTab`.

Example: SELECT with a Prepared Statement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++

   // Prepare a statement
   auto stmt = db.prepare("SELECT id, name FROM users WHERE id > ?1;");
   if (!stmt) {
       std::cerr << "Preparation failed: " << db.error().message << std::endl;
   } else {
       // Bind parameter: ?1 = 10
       stmt.bind(1, 10);

       // Step through rows
       while (stmt.step()) {
           // We can get data by column index
           auto id = stmt.value(0).integer();
           auto name = stmt.value(1).string();
           std::cout << "User: " << id << " - " << name << std::endl;
       }

       // Reset the statement to reuse it
       stmt.reset();
   }

Statement Methods
~~~~~~~~~~~~~~~~~
- **bool step()**: Moves to the next row of the result set. Returns `true` if a row is available, `false` otherwise.
- **bool reset()**: Resets the statement so it can be re-executed (if the SQL allows).
- **size_t columns()**: Number of columns in the result set.
- **String column(size_t index)**: Column name at position `index`.
- **Value value(size_t column)**: Retrieves the data in the specified column as a **Value**.
- **Object values()**: Returns all columns for the current row as a name-value map (keys are column names).
- **bind(size_t index, const Value &value)**: Binds a **Value** to a 1-based parameter index (`?1`, `?2`, etc.).
- **bind(const String &name, const Value &value)**: Binds to a named parameter (like `:param_name`).
- **bind(size_t index, const Blob &value)**, **bind(const String &name, const Blob &value)**: Binds BLOB data.
- **void close()**: Explicitly closes the statement (called automatically if the Statement goes out of scope).

Transactions
------------
Use the **Transaction** class to group statements into an atomic unit of work. A transaction is started with:

.. code-block:: c++

   Transaction transaction();

This returns a **Transaction** object, which will automatically roll back changes if not committed. Once committed, changes are permanent.

Example: Transaction Usage
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++

   {
       auto tx = db.transaction();
       // Insert multiple rows
       db.execute("INSERT INTO users (name) VALUES ('Bob');");
       db.execute("INSERT INTO users (name) VALUES ('Charlie');");

       // Check if everything is okay
       if (!tx.commit()) {
           std::cerr << "Transaction commit failed: " << tx.error().message << std::endl;
           // Transaction will roll back automatically on destruction if uncommitted
       }
   }  // If commit() was not called, a rollback is performed here

Backup
------
The **Backup** class allows you to copy data between two **Database** instances. Typically, you create a backup from one database to another by calling:

.. code-block:: c++

   Backup backup(Database &source);

Then you use **backup.step()** to copy pages from the source to the current database.

Example: Creating a Backup
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++

   // Open a source db
   SQLite::Database srcDB;
   srcDB.open("source.db");

   // Open a destination db (in-memory, for example)
   SQLite::Database destDB;
   destDB.open(":memory:");

   // Start the backup from source -> destination
   auto bk = destDB.backup(srcDB);

   // Copy all pages in steps, for example
   while (bk.step(5)) {
       // Step in increments of 5 pages
   }

   if (bk.finished()) {
       std::cout << "Backup completed successfully!\n";
   } else {
       auto err = bk.error();
       std::cerr << "Backup failed: " << err.message << std::endl;
   }

Error Handling
--------------
All major operations in **Database**, **Statement**, **Transaction**, and **Backup** either return a boolean or a **Result** structure. You can retrieve detailed error information via:

.. code-block:: c++

   db.error();      // For Database
   stmt.error();    // For Statement (same as db.error())
   tx.error();      // For Transaction
   backup.error();  // For Backup (destination DB error typically)

The **Result** struct includes:
- **error**: Main **Error** code (e.g., OK, Busy, Error, etc.).
- **extended**: Additional details in the **Extended** enum.
- **message**: A string describing the issue, if available.

Example: Error Inspection
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++

   bool success = db.execute("DROP TABLE nonexistent_table");
   if (!success) {
       auto res = db.error();
       std::cerr << "Error: " << res.error         // E.g. Error::Error
                 << ", Extended: " << res.extended  // E.g. Extended::CorruptIndex
                 << ", Message: " << res.message    // Detailed message from SQLite
                 << std::endl;
   }

Blob Data
---------
To store or retrieve binary data (BLOBs), the library provides a **Blob** struct. You wrap a **Value** in a **Blob** when binding:

.. code-block:: c++

   Value blobData("binary stuff"); // For demonstration, not truly binary here
   SQLite::Blob blob(blobData);

   // Bind to statement param at index 1
   stmt.bind(1, blob);

When retrieving a BLOB, you can call `stmt.value(column)`, which gives you a **Value**. Interpreting that **Value** as raw bytes is up to your application code.

Practical Workflow
------------------
1. **Open** the database:

   .. code-block:: c++

       SQLite::Database db;
       db.open("mydb.db", SQLite::Database::ReadWrite | SQLite::Database::Create);

2. **Create table** (optional, if not already existing):

   .. code-block:: c++

       db.execute("CREATE TABLE IF NOT EXISTS logs (timestamp TEXT, level INTEGER, message TEXT);");

3. **Start a transaction** (optional if you need atomic behavior):

   .. code-block:: c++

       auto tx = db.transaction();

4. **Prepare a statement**:

   .. code-block:: c++

       auto stmt = db.prepare("INSERT INTO logs (timestamp, level, message) VALUES (?1, ?2, ?3);");

5. **Bind parameters** and **step**:

   .. code-block:: c++

       stmt.bind(1, "2025-04-04 12:00:00");
       stmt.bind(2, 2);
       stmt.bind(3, "Example log entry");
       stmt.step();

       // The statement is done executing. No rows to iterate for INSERT statements.
       // If you want, you can reset() and bind new values to reuse it.

6. **Commit** the transaction:

   .. code-block:: c++

       if (!tx.commit()) {
           std::cerr << "Transaction commit failed: " << tx.error().message << std::endl;
       }

7. **Query data**:

   .. code-block:: c++

       stmt = db.prepare("SELECT * FROM logs;");
       while (stmt.step()) {
           auto row = stmt.values();
           std::cout << "Timestamp: " << row["timestamp"].string()
                     << ", Level: " << row["level"].integer()
                     << ", Msg: "   << row["message"].string()
                     << std::endl;
       }

8. **Close** the database or let it close automatically upon destruction:

   .. code-block:: c++

       db.close();  // Optional, automatically closed on destruction

Summary
-------
Using **Neyson::SQLite**, you can easily manage SQLite databases in a C++ project, taking advantage of:
- **Value**-based parameter binding and result retrieval.
- Simple transaction handling with **Transaction**.
- Convenient data backup between databases using **Backup**.
- Detailed error reporting via **Result**, **Error**, and **Extended** enums.
