// Copyright by Contributors

#include "../include/dmlc/base.h"
#include "../include/dmlc/io.h"
#include "../include/dmlc/logging.h"
#include <cstring>
#include "io/uri_spec.h"
#include "io/line_split.h"
#include "io/recordio_split.h"
#include "io/single_file_split.h"
#include "io/filesys.h"
#include "io/local_filesys.h"
#include "io/cached_input_split.h"

namespace dmlc {
namespace io {
FileSystem *FileSystem::GetInstance(const URI &path) {
  if (path.protocol == "file://" || path.protocol.length() == 0) {
    return LocalFileSystem::GetInstance();
  }

  LOG(FATAL) << "unknown filesystem protocol " + path.protocol;
  return NULL;
}
}  // namespace io

InputSplit* InputSplit::Create(const char *uri_,
                               unsigned part,
                               unsigned nsplit,
                               const char *type) {
  using namespace std;
  using namespace dmlc::io;
  // allow cachefile in format path#cachefile
  io::URISpec spec(uri_, part, nsplit);
  if (!strcmp(spec.uri.c_str(), "stdin")) {
    return new SingleFileSplit(spec.uri.c_str());
  }
  CHECK(part < nsplit) << "invalid input parameter for InputSplit::Create";
  URI path(spec.uri.c_str());
  InputSplitBase *split = NULL;
  if (!strcmp(type, "text")) {
    split =  new LineSplitter(FileSystem::GetInstance(path),
                              spec.uri.c_str(), part, nsplit);
  } else if (!strcmp(type, "recordio")) {
    split =  new RecordIOSplitter(FileSystem::GetInstance(path),
                                  spec.uri.c_str(), part, nsplit);
  } else {
    LOG(FATAL) << "unknown input split type " << type;
  }
#if DMLC_ENABLE_STD_THREAD
  if (spec.cache_file.length() == 0) {
    return split;
  } else {
    return new CachedInputSplit(split, spec.cache_file.c_str());
  }
#else
  CHECK(spec.cache_file.length() == 0)
      << "to enable cached file, compile with c++11";
  return split;
#endif
}

Stream *Stream::Create(const char *uri,
                       const char * const flag,
                       bool try_create) {
  io::URI path(uri);
  return io::FileSystem::
      GetInstance(path)->Open(path, flag, try_create);
}

SeekStream *SeekStream::CreateForRead(const char *uri, bool try_create) {
  io::URI path(uri);
  return io::FileSystem::
      GetInstance(path)->OpenForRead(path, try_create);
}
}  // namespace dmlc
