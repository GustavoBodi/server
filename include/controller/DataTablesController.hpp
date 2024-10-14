#pragma once
#include <tuple>
#include "http/HttpResponse.hpp"

class DataTablesController {
public:
  using dependencies = std::tuple<>;
  DataTablesController() {}

  HttpResponse Get(const std::string &request, std::string companyId) const {
    (void) request;
    return HttpResponse(200, companyId, companyId);
  }

  virtual ~DataTablesController() {}
};


class DREController: public DataTablesController {
public:
  using dependencies = std::tuple<>;
  DREController(): DataTablesController() {}

  ~DREController() = default;
};
