// Copyright 2015 Ruimin Wang
// Author: Ruimin Wang, wangruimin_sx@qiyi.com, ruimin.wang13@gmail.com

#ifndef FILE_READER_H__
#define FILE_READER_H__

#include "rankboost.h"

// a file reader for class rankboost ranker
//   the format of file:
//     sample_id sample_label "qid" qid feature_id feature_value...
class FileReader {
public:

  FileReader();

  // constructor given a filename
  FileReader(const char* filename);

  // getter for samples
  const std::vector<RankList>& samples() const;

  // getter for features
  const std::vector<size_t>& features() const;

  // read from a file, the format of file is shown above
  void read(const char* filename);

private:

  // whole samples that read from the file. specified description
  // of samples is shown in rankboost class
  std::vector<RankList> __samples;

  // all features shown in the file.
  std::vector<size_t> __features;
};

#endif
