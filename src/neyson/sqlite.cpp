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

#include "sqlite.h"

#ifdef NEYSON_USE_SQLITE

#include <sqlite3.h>

#include <stdexcept>

#define DB (sqlite3 *)_impl.get()
#define ST (sqlite3_stmt *)_impl.get()
#define BU (sqlite3_backup *)_impl.get()

namespace Neyson
{
namespace SQLite
{
int exec_callback(void *data, int count, char **values, char **names)
{
    const auto &step = *(std::function<bool(Object &)> *)data;

    Object object;
    for (int i = 0; i < count; ++i)
    {
        auto value = values[i];
        object[names[i]] = value ? Value(value) : Value();
    }
    return step(object) ? 0 : 1;
}

bool Database::open(const String &path, int flags)
{
    sqlite3 *impl = nullptr;
    auto code = sqlite3_open_v2(path.c_str(), &impl, flags, nullptr);
    auto deleter = [](sqlite3 *impl) { sqlite3_close_v2(impl); };
    if (impl) _impl = std::shared_ptr<sqlite3>(impl, deleter);
    return code == SQLITE_OK;
}

bool Database::execute(const String &query, std::function<bool(Object &)> step)
{
    if (!_impl) return false;
    void *data = step ? &step : nullptr;
    auto func = step ? exec_callback : nullptr;
    auto code = sqlite3_exec(DB, query.c_str(), func, data, nullptr);
    return code == SQLITE_OK || (step && code == SQLITE_ABORT);
}

Transaction Database::transaction()
{
    return execute("BEGIN TRANSACTION;") ? Transaction(*this) : Transaction();  //
}

Statement Database::prepare(const String &query, int prepare)
{
    if (!_impl) return {};
    sqlite3_stmt *impl = nullptr;
    auto code = sqlite3_prepare_v3(DB, query.c_str(), query.size() + 1, prepare, &impl, nullptr);
    auto deleter = [](sqlite3_stmt *impl) { sqlite3_finalize(impl); };
    return code == SQLITE_OK ? Statement(std::shared_ptr<sqlite3_stmt>(impl, deleter), *this) : Statement();
}

Backup Database::backup(Database &source)
{
    if (!_impl) return {};
    auto impl = sqlite3_backup_init(DB, "main", (sqlite3 *)source._impl.get(), "main");
    auto deleter = [](sqlite3_backup *impl) { sqlite3_backup_finish(impl); };
    return impl ? Backup(std::shared_ptr<sqlite3_backup>(impl, deleter), source, *this) : Backup();
}

ssize_t Database::changes() const { return sqlite3_changes64(DB); }

ssize_t Database::rowid() const { return sqlite3_last_insert_rowid(DB); }

Result Database::error() const
{
    if (!_impl) return Result{Error::Unallocated, Extended(-1)};
    auto code = sqlite3_extended_errcode(DB);
    auto msg = sqlite3_errmsg(DB);
    return Result{Error(code & 0xFF), Extended(code), msg};
}

////////////////////////////////////////////////////////////////////////////////

bool Statement::step()
{
    if (_finished) return false;
    auto code = sqlite3_step(ST);
    if (code == SQLITE_DONE) _finished = true;
    return code == SQLITE_ROW;
}

bool Statement::reset()
{
    _finished = false;
    auto code = sqlite3_reset(ST);
    return code == SQLITE_OK;
}

size_t Statement::columns()
{
    return sqlite3_column_count(ST);  //
}

String Statement::column(size_t index)
{
    return sqlite3_column_name(ST, index);  //
}

Value Statement::value(size_t column)
{
    auto type = sqlite3_column_type(ST, column);

    if (type == SQLITE_NULL)
        return Value();
    else if (type == SQLITE_INTEGER)
        return sqlite3_column_int64(ST, column);
    else if (type == SQLITE_FLOAT)
        return sqlite3_column_double(ST, column);
    else if (type == SQLITE_TEXT)
    {
        auto raw = sqlite3_column_text(ST, column);
        auto size = sqlite3_column_bytes(ST, column);
        return String((const char *)raw, size);
    }
    else if (type == SQLITE_BLOB)
    {
        auto raw = sqlite3_column_blob(ST, column);
        auto size = sqlite3_column_bytes(ST, column);
        return String((const char *)raw, size);
    }
    else
        throw std::runtime_error("Returned SQLite value type is not supported!");
}

Object Statement::values()
{
    Object values;
    size_t count = columns();
    for (size_t i = 0; i < count; ++i) values[column(i)] = value(i);
    return values;
}

bool Statement::bind(size_t index, const Value &value)
{
    if (value.isnull())
        return sqlite3_bind_null(ST, index) == SQLITE_OK;
    else if (value.isint())
        return sqlite3_bind_int64(ST, index, value.integer()) == SQLITE_OK;
    else if (value.isreal())
        return sqlite3_bind_double(ST, index, value.real()) == SQLITE_OK;
    else if (value.isstr())
    {
        const String &str = value.string();
        return sqlite3_bind_text(ST, index, str.data(), str.size(), SQLITE_TRANSIENT) == SQLITE_OK;
    }
    else
        throw std::runtime_error("Given value type is not supported when binding!");
}

bool Statement::bind(const String &name, const Value &value)
{
    return bind(sqlite3_bind_parameter_index(ST, name.c_str()), value);
}

bool Statement::bind(size_t index, const Blob &value)
{
    NEYSON_ASSERT(value.value.isstr(), "Value type must be String when binding blob!");
    const String &str = value.value.string();
    return sqlite3_bind_blob(ST, index, str.data(), str.size(), SQLITE_TRANSIENT) == SQLITE_OK;
}

bool Statement::bind(const String &name, const Blob &value)
{
    return bind(sqlite3_bind_parameter_index(ST, name.c_str()), value);
}

////////////////////////////////////////////////////////////////////////////////

Transaction::~Transaction()
{
    if (!_commited) _database.execute("ROLLBACK;");
}

bool Transaction::commit()
{
    auto result = _database.execute("COMMIT;");
    _commited = result;
    return result;
}

bool Backup::step(int pages)
{
    if (_finished) return false;
    auto code = sqlite3_backup_step(BU, pages);
    if (code == SQLITE_DONE) _finished = true, close();
    return code == SQLITE_OK;
}

////////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream &os, Error error)
{
    if (error == Error::Unallocated) return os << "Error::Unallocated";
    if (error == Error::OK) return os << "Error::OK";
    if (error == Error::Error) return os << "Error::Error";
    if (error == Error::Internal) return os << "Error::Internal";
    if (error == Error::Perm) return os << "Error::Perm";
    if (error == Error::Abort) return os << "Error::Abort";
    if (error == Error::Busy) return os << "Error::Busy";
    if (error == Error::Locked) return os << "Error::Locked";
    if (error == Error::NoMem) return os << "Error::NoMem";
    if (error == Error::ReadOnly) return os << "Error::ReadOnly";
    if (error == Error::Interrupt) return os << "Error::Interrupt";
    if (error == Error::IOErr) return os << "Error::IOErr";
    if (error == Error::Corrupt) return os << "Error::Corrupt";
    if (error == Error::NotFound) return os << "Error::NotFound";
    if (error == Error::Full) return os << "Error::Full";
    if (error == Error::CantOpen) return os << "Error::CantOpen";
    if (error == Error::Protocol) return os << "Error::Protocol";
    if (error == Error::Empty) return os << "Error::Empty";
    if (error == Error::Schema) return os << "Error::Schema";
    if (error == Error::TooBig) return os << "Error::TooBig";
    if (error == Error::Constraint) return os << "Error::Constraint";
    if (error == Error::Mismatch) return os << "Error::Mismatch";
    if (error == Error::Misuse) return os << "Error::Misuse";
    if (error == Error::NoLFS) return os << "Error::NoLFS";
    if (error == Error::Auth) return os << "Error::Auth";
    if (error == Error::Format) return os << "Error::Format";
    if (error == Error::Range) return os << "Error::Range";
    if (error == Error::NotADB) return os << "Error::NotADB";
    if (error == Error::Notice) return os << "Error::Notice";
    if (error == Error::Warning) return os << "Error::Warning";
    if (error == Error::Row) return os << "Error::Row";
    if (error == Error::Done) return os << "Error::Done";
    return os << "Error::Unknown";
}

std::ostream &operator<<(std::ostream &os, Extended ext)
{
    if (ext == Extended::ErrorMissingCollSeq) return os << "Extended::ErrorMissingCollSeq";
    if (ext == Extended::ErrorRetry) return os << "Extended::ErrorRetry";
    if (ext == Extended::ErrorSnapshot) return os << "Extended::ErrorSnapshot";
    if (ext == Extended::IOErrRead) return os << "Extended::IOErrRead";
    if (ext == Extended::IOErrShortRead) return os << "Extended::IOErrShortRead";
    if (ext == Extended::IOErrWrite) return os << "Extended::IOErrWrite";
    if (ext == Extended::IOErrFSync) return os << "Extended::IOErrFSync";
    if (ext == Extended::IOErrDirFSync) return os << "Extended::IOErrDirFSync";
    if (ext == Extended::IOErrTruncate) return os << "Extended::IOErrTruncate";
    if (ext == Extended::IOErrFStat) return os << "Extended::IOErrFStat";
    if (ext == Extended::IOErrUnlock) return os << "Extended::IOErrUnlock";
    if (ext == Extended::IOErrRDLock) return os << "Extended::IOErrRDLock";
    if (ext == Extended::IOErrDelete) return os << "Extended::IOErrDelete";
    if (ext == Extended::IOErrBlocked) return os << "Extended::IOErrBlocked";
    if (ext == Extended::IOErrNoMem) return os << "Extended::IOErrNoMem";
    if (ext == Extended::IOErrAccess) return os << "Extended::IOErrAccess";
    if (ext == Extended::IOErrCheckReservedLock) return os << "Extended::IOErrCheckReservedLock";
    if (ext == Extended::IOErrLock) return os << "Extended::IOErrLock";
    if (ext == Extended::IOErrClose) return os << "Extended::IOErrClose";
    if (ext == Extended::IOErrDirClose) return os << "Extended::IOErrDirClose";
    if (ext == Extended::IOErrSHMOpen) return os << "Extended::IOErrSHMOpen";
    if (ext == Extended::IOErrSHMSize) return os << "Extended::IOErrSHMSize";
    if (ext == Extended::IOErrSHMLock) return os << "Extended::IOErrSHMLock";
    if (ext == Extended::IOErrSHMMap) return os << "Extended::IOErrSHMMap";
    if (ext == Extended::IOErrSeek) return os << "Extended::IOErrSeek";
    if (ext == Extended::IOErrDeleteNoEnt) return os << "Extended::IOErrDeleteNoEnt";
    if (ext == Extended::IOErrMMap) return os << "Extended::IOErrMMap";
    if (ext == Extended::IOErrGetTempPath) return os << "Extended::IOErrGetTempPath";
    if (ext == Extended::IOErrConvPath) return os << "Extended::IOErrConvPath";
    if (ext == Extended::IOErrVNode) return os << "Extended::IOErrVNode";
    if (ext == Extended::IOErrAuth) return os << "Extended::IOErrAuth";
    if (ext == Extended::IOErrBeginAtomic) return os << "Extended::IOErrBeginAtomic";
    if (ext == Extended::IOErrCommitAtomic) return os << "Extended::IOErrCommitAtomic";
    if (ext == Extended::IOErrRollbackAtomic) return os << "Extended::IOErrRollbackAtomic";
    if (ext == Extended::IOErrData) return os << "Extended::IOErrData";
    if (ext == Extended::IOErrCorruptFS) return os << "Extended::IOErrCorruptFS";
    if (ext == Extended::LockedSharedCache) return os << "Extended::LockedSharedCache";
    if (ext == Extended::LockedVTab) return os << "Extended::LockedVTab";
    if (ext == Extended::BusyRecovery) return os << "Extended::BusyRecovery";
    if (ext == Extended::BusySnapshot) return os << "Extended::BusySnapshot";
    if (ext == Extended::BusyTimeout) return os << "Extended::BusyTimeout";
    if (ext == Extended::CantOpenNoTempDir) return os << "Extended::CantOpenNoTempDir";
    if (ext == Extended::CantOpenIsDir) return os << "Extended::CantOpenIsDir";
    if (ext == Extended::CantOpenFullPath) return os << "Extended::CantOpenFullPath";
    if (ext == Extended::CantOpenConvPath) return os << "Extended::CantOpenConvPath";
    if (ext == Extended::CantOpenDirtywal) return os << "Extended::CantOpenDirtywal";
    if (ext == Extended::CantOpenSymlink) return os << "Extended::CantOpenSymlink";
    if (ext == Extended::CorruptVTab) return os << "Extended::CorruptVTab";
    if (ext == Extended::CorruptSequence) return os << "Extended::CorruptSequence";
    if (ext == Extended::CorruptIndex) return os << "Extended::CorruptIndex";
    if (ext == Extended::ReadOnlyRecovery) return os << "Extended::ReadOnlyRecovery";
    if (ext == Extended::ReadOnlyCantLock) return os << "Extended::ReadOnlyCantLock";
    if (ext == Extended::ReadOnlyRollback) return os << "Extended::ReadOnlyRollback";
    if (ext == Extended::ReadOnlyDBMoved) return os << "Extended::ReadOnlyDBMoved";
    if (ext == Extended::ReadOnlyCantInit) return os << "Extended::ReadOnlyCantInit";
    if (ext == Extended::ReadOnlyDirectory) return os << "Extended::ReadOnlyDirectory";
    if (ext == Extended::AbortRollback) return os << "Extended::AbortRollback";
    if (ext == Extended::ConstraintCheck) return os << "Extended::ConstraintCheck";
    if (ext == Extended::ConstraintCommitHoot) return os << "Extended::ConstraintCommitHoot";
    if (ext == Extended::ConstraintForeignKey) return os << "Extended::ConstraintForeignKey";
    if (ext == Extended::ConstraintFunction) return os << "Extended::ConstraintFunction";
    if (ext == Extended::ConstraintNotNull) return os << "Extended::ConstraintNotNull";
    if (ext == Extended::ConstraintPrimaryKey) return os << "Extended::ConstraintPrimaryKey";
    if (ext == Extended::ConstraintTrigger) return os << "Extended::ConstraintTrigger";
    if (ext == Extended::ConstraintUnique) return os << "Extended::ConstraintUnique";
    if (ext == Extended::ConstraintVTab) return os << "Extended::ConstraintVTab";
    if (ext == Extended::ConstraintRowID) return os << "Extended::ConstraintRowID";
    if (ext == Extended::ConstraintPinned) return os << "Extended::ConstraintPinned";
    if (ext == Extended::ConstraintDataType) return os << "Extended::ConstraintDataType";
    if (ext == Extended::NoticeRecoveryWal) return os << "Extended::NoticeRecoveryWal";
    if (ext == Extended::NoticeRecoverRollback) return os << "Extended::NoticeRecoverRollback";
    if (ext == Extended::NoticeRBU) return os << "Extended::NoticeRBU";
    if (ext == Extended::WarningAutoIndex) return os << "Extended::WarningAutoIndex";
    if (ext == Extended::AuthUser) return os << "Extended::AuthUser";
    if (ext == Extended::OKLoadPermanently) return os << "Extended::OKLoadPermanently";
    if (ext == Extended::OKSymlink) return os << "Extended::OKSymlink";
    return os << "Extended::Unknown";
}
}  // namespace SQLite
}  // namespace Neyson

#endif
