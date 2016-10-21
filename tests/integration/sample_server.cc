#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "lib/exposer.h"
#include "lib/registry.h"

int main(int argc, char** argv) {
  using namespace prometheus;

  // create an http server running on port 8080
  auto exposer = Exposer{"127.0.0.1:8080"};

  // create a metrics registry with component=main labels applied to all its
  // metrics
  auto registry = std::make_shared<Registry>(
      std::map<std::string, std::string>{{"component", "main"}});

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimenstions)
  auto counter_family = registry->AddCounter(
      "time_running_seconds", "How many seconds is this server running?",
      {{"label", "value"}});

  // add a counter to the metric family
  auto second_counter = counter_family->Add(
      {{"another_label", "value"}, {"yet_another_label", "value"}});

  // ask the exposer to scrape the registry on incoming scrapes
  exposer.RegisterCollectable(registry);

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // increment the counter by one (second)
    second_counter->Increment();
  }
  return 0;
}
