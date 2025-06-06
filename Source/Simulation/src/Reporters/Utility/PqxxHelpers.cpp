/*
 * PqxxHelpers.cpp
 *
 * This class implements useful helper functions for use with PQXX.
 */
#include "PqxxHelpers.h"

#include <pqxx/pqxx>
#include <thread>

#include "Core/Config/Config.h"
#include "Model.h"
#include "easylogging++.h"

namespace pqxx_db {
pqxx::connection* get_connection() {
  // Getting a connection is straightforward, so this function is largely
  // intended to warp retry functionality
  int retry_count = 0;

  while (retry_count <= RETRY_LIMIT) {
    try {
      return new pqxx::connection(Model::CONFIG->connection_string());
    } catch (const pqxx::broken_connection &e) {
      retry_count++;
      LOG(WARNING) << "Unable to connect to database, try " << retry_count;

      // Sleep for ten seconds before retrying
      LOG(WARNING) << "Waiting " << (WAIT_TIMESPAN * retry_count) / 1000
                   << " seconds to retry...";
      std::chrono::milliseconds timespan(WAIT_TIMESPAN * retry_count);
      std::this_thread::sleep_for(timespan);
    }
  }

  LOG(ERROR) << "Unable to commit to the database after " << retry_count
             << " tries!";
  LOG(ERROR) << "Unable to connect to database, giving up.";
  exit(-1);
}
}  // namespace pqxx_db
