/*
 * File:   PersonIndexAllHandler.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:21 AM
 */

#ifndef PERSONINDEXALLHANDLER_H
#define PERSONINDEXALLHANDLER_H

#include "Core/PropertyMacro.h"
#include "IndexHandler.hxx"

class PersonIndexAllHandler : public IndexHandler {
  DELETE_COPY_AND_MOVE(PersonIndexAllHandler)

public:
  PersonIndexAllHandler() {}

  virtual ~PersonIndexAllHandler() {}
};

#endif
