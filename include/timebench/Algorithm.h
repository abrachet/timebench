/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <algorithm>

namespace tb {

template <class Range, class OutputIt, class UnaryOperation>
OutputIt transform(Range&& range, OutputIt d_first, UnaryOperation unary_op) {
  return std::transform(std::begin(range), std::end(range), d_first, unary_op);
}

}  // namespace tb
