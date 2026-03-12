#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

namespace kigumi {

template <class BidirIt, class T, class Compare = std::less<T>>
BidirIt bsearch_lt(BidirIt first, BidirIt last, const T& value, Compare comp = Compare()) {
  auto it = std::lower_bound(first, last, value, comp);
  return it == first ? last : std::prev(it);
}

template <class BidirIt, class T, class Compare = std::less<T>>
BidirIt bsearch_le(BidirIt first, BidirIt last, const T& value, Compare comp = Compare()) {
  auto it = std::upper_bound(first, last, value, comp);
  return it == first ? last : std::prev(it);
}

template <class BidirIt, class T, class Compare = std::less<T>>
BidirIt bsearch_gt(BidirIt first, BidirIt last, const T& value, Compare comp = Compare()) {
  return std::upper_bound(first, last, value, comp);
}

template <class BidirIt, class T, class Compare = std::less<T>>
BidirIt bsearch_ge(BidirIt first, BidirIt last, const T& value, Compare comp = Compare()) {
  return std::lower_bound(first, last, value, comp);
}

template <class BidirIt, class T, class Compare = std::less<T>>
BidirIt bsearch_eq(BidirIt begin, BidirIt end, const T& value, Compare comp = Compare()) {
  BidirIt l = begin;
  BidirIt r = end;

  while (l != r) {
    BidirIt m = l;
    std::advance(m, std::distance(l, r) / 2);

    if (comp(*m, value)) {
      l = std::next(m);
    } else if (comp(value, *m)) {
      r = m;
    } else {
      return m;
    }
  }

  return end;
}

template <class BidirIt, class T, class Compare = std::less<T>>
std::pair<BidirIt, BidirIt> bsearch_eq_range(BidirIt first, BidirIt last, const T& value,
                                             Compare comp = Compare()) {
  BidirIt l = first;
  BidirIt r = last;

  while (l != r) {
    BidirIt m = l;
    std::advance(m, std::distance(l, r) / 2);

    if (comp(*m, value)) {
      l = std::next(m);
    } else if (comp(value, *m)) {
      r = m;
    } else {
      BidirIt ll = l;
      BidirIt lr = m;

      while (ll != lr) {
        BidirIt lm = ll;
        std::advance(lm, std::distance(ll, lr) / 2);

        if (comp(*lm, value)) {
          ll = std::next(lm);
        } else {
          lr = lm;
        }
      }

      BidirIt rl = std::next(m);
      BidirIt rr = r;

      while (rl != rr) {
        BidirIt rm = rl;
        std::advance(rm, std::distance(rl, rr) / 2);

        if (comp(value, *rm)) {
          rr = rm;
        } else {
          rl = std::next(rm);
        }
      }

      return {ll, rl};
    }
  }

  return {l, l};
}

}  // namespace kigumi
