#pragma once
#include <tuple>
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

class DataTablesController {
public:
  using dependencies = std::tuple<>;
  DataTablesController() {}

  HttpResponse Get(HttpRequest &request, std::string companyId) const {
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
