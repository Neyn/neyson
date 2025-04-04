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

#ifdef NEYSON_ENABLE_SQLITE

#include "tests.h"

TEST(SQLite, Database)
{
    using namespace SQLite;

    remove("test.sqlite");
    Database database;
    EXPECT_TRUE(!database.open("test.sqlite", Database::ReadWrite));
    EXPECT_TRUE(database.open("test.sqlite"));
    EXPECT_TRUE(database.open(":memory:"));

    bool found = false;
    auto func = [&](Object &values) { return (found = true, false); };
    EXPECT_TRUE(!database.execute("CRATE TABLE test(name TEXT, value INTEGER);"));
    EXPECT_TRUE(database.execute("CREATE TABLE test(name TEXT, value INTEGER);"));
    EXPECT_TRUE(database.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='test';", func));
    EXPECT_TRUE(found);
}

TEST(SQLite, Statement)
{
    using namespace SQLite;

    Database database;
    EXPECT_TRUE(database.open(":memory:"));
    EXPECT_TRUE(database.execute(
        "CREATE TABLE test(null_name, int_name INTEGER, real_name REAL, text_name TEXT, blob_name BLOB);"));

    bool found = false;
    auto func1 = [&](Object &values) { return (found = true, false); };
    EXPECT_TRUE(database.execute("SELECT * FROM test;", func1));
    EXPECT_TRUE(!found);

    auto stmt = database.prepare(
        "INSERT INTO test (null_name, int_name, real_name, text_name, blob_name) VALUES(?, ?, ?, ?, ?);");
    EXPECT_TRUE(stmt);
    stmt.bind(1, {});
    stmt.bind(2, 10);
    stmt.bind(3, 10.01);
    stmt.bind(4, "Hello");
    stmt.bind(5, Blob("Good Day"));
    EXPECT_TRUE(!stmt.step());
    EXPECT_TRUE(stmt.error());
    EXPECT_TRUE(stmt.changes() == 1);
    EXPECT_TRUE(stmt.rowid() == 1);

    auto func2 = [&](Object &values) {
        EXPECT_TRUE(values["rowid"] == Value(0));
        EXPECT_TRUE(values["null_name"] == Value());
        EXPECT_TRUE(values["int_name"] == Value(10));
        EXPECT_TRUE(values["real_name"] == Value(10.01));
        EXPECT_TRUE(values["text_name"] == Value("Hello"));
        EXPECT_TRUE(values["blob_name"] == Value("Good Day"));
        EXPECT_TRUE(values.size() == 6);
        return true;
    };
    EXPECT_TRUE(database.execute("SELECT rowid, null_name, int_name, real_name, text_name, blob_name FROM test;"));

    stmt = database.prepare("UPDATE test SET null_name=?;");
    EXPECT_TRUE(stmt);
    stmt.bind(1, -1);
    EXPECT_TRUE(!stmt.step());
    EXPECT_TRUE(stmt.error());
    EXPECT_TRUE(stmt.changes() == 1);

    stmt = database.prepare("SELECT rowid, null_name, int_name, real_name, text_name, blob_name FROM test;");
    EXPECT_TRUE(stmt);
    EXPECT_TRUE(stmt.step());
    Object result{{"rowid", 1},         {"null_name", -1},      {"int_name", 10},
                  {"real_name", 10.01}, {"text_name", "Hello"}, {"blob_name", "Good Day"}};
    EXPECT_TRUE(stmt.values() == result);
    EXPECT_TRUE(!stmt.step());
    EXPECT_TRUE(stmt.reset());
    EXPECT_TRUE(stmt.step());
    EXPECT_TRUE(stmt.values() == result);
    EXPECT_TRUE(!stmt.step());

    stmt = database.prepare("DELETE FROM test WHERE rowid=?;");
    EXPECT_TRUE(stmt);
    stmt.bind(1, 1);
    EXPECT_TRUE(!stmt.step());
    EXPECT_TRUE(stmt.error());
    EXPECT_TRUE(stmt.changes() == 1);
}

TEST(SQLite, Transaction)
{
    using namespace SQLite;

    Database database;
    EXPECT_TRUE(database.open(":memory:"));
    EXPECT_TRUE(database.execute("CREATE TABLE test(first INTEGER, second TEXT);"));

    {
        auto transaction = database.transaction();
        EXPECT_TRUE(database.execute("INSERT INTO test (first, second) VALUES(10, 'Hello');"));
        EXPECT_TRUE(database.execute("INSERT INTO test (first, second) VALUES(11, 'Bye');"));
        transaction.commit();

        EXPECT_TRUE(database.execute("SELECT COUNT(*) as cnt FROM test;", [](Object &values) {
            EXPECT_TRUE(values["cnt"] = 2);
            return true;
        }));

        auto stmt = database.prepare("SELECT * FROM test;");
        EXPECT_TRUE(stmt);
        EXPECT_TRUE(stmt.step());
        Object values{{"first", 10}, {"second", "Hello"}};
        EXPECT_TRUE(stmt.values() == values);
        EXPECT_TRUE(stmt.step());
        values = {{"first", 11}, {"second", "Bye"}};
        EXPECT_TRUE(stmt.values() == values);
        EXPECT_TRUE(!stmt.step());
    }

    EXPECT_TRUE(database.execute("DELETE FROM test;"));
    EXPECT_TRUE(database.changes() == 2);

    {
        auto transaction = database.transaction();
        EXPECT_TRUE(database.execute("INSERT INTO test (first, second) VALUES(10, 'Hello');"));
        EXPECT_TRUE(database.execute("INSERT INTO test (first, second) VALUES(11, 'Bye');"));
    }

    auto stmt = database.prepare("SELECT * FROM test;");
    EXPECT_TRUE(stmt);
    EXPECT_TRUE(!stmt.step());
}

TEST(SQLite, Backup)
{
    using namespace SQLite;
    auto count = 100;

    Database old;
    old.open(":memory:");
    EXPECT_TRUE(old.execute("CREATE TABLE test(rand_text TEXT);"));
    for (size_t i = 0; i < count; ++i) old.execute("INSERT INTO test (rand_text) VALUES('" + randstr() + "');");

    EXPECT_TRUE(old.execute("SELECT COUNT(*) as cnt FROM test;", [&](Object &values) {
        EXPECT_TRUE(values["cnt"] = count);
        return true;
    }));

    Database now;
    now.open(":memory:");
    auto backup = now.backup(old);
    EXPECT_TRUE(backup);
    backup.step();

    auto stmt1 = old.prepare("SELECT * FROM test;");
    auto stmt2 = now.prepare("SELECT * FROM test;");

    for (size_t i = 0; i < count; ++i)
    {
        EXPECT_TRUE(stmt1.step());
        EXPECT_TRUE(stmt2.step());
        EXPECT_TRUE(stmt1.values() == stmt2.values());
    }
    EXPECT_TRUE(!stmt1.step());
    EXPECT_TRUE(!stmt2.step());
}

static SQLite::Database createInMemoryDatabase()
{
    using namespace SQLite;

    Database db;
    bool opened = db.open(":memory:", Database::ReadWrite | Database::Create);
    // We rely on the test below to ensure it really opened
    EXPECT_TRUE(opened);

    bool execOk = db.execute("CREATE TABLE test (id INTEGER PRIMARY KEY, name TEXT, value REAL)");
    EXPECT_TRUE(execOk);
    return db;
}

TEST(SQLiteTests, OpenCloseFileDatabase)
{
    using namespace SQLite;

    Database db;
    bool opened = db.open("test_file.db", Database::ReadWrite | Database::Create);
    EXPECT_TRUE(opened);  // Expect to open successfully

    // Should be valid now
    EXPECT_TRUE(static_cast<bool>(db));

    db.close();
    EXPECT_FALSE(static_cast<bool>(db));

    // Optional: remove("test_file.db");
}

TEST(SQLiteTests, OpenInvalidPath)
{
    using namespace SQLite;

    Database db;
    bool opened = db.open("/root/non_existent_dir/invalid.db", Database::ReadWrite);
    // Expect that it fails to open
    EXPECT_FALSE(opened);

    auto err = db.error();
    // Typically, we'd expect CantOpen here
    EXPECT_EQ(err.error, Error::CantOpen);
}

TEST(SQLiteTests, ExecuteWithCallback)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    bool ok1 = db.execute("INSERT INTO test (name, value) VALUES ('alice', 1.1)");
    EXPECT_TRUE(ok1);

    bool ok2 = db.execute("INSERT INTO test (name, value) VALUES ('bob', 2.2)");
    EXPECT_TRUE(ok2);

    // Gather names and values to verify callback
    std::vector<std::string> names;
    std::vector<double> values;

    auto callback = [&](Object &row) -> bool {
        if (row.find("name") != row.end())
        {
            names.push_back(row["name"].string());
        }
        if (row.find("value") != row.end())
        {
            values.push_back(double(row["value"]));
        }
        return true;  // continue for all rows
    };

    bool selectOk = db.execute("SELECT name, value FROM test ORDER BY id", callback);
    EXPECT_TRUE(selectOk);
    EXPECT_EQ(names.size(), 2u);
    EXPECT_EQ(names[0], "alice");
    EXPECT_EQ(names[1], "bob");
    EXPECT_EQ(values.size(), 2u);
    EXPECT_NEAR(values[0], 1.1, 1e-8);
    EXPECT_NEAR(values[1], 2.2, 1e-8);
}

