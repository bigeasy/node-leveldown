/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LD_DATABASE_ASYNC_H
#define LD_DATABASE_ASYNC_H

#include <vector>
#include <node.h>

#include "leveldb/cache.h"

#include "async.h"

namespace leveldown {

class OpenWorker : public AsyncWorker {
public:
  OpenWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
    , bool createIfMissing
    , bool errorIfExists
    , bool compression
    , uint32_t cacheSize
    , uint32_t writeBufferSize
    , uint32_t blockSize
    , uint32_t maxOpenFiles
    , uint32_t blockRestartInterval
  );

  virtual ~OpenWorker ();
  virtual void Execute ();

private:
  leveldb::Options* options;
};

class CloseWorker : public AsyncWorker {
public:
  CloseWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
  );

  virtual ~CloseWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();
};

class IOWorker    : public AsyncWorker {
public:
  IOWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
    , leveldb::Slice key
    , v8::Persistent<v8::Value> keyPtr
  );

  virtual ~IOWorker ();
  virtual void WorkComplete ();

protected:
  leveldb::Slice key;
  v8::Persistent<v8::Value> keyPtr;
};

class ReadWorker : public IOWorker {
public:
  ReadWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
    , leveldb::Slice key
    , bool asBuffer
    , bool fillCache
    , v8::Persistent<v8::Value> keyPtr
  );

  virtual ~ReadWorker ();
  virtual void Execute ();
  virtual void HandleOKCallback ();

private:
  bool asBuffer;
  leveldb::ReadOptions* options;
  std::string value;
};

class DeleteWorker : public IOWorker {
public:
  DeleteWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
    , leveldb::Slice key
    , bool sync
    , v8::Persistent<v8::Value> keyPtr
  );

  virtual ~DeleteWorker ();
  virtual void Execute ();

protected:
  leveldb::WriteOptions* options;
};

class WriteWorker : public DeleteWorker {
public:
  WriteWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
    , leveldb::Slice key
    , leveldb::Slice value
    , bool sync
    , v8::Persistent<v8::Value> keyPtr
    , v8::Persistent<v8::Value> valuePtr
  );

  virtual ~WriteWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();

private:
  leveldb::Slice value;
  v8::Persistent<v8::Value> valuePtr;
};

class BatchWorker : public AsyncWorker {
public:
  BatchWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
    , leveldb::WriteBatch* batch
    , std::vector<Reference>* references
    , bool sync
  );

  virtual ~BatchWorker ();
  virtual void Execute ();

private:
  leveldb::WriteOptions* options;
  leveldb::WriteBatch* batch;
  std::vector<Reference>* references;
};

class ApproximateSizeWorker : public AsyncWorker {
public:
  ApproximateSizeWorker (
      Database* database
    , v8::Persistent<v8::Function> callback
    , leveldb::Slice start
    , leveldb::Slice end
    , v8::Persistent<v8::Value> startPtr
    , v8::Persistent<v8::Value> endPtr
  );

  virtual ~ApproximateSizeWorker ();
  virtual void Execute ();
  virtual void HandleOKCallback ();
  virtual void WorkComplete ();

  private:
    leveldb::Range range;
    v8::Persistent<v8::Value> startPtr;
    v8::Persistent<v8::Value> endPtr;
    uint64_t size;
};

} // namespace leveldown

#endif
