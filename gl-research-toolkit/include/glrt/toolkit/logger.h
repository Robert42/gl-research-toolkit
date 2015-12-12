#ifndef GLRT_LOGGER_H
#define GLRT_LOGGER_H


namespace glrt {

class Logger final
{
public:
  Logger();
  ~Logger();

  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;
};

} // namespace glrt

#endif // GLRT_LOGGER_H