TEST(SQLiteTests, StatementUsage)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    {
        // Insert
        Statement stmt = db.prepare("INSERT INTO test (name, value) VALUES (?, ?)");
        EXPECT_TRUE(static_cast<bool>(stmt));

        bool b1 = stmt.bind(1, Value("charlie"));
        bool b2 = stmt.bind(2, Value(3.3));
        EXPECT_TRUE(b1);
        EXPECT_TRUE(b2);

        EXPECT_FALSE(stmt.step());
        EXPECT_TRUE(stmt.finished());
        EXPECT_EQ(stmt.changes(), 1);
        EXPECT_GT(stmt.rowid(), 0);
    }

    // Insert another row
    db.execute("INSERT INTO test (name, value) VALUES ('david', 4.4)");

    {
        Statement stmt = db.prepare("SELECT id, name, value FROM test ORDER BY id");
        EXPECT_TRUE(static_cast<bool>(stmt));

        size_t rowCount = 0;
        while (stmt.step())
        {
            EXPECT_EQ(stmt.columns(), 3u);

            // Read each column or get an entire row
            Object row = stmt.values();
            EXPECT_EQ(row.size(), 3u);

            rowCount++;
        }
        EXPECT_GT(rowCount, 0u);
        EXPECT_TRUE(stmt.finished());
    }
}

