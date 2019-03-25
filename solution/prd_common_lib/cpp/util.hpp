#pragma once

/// apply a functor on a range of element and another functor on the range exept the first element
template<typename Iterator, typename Fn1, typename Fn2>
void for_each_with_separator(Iterator from, Iterator to, Fn1 always, Fn2 butFirst) {
	switch ((from == to) ? 1 : 2) {
	case 0:
		do {
			butFirst(*from);
			[[fallthrough]];
	case 2:
		always(*from); ++from;
		} while (from != to);
		[[fallthrough]];
	default: // reached directly when from == to
		break;
	}
}