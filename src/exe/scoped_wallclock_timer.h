//
// Created by igudich on 11/6/18.
//

#ifndef MD_WORKSPACE_STRUCTURE_SCOPED_WALLCLOCK_TIMER_H
#define MD_WORKSPACE_STRUCTURE_SCOPED_WALLCLOCK_TIMER_H

#include <chrono>
#include <string>

class scoped_wallclock_timer {
public:
  using Clock = std::chrono::high_resolution_clock;

public:
  scoped_wallclock_timer(const std::string &name);
  ~scoped_wallclock_timer();

private:
  const std::string m_name;
  const std::chrono::time_point<Clock> m_start;
};


#endif //MD_WORKSPACE_STRUCTURE_SCOPED_WALLCLOCK_TIMER_H