TEST(SQLiteTests, StatementReset)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    Statement stmt = db.prepare("INSERT INTO test (name, value) VALUES (?, ?)");
    EXPECT_TRUE(static_cast<bool>(stmt));

    stmt.bind(1, Value("eve"));
    stmt.bind(2, Value(5.5));
    EXPECT_FALSE(stmt.step());
    EXPECT_TRUE(stmt.finished());
    EXPECT_EQ(stmt.changes(), 1);

    bool resetOk = stmt.reset();
    EXPECT_TRUE(resetOk);
    EXPECT_FALSE(stmt.finished());

    stmt.bind(1, Value("frank"));
    stmt.bind(2, Value(6.6));
    EXPECT_FALSE(stmt.step());
    EXPECT_TRUE(stmt.finished());
    EXPECT_EQ(stmt.changes(), 1);
}

TEST(SQLiteTests, StatementNamedBindings)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    Statement stmt = db.prepare("INSERT INTO test (name, value) VALUES (:name, :val)");
    EXPECT_TRUE(static_cast<bool>(stmt));

    bool nameBound = stmt.bind(":name", Value("george"));
    bool valBound = stmt.bind(":val", Value(7.7));
    EXPECT_TRUE(nameBound);
    EXPECT_TRUE(valBound);

    bool stepped = stmt.step();
    EXPECT_FALSE(stepped);
    EXPECT_TRUE(stmt.finished());

    int count = 0;
    db.execute("SELECT COUNT(*) AS cnt FROM test WHERE name='george'", [&](Object &row) {
        count = row["cnt"].toint();
        return false;
    });
    EXPECT_EQ(count, 1);
}

TEST(SQLiteTests, StatementBlobBinding)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    bool alterOk = db.execute("ALTER TABLE test ADD COLUMN data BLOB");
    EXPECT_TRUE(alterOk);

    Statement stmt = db.prepare("INSERT INTO test (name, value, data) VALUES (?, ?, ?)");
    EXPECT_TRUE(static_cast<bool>(stmt));

    std::string blobData = "binary\0data";
    Value blobValue(blobData);
    Blob blob(blobValue);

    stmt.bind(1, Value("harry"));
    stmt.bind(2, Value(8.8));
    stmt.bind(3, blob);

    EXPECT_FALSE(stmt.step());
    EXPECT_TRUE(stmt.finished());
    EXPECT_EQ(stmt.changes(), 1);
}

