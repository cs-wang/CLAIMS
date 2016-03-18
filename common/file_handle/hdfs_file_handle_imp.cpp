/*
 * Copyright [2012-2015] DaSE@ECNU
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * /Claims/common/file_handle/hdfs_file_handle_imp.cpp
 *
 *  Created on: Oct 20, 2015
 *      Author: yukai
 *		   Email: yukai2014@gmail.com
 *
 * Description:
 *
 */

//// this macro decides whether write DLOG message into log file.
//// Open means no DLOG message.
//// it will also prohibit assert()
// #define NDEBUG

#include "./hdfs_file_handle_imp.h"
#include <glog/logging.h>
#include <string>

#include "../../Config.h"
#include "../memory_handle.h"
#include "../../utility/lock_guard.h"

using std::endl;
using claims::utility::LockGuard;

namespace claims {
namespace common {

HdfsFileHandleImp::HdfsFileHandleImp() : read_start_pos_(-1) {
  fs_ = hdfsConnect(Config::hdfs_master_ip.c_str(), Config::hdfs_master_port);
  if (NULL == fs_) {
    LOG(ERROR) << "failed to connect to HDFS(ip:" << Config::hdfs_master_ip
               << ", port:" << Config::hdfs_master_port << ")" << endl;
    assert(false);
  }
  LOG(INFO) << "connected to HDFS(ip:" << Config::hdfs_master_ip
            << ", port:" << Config::hdfs_master_port << ")" << endl;
}

HdfsFileHandleImp::~HdfsFileHandleImp() {
  int ret = rSuccess;
  EXEC_AND_ONLY_LOG_ERROR(ret, Close(), "failed to close ");
  ret = hdfsDisconnect(fs_);
  fs_ = NULL;
  if (ret != 0) LOG(ERROR) << "failed to disconnect to hdfs" << endl;
}

RetCode HdfsFileHandleImp::Open(std::string file_name, FileOpenFlag open_flag) {
  assert(NULL != fs_ && "failed to connect hdfs");
  int ret = rSuccess;
  open_flag_ = open_flag;
  file_name_ = file_name;
  if (kReadFile == open_flag && false == CanAccess(file_name_)) {
    ret = rAccessHdfsFileFail;
    ELOG(ret, "File name:" << file_name_
                           << " open mode:" << file_open_flag_info[open_flag]);
    return ret;
  }

  if (kCreateFile == open_flag) {
    file_ = hdfsOpenFile(fs_, file_name_.c_str(), O_WRONLY, 0, 0, 0);
  } else if (kAppendFile == open_flag) {
    file_ = hdfsOpenFile(fs_, file_name_.c_str(), O_WRONLY | O_APPEND, 0, 0, 0);
  } else if (kReadFile == open_flag) {
    file_ = hdfsOpenFile(fs_, file_name_.c_str(), O_RDONLY, 0, 0, 0);
  } else {
    LOG(ERROR) << "parameter flag:" << open_flag << " is invalid" << endl;
    return rParamInvalid;
  }
  if (NULL == file_) {
    PLOG(ERROR) << "failed to open hdfs file :" << file_name_;
    return rOpenHdfsFileFail;
  } else {
    LOG(INFO) << "opened hdfs file: " << file_name_ << " with "
              << (kCreateFile == open_flag
                      ? "kCreateFile"
                      : kAppendFile == open_flag ? "kAppendFile" : "kReadFile")
              << endl;
    return rSuccess;
  }
}

RetCode HdfsFileHandleImp::Write(const void* buffer, const size_t length) {
  assert(NULL != fs_ && "failed to connect hdfs");
  assert(NULL != file_ && "make sure file is opened");
  assert(open_flag_ != kReadFile &&
         "It's unavailable to write into a read-only file");
  size_t total_write_num = 0;
  while (total_write_num < length) {
    int32_t write_num = hdfsWrite(
        fs_, file_, static_cast<const char*>(buffer) + total_write_num,
        length - total_write_num);
    if (-1 == write_num) {
      PLOG(ERROR) << "failed to write buffer(" << buffer
                  << ") to file: " << file_name_ << endl;
      return rWriteDiskFileFail;
    }
    total_write_num += write_num;
  }
  //  if (length > 100) {
  //    DLOG(INFO) << "write " << length << " length data from " << buffer
  //               << " into hdfs file:" << file_name_ << endl;
  //  } else {
  //    DLOG(INFO) << "write " << length
  //               << " length data :" << static_cast<const char*>(buffer)
  //               << " from " << buffer << " into  hdfs file:" << file_name_
  //               << endl;
  //  }
  return rSuccess;
}

RetCode HdfsFileHandleImp::AtomicWrite(const void* buffer,
                                       const size_t length) {
  assert(NULL != fs_ && "failed to connect hdfs");
  assert(NULL != file_ && "make sure file is opened");
  assert(open_flag_ != kReadFile &&
         "It's unavailable to write into a read-only file");
  size_t total_write_num = 0;
  LockGuard<Lock> gurad(write_lock_);
  while (total_write_num < length) {
    int32_t write_num = hdfsWrite(
        fs_, file_, static_cast<const char*>(buffer) + total_write_num,
        length - total_write_num);
    if (-1 == write_num) {
      PLOG(ERROR) << "failed to write buffer(" << buffer
                  << ") to file: " << file_name_ << endl;
      return rWriteDiskFileFail;
    }
    total_write_num += write_num;
  }
  return rSuccess;
}

RetCode HdfsFileHandleImp::Close() {
  if (NULL == file_) {
    LOG(INFO) << "hdfs file have been closed " << endl;
    return rSuccess;
  }
  assert(NULL != fs_ && "failed to connect hdfs");

  //  static char* hdfs_file_type[] = {"UNINITIALIZED", "INPUT", "OUTPUT"};
  //  LOG(INFO) << "the type of file_ is" << hdfs_file_type[file_->type] <<
  //  endl;

  if (0 != hdfsCloseFile(fs_, file_)) {
    LOG(ERROR) << "failed to close hdfs file: " << file_name_ << endl;
    return rCloseHdfsFileFail;
  }
  file_ = NULL;
  LOG(INFO) << "hdfs file is been closed " << endl;
  return rSuccess;
}

RetCode HdfsFileHandleImp::ReadTotalFile(void*& buffer, size_t* length) {
  assert(NULL != fs_ && "failed to connect hdfs");
  assert(NULL != file_ && "make sure file is opened");

  int ret = rSuccess;
  hdfsFileInfo* hdfsfile = hdfsGetPathInfo(fs_, file_name_.c_str());
  int file_length = hdfsfile->mSize;
  LOG(INFO) << "The length of file " << file_name_ << " is " << file_length
            << endl;
  // set position 0
  if (rSuccess != (ret = SetPosition(0))) {
    return ret;
  }

  buffer = Malloc(file_length + 1);
  int read_num = hdfsRead(fs_, file_, buffer, file_length);
  LOG(INFO) << "read " << read_num << " data from hdfs file " << file_name_
            << endl;

  if (read_num != file_length) {
    LOG(ERROR) << "failed to read file [" << file_name_
               << "] from hdfs , expected read " << file_length
               << " , actually read " << read_num << endl;
    return rReadHdfsFileFail;
  }
  *length = read_num;
  return ret;
}

RetCode HdfsFileHandleImp::Read(void* buffer, size_t length) {
  assert(NULL != fs_ && "failed to connect hdfs");
  assert(NULL != file_ && "make sure file is opened");

  int total_read_num = 0;
  while (total_read_num < length) {
    int read_num =
        hdfsRead(fs_, file_, static_cast<char*>(buffer) + total_read_num,
                 length - total_read_num);
    if (-1 == read_num) {
      LOG(ERROR) << "failed to read " << length << "from file:" << file_name_
                 << endl;
      return rReadHdfsFileFail;
    }
    total_read_num += read_num;
  }
  LOG(INFO) << "read total " << total_read_num << " from hdfs file "
            << file_name_ << endl;
  return rSuccess;
}

RetCode HdfsFileHandleImp::SetPosition(size_t pos) {
  assert(NULL != fs_ && "failed to connect hdfs");
  assert(NULL != file_ && "make sure file is opened");

  assert(kReadFile == open_flag_ &&
         "Seeking is only work for files opened in read-only mode");
  int ret = hdfsSeek(fs_, file_, pos);
  if (0 != ret) {
    LOG(ERROR) << "failed to seek to " << pos << " in " << file_name_ << " file"
               << endl;
    return rLSeekHdfsFileFail;
  }

  LOG(INFO) << "Seek to " << pos << "in " << file_name_ << " file" << endl;
  return rSuccess;
}

RetCode HdfsFileHandleImp::DeleteFile() {
  int ret = rSuccess;
  EXEC_AND_ONLY_LOG_ERROR(ret, Close(), "file name: " << file_name_);
  if (0 == hdfsExists(fs_, file_name_.c_str())) {
    if (0 != hdfsDelete(fs_, file_name_.c_str(), 0)) {
      LOG(ERROR) << "Failed to delete file : [" + file_name_ + "]."
                 << std::endl;
      return rFailure;
    }
  } else {
    LOG(WARNING) << "The file " << file_name_ << "is not exits!\n" << std::endl;
  }
  return rSuccess;
}

}  // namespace common
} /* namespace claims */
