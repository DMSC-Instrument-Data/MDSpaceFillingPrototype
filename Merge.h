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

template <typename EventT>
void merge_event_curves(typename EventT::ZCurve &curve,
                        typename EventT::ZCurve &curveNew) {
  curve.reserve(curve.size() + curveNew.size());

  /* Obtain iterator to last event of original curve */
  auto middle = curve.begin();
  std::advance(middle, curve.size() - 1);

  /* Add new events */
  curve.insert(curve.cend(), curveNew.begin(), curveNew.end());

  /* Advance iterator so it points to the first event of the second event curve
   */
  std::advance(middle, 1);

  std::inplace_merge(curve.begin(), middle, curve.end());
}
