#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <iomanip>
#include <sstream>
#include <string>

namespace chan {
static inline uint64_t timeval_to_us(struct timeval& tv) {
  static const uint64_t SECS_TO_USECS = 1000000;
  return tv.tv_sec * SECS_TO_USECS + tv.tv_usec;
}

struct ResourceUsagePeriod {
  // See doc with: man getrusage
  // or this link: http://man7.org/linux/man-pages/man2/getrusage.2.html
  uint64_t period_us;        //period between captures of data
  uint64_t cpu_percent_100;  // Percent of usage cpu multiply by 100
  uint64_t nvcsw;            // Voluntary switch context changes
  uint64_t nivcsw;           // Involuntary switch context changes
  uint64_t inblock;          // block inputs
  uint64_t outblock;         //block outputs

  float getCpuUsageFloat() {
    return ((float)cpu_percent_100) / 100.0;
  }

  std::string getCpuUsageString() {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << getCpuUsageFloat();
    return ss.str();
  }
  std::string str() {
    std::stringstream ss;
    ss << "period: " << period_us;
    ss << "us ,cpu: ";
    ss << getCpuUsageString();
    ss << "%, inputs: " << inblock;
    ss << ", outputs: " << outblock;
    ss << ", voluntary_context_swithes: " << nvcsw;
    ss << ", unvoluntary_context_swithes: " << nivcsw;
    return ss.str();
  }
};

class ResourcesUsage {
 private:
  struct rusage              last_usage;
  uint64_t                   last_time;
  struct ResourceUsagePeriod resourcePeriod;

 public:
  ResourcesUsage() {
    std::string error;
    reset();
    updateResources(error);
  }
  void reset() {
    memset(&resourcePeriod, 0, sizeof(resourcePeriod));
    memset(&last_usage, 0, sizeof(last_usage));
    last_time = 0;
  }

  bool updateResources(std::string& error) {
    struct rusage usage;

    timeval tv_current_time;

    if (getrusage(RUSAGE_SELF, &usage) != 0) {
      error = "Error calling getrusage(): " + std::string(strerror(errno));
      errno = 0;
      return false;
    }

    gettimeofday(&tv_current_time, nullptr);

    // update current
    uint64_t current_time = timeval_to_us(tv_current_time);

    // Only calculate if we have data for last time
    if (last_time > 0) {
      // We can calculate values we for period
      resourcePeriod.period_us = current_time - last_time;
      // clang-format off
      resourcePeriod.cpu_percent_100 = (
          (timeval_to_us(usage.ru_stime) + timeval_to_us(usage.ru_utime))
          - (timeval_to_us(last_usage.ru_stime) + timeval_to_us(last_usage.ru_utime))) 
        * 10000 
        / resourcePeriod.period_us;
      // clang-format on
      resourcePeriod.nvcsw    = usage.ru_nvcsw - last_usage.ru_nvcsw;
      resourcePeriod.nivcsw   = usage.ru_nivcsw - last_usage.ru_nivcsw;
      resourcePeriod.inblock  = usage.ru_inblock - last_usage.ru_inblock;
      resourcePeriod.outblock = usage.ru_oublock - last_usage.ru_oublock;
    }

    // update last
    last_usage = usage;
    last_time  = current_time;
    // if the first run init values and exit
    return true;
  }

  ResourceUsagePeriod getResourcesUsage() {
    return resourcePeriod;
  }
};

};  // namespace chan
