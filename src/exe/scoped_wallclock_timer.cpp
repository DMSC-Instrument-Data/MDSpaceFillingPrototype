//
// Created by igudich on 11/6/18.
//

#include <iostream>
#include "scoped_wallclock_timer.h"

scoped_wallclock_timer::scoped_wallclock_timer(const std::string &name)
    : m_name(name), m_start(Clock::now()) {
  std::cout << "TIMER: " << m_name << " started...\n";
}

scoped_wallclock_timer::~scoped_wallclock_timer() {
  const auto duration = Clock::now() - m_start;
  std::cout << "TIMER: " << m_name << " complete in "
            << std::chrono::duration<float>(duration).count() << " seconds\n";
}