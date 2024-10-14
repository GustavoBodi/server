#pragma once
#include <iostream>
#include <memory>
#include <concepts>

class ILogger {
public:
  virtual void testPrint() = 0;
  virtual ~ILogger() {}
};

class Logger: public ILogger {
public:
  Logger() {}
  void testPrint() {
    std::cout << "print something" << std::endl;
  }
  ~Logger() = default;
};