TEST(SQLiteTests, StatementErrorReporting)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    // Attempt to prepare an invalid statement
    Statement badStmt = db.prepare("INSERT INTO no_such_table (x) VALUES (1)");

    // Depending on your implementation, prepare might return a Statement
    // that fails on step(), or might fail immediately
    Result prepResult = badStmt.error();
    if (!static_cast<bool>(prepResult))
    {
        // We have an error from prepare
        // Possibly Error::Error or Error::NotFound, etc.
        // We'll just confirm it's not OK
        EXPECT_NE(prepResult.error, Error::OK);
    }
    else
    {
        // If it didn't fail on prepare, maybe it will fail on step
        bool stepped = badStmt.step();
        EXPECT_FALSE(stepped);
        Result stepResult = badStmt.error();
        EXPECT_NE(stepResult.error, Error::OK);
    }
}

TEST(SQLiteTests, DatabaseChangesAndRowID)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    // Insert single row
    bool ins1 = db.execute("INSERT INTO test (name, value) VALUES ('ivy', 9.9)");
    EXPECT_TRUE(ins1);
    EXPECT_EQ(db.changes(), 1);
    ssize_t firstRowID = db.rowid();
    EXPECT_GT(firstRowID, 0);

    // Insert two more rows
    bool ins2 = db.execute("INSERT INTO test (name, value) VALUES ('jack', 10.1)");
    EXPECT_TRUE(ins2);
    bool ins3 = db.execute("INSERT INTO test (name, value) VALUES ('kate', 11.2)");
    EXPECT_TRUE(ins3);

    // Usually reflects the last operation's changes
    EXPECT_EQ(db.changes(), 1);

    ssize_t secondRowID = db.rowid();
    EXPECT_GT(secondRowID, firstRowID);
}

TEST(SQLiteTests, TransactionCommit)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    {
        Transaction tx = db.transaction();
        EXPECT_TRUE(static_cast<bool>(tx));

        db.execute("INSERT INTO test (name, value) VALUES ('transaction', 12.3)");
        bool committed = tx.commit();
        EXPECT_TRUE(committed);
        EXPECT_TRUE(tx.commited());
    }

    // After commit, data should persist
    int count = 0;
    db.execute("SELECT COUNT(*) AS cnt FROM test WHERE name='transaction'", [&](Object &row) {
        count = row["cnt"].toint();
        return false;
    });
    EXPECT_EQ(count, 1);
}

TEST(SQLiteTests, TransactionRollback)
{
    using namespace SQLite;

    Database db = createInMemoryDatabase();

    {
        Transaction tx = db.transaction();
        EXPECT_TRUE(static_cast<bool>(tx));

        db.execute("INSERT INTO test (name, value) VALUES ('rollback_test', 999)");
        // No commit => rollback on destruction
    }

    int count = 0;
    db.execute("SELECT COUNT(*) AS cnt FROM test WHERE name='rollback_test'", [&](Object &row) {
        count = row["cnt"].toint();
        return false;
    });
    EXPECT_EQ(count, 0);
}

TEST(SQLiteTests, BackupDatabases)
{
    using namespace SQLite;

    Database source = createInMemoryDatabase();
    source.execute("INSERT INTO test (name, value) VALUES ('backup_source', 13.13)");

    Database dest;
    bool opened = dest.open(":memory:", Database::ReadWrite | Database::Create);
    EXPECT_TRUE(opened);

    // If your backup logic copies schema automatically, that's all we need
    Backup backupObj = dest.backup(source);
    EXPECT_TRUE(static_cast<bool>(backupObj));
    EXPECT_FALSE(backupObj.finished());

    bool stepped = backupObj.step();
    EXPECT_FALSE(stepped);
    EXPECT_TRUE(backupObj.finished());

    int count = 0;
    dest.execute("SELECT COUNT(*) as cnt FROM test WHERE name='backup_source'", [&](Object &row) {
        count = row["cnt"].toint();
        return false;
    });
    EXPECT_EQ(count, 1);

    backupObj.close();
    EXPECT_FALSE(static_cast<bool>(backupObj));
}

TEST(SQLiteTests, ErrorAndExtendedValues)
{
    using namespace SQLite;

    // Check basic usage of Error and Extended
    Result r;
    r.error = Error::Error;
    r.extended = Extended::ErrorMissingCollSeq;  // Example
    r.message = "Sample error text";

    // Not OK => operator bool() should be false
    EXPECT_FALSE(static_cast<bool>(r));
    EXPECT_EQ(r.error, Error::Error);
    EXPECT_EQ(r.extended, Extended::ErrorMissingCollSeq);
    EXPECT_EQ(r.message, "Sample error text");
}

#endif
