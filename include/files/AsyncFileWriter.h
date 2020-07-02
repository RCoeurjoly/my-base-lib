// c/c++
#include <iostream>

// clit
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
// to define ostream on types
#include <fmt/ostream.h>

namespace chan {
namespace files {
namespace async {

class BinaryRecord {
 private:
  const std::vector<char> _bin_data;

 public:
  //copy bin data to this record
  BinaryRecord(const char* bin_data, size_t bin_len) : _bin_data(bin_data, bin_data + bin_len){};
  BinaryRecord(const std::vector<char>& bin_data) : _bin_data(bin_data){};
  //or just move if possible
  BinaryRecord(std::vector<char>&& bin_data) : _bin_data(move(bin_data)){};

  //support for logging directly from spdlog
  template <typename OStream>
  friend OStream& operator<<(OStream& os, const BinaryRecord& bin_record) {
    std::ostream_iterator<char> out_iter(os);
    std::copy(bin_record._bin_data.begin(), bin_record._bin_data.end(), out_iter);
    return os;
  }
};

class BinaryFormatter final : public spdlog::formatter {
 public:
  // TODO se le puden pasar argumentos para guardar de diferentes formas mensajes
  explicit BinaryFormatter(){};

  // use default pattern is not given
  ///explicit BinaryFormatter();

  BinaryFormatter(const BinaryFormatter& other) = delete;
  BinaryFormatter& operator=(const BinaryFormatter& other) = delete;

  std::unique_ptr<formatter> clone() const override {
    return spdlog::details::make_unique<BinaryFormatter>();
  };
  void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override {
    dest.append(msg.payload.begin(), msg.payload.end());
  };
};

class AsyncFileWriter {
  std::shared_ptr<spdlog::logger> _bin_logger;

 public:
  // Logger with sinks init list
  AsyncFileWriter(const std::string& filename, spdlog::sinks_init_list sinks) {
    // default value se puede cambiar antes de creear los sinks
    if (!spdlog::thread_pool()) {
      spdlog::init_thread_pool(8192 * 2, 2);
    }
    _bin_logger = std::make_shared<spdlog::async_logger>("AsyncFileWriter: " + filename, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    _bin_logger->set_pattern("");
    auto bin_formatter = spdlog::details::make_unique<BinaryFormatter>();
    _bin_logger->set_formatter(std::move(bin_formatter));
    spdlog::register_logger(_bin_logger);
  }
  AsyncFileWriter(const std::string& filename, spdlog::sink_ptr single_sink) : AsyncFileWriter(filename, {single_sink}) {}
  AsyncFileWriter(const std::string& filename) : AsyncFileWriter(filename, std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename)) {}

  void
  flush() {
    _bin_logger->flush();
  }

  void write(const char* data, size_t len) {
    BinaryRecord b{data, len};
    _bin_logger->info("{}", b);
  }

  virtual ~AsyncFileWriter() {
    flush();
    spdlog::drop(_bin_logger->name());
  }
};  // namespace async
}  // namespace async
}  // namespace files
}  // namespace chan
