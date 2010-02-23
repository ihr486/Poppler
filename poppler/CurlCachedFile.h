//========================================================================
//
// CurlCachedFile.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2010 Hib Eris <hib@hiberis.nl>
//
//========================================================================

#ifndef CURLCACHELOADER_H
#define CURLCACHELOADER_H

#include "poppler-config.h"
#include "CachedFile.h"

#include <curl/curl.h>

//------------------------------------------------------------------------

class CurlCachedFileLoader : public CachedFileLoader {

public:

  CurlCachedFileLoader();
  ~CurlCachedFileLoader();
  size_t init(GooString *url, CachedFile* cachedFile);
  int load(GooVector<ByteRange> *ranges, CachedFileWriter *writer);

private:

  GooString *url;
  CachedFile *cachedFile;
  CURL *curl;

};

#endif

