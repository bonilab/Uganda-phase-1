/*
 * File:   DrugDatabase.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 11:05 AM
 */

#ifndef DRUGDATABASE_H
#define DRUGDATABASE_H

#include <map>

#include "Core/PropertyMacro.h"
#include "DrugType.h"

typedef std::map<int, DrugType*> DrugTypePtrMap;

class DrugDatabase : public DrugTypePtrMap {
  DELETE_COPY_AND_MOVE(DrugDatabase)
  // VIRTUAL_PROPERTY_REF(DrugTypePtrMap, drug_db)

public:
  DrugDatabase();

  //    DrugDatabase(const DrugDatabase& orig);
  virtual ~DrugDatabase();

  void add(DrugType* dt);

private:
};

#endif /* DRUGDATABASE_H */
