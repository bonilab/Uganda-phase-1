/*
 * File:   IndexHandler.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:29 AM
 */

#ifndef INDEXHANDLER_H
#define INDEXHANDLER_H

#include <stdio.h>

#include "Core/PropertyMacro.h"

class IndexHandler {
  DELETE_COPY_AND_MOVE(IndexHandler)

  PROPERTY_REF(std::size_t, index)

public:
  IndexHandler() : index_(-1) {}

  virtual ~IndexHandler() = default;
};

#endif
