/*
 * Space filling curve prototype for MD event data structure
 * Copyright (C) 2018 European Spallation Source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

class scoped_wallclock_timer {
public:
  using Clock = std::chrono::high_resolution_clock;

public:
  scoped_wallclock_timer(benchmark::State &state,
                         const std::string &counterName)
      : m_state(state), m_counterName(counterName), m_start(Clock::now()) {}

  ~scoped_wallclock_timer() {
    const auto duration = Clock::now() - m_start;
    m_state.PauseTiming();
    const auto durationSeconds =
        std::chrono::duration<double>(duration).count();
    m_state.counters[m_counterName] += durationSeconds;
    m_state.ResumeTiming();
  }

private:
  benchmark::State &m_state;
  const std::string m_counterName;
  const std::chrono::time_point<Clock> m_start;
};
