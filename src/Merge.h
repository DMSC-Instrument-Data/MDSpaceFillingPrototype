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
void merge_event_curves_inplace(typename EventT::ZCurve &curve,
                                typename EventT::ZCurve &curveNew) {
  curve.reserve(curve.size() + curveNew.size());

  /* Obtain iterator to last element of the first/existing curve */
  auto middle = curve.begin();
  std::advance(middle, curve.size() - 1);

  /* Append new event curve to end of storage of first/existing curve */
  curve.insert(curve.cend(), curveNew.begin(), curveNew.end());

  /* Advance iterator to the first element of the newly added curve */
  std::advance(middle, 1);

  std::inplace_merge(curve.begin(), middle, curve.end());
}

template <typename EventT>
void merge_event_curves(typename EventT::ZCurve &curve,
                        const typename EventT::ZCurve &a,
                        const typename EventT::ZCurve &b) {
  curve.reserve(a.size() + b.size());
  std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(curve));
}
