"""Member 5: numerical checks of the claims in docs/derivations/D5.md.

A small Python copy of avl.cpp's insert, balance, join_nodes and split_node, on plain
integer keys (smaller key = earlier in the tree order). It checks:
  1. Theorem 1 and its lemmas for every sorted insert k = 1..3000;
  2. the worked split example (perfect tree on 1..15, split at 5);
  3. Lemma 4 (join height is max or max+1, result is a valid AVL tree) on 20000 random joins;
  4. Theorem 2 (at most 4H join calls per side of a split) on 3000 random splits.
These are checks, not proofs; the proofs are in D5. Run: python scratch/scratch-e/proof_checks.py
"""
import random
import sys

sys.setrecursionlimit(20000)


class Node:
    __slots__ = ("k", "l", "r", "h")

    def __init__(self, k, l=None, r=None):
        self.k, self.l, self.r = k, l, r
        fix(self)


def h(n):
    return n.h if n else 0


def fix(n):
    n.h = 1 + max(h(n.l), h(n.r))


stats = {"single": 0, "double": 0, "join_calls": 0}


def rotate_left(x):
    y = x.r; x.r = y.l; y.l = x; fix(x); fix(y)
    return y


def rotate_right(y):
    x = y.l; y.l = x.r; x.r = y; fix(y); fix(x)
    return x


def balance(n):  # same branches as Avl::balance
    fix(n)
    if h(n.l) - h(n.r) > 1:
        if h(n.l.r) > h(n.l.l):
            n.l = rotate_left(n.l); stats["double"] += 1
        else:
            stats["single"] += 1
        return rotate_right(n)
    if h(n.r) - h(n.l) > 1:
        if h(n.r.l) > h(n.r.r):
            n.r = rotate_right(n.r); stats["double"] += 1
        else:
            stats["single"] += 1
        return rotate_left(n)
    return n


def insert(n, k):
    if not n:
        return Node(k)
    if k < n.k:
        n.l = insert(n.l, k)
    else:
        n.r = insert(n.r, k)
    return balance(n)


def join_nodes(l, k, r):  # same branches as Avl::join_nodes
    stats["join_calls"] += 1
    if abs(h(l) - h(r)) <= 1:
        return Node(k, l, r)
    if h(l) > h(r):
        l.r = join_nodes(l.r, k, r)
        return balance(l)
    r.l = join_nodes(l, k, r.l)
    return balance(r)


def split_node(n, p, calls):  # same branches as Avl::split_node; counts join calls per side
    if not n:
        return None, None, None
    if n.k == p:
        return n.l, n.k, n.r
    if p < n.k:
        a, found, middle = split_node(n.l, p, calls)
        before = stats["join_calls"]; right = join_nodes(middle, n.k, n.r)
        calls["right"] += stats["join_calls"] - before
        return a, found, right
    middle, found, b = split_node(n.r, p, calls)
    before = stats["join_calls"]; left = join_nodes(n.l, n.k, middle)
    calls["left"] += stats["join_calls"] - before
    return left, found, b


def perfect(n):
    return n is None or (h(n.l) == h(n.r) and perfect(n.l) and perfect(n.r))


def keys(n, out):
    if n:
        keys(n.l, out); out.append(n.k); keys(n.r, out)
    return out


def valid(n, lo=None, hi=None):
    """Return the height; raise if order, height or balance is wrong."""
    if not n:
        return 0
    assert (lo is None or lo < n.k) and (hi is None or n.k < hi), "key order"
    a, b = valid(n.l, lo, n.k), valid(n.r, n.k, hi)
    assert abs(a - b) <= 1 and n.h == 1 + max(a, b), "height or balance"
    return n.h


def check_theorem1(limit=3000):
    # Each new key is the new minimum (leftmost), like rotation_inserts.
    root = None
    for k in range(1, limit + 1):
        height_before = h(root)
        single, double = stats["single"], stats["double"]
        root = insert(root, -k)
        rotations = stats["single"] - single
        assert stats["double"] == double, f"insert {k}: double rotation"
        power_of_two = (k & (k - 1)) == 0
        assert rotations == (0 if power_of_two else 1), f"insert {k}: {rotations} rotations"
        assert (h(root) == height_before + 1) == power_of_two, f"insert {k}: height change"
        assert h(root) == k.bit_length(), f"insert {k}: height is not bitlen(k)"
        v = root
        while v:  # Invariants I and II along the left spine
            assert h(v.l) >= h(v.r), f"insert {k}: Invariant I"
            assert perfect(v.r), f"insert {k}: Invariant II"
            v = v.l
    print(f"Theorem 1, Lemmas 1-3, Invariants I-II: hold for every insert k = 1..{limit}")


def check_worked_example():
    def build(lo, hi):
        if lo > hi:
            return None
        m = (lo + hi) // 2
        return Node(m, build(lo, m - 1), build(m + 1, hi))
    calls = {"left": 0, "right": 0}
    before = stats["join_calls"]
    a, found, b = split_node(build(1, 15), 5, calls)
    total = stats["join_calls"] - before
    assert keys(a, []) == [1, 2, 3, 4] and h(a) == 3, "left side"
    assert found == 5, "pivot"
    assert keys(b, []) == list(range(6, 16)) and h(b) == 4, "right side"
    assert total == 4, f"{total} join calls"
    valid(a); valid(b)
    print("Worked example: left {1..4} height 3, pivot 5, right {6..15} height 4, 4 join calls")


def check_lemma4(trials=20000):
    rng = random.Random(253)
    gaps = set()
    for _ in range(trials):
        nl = rng.randint(0, 5) if rng.random() < 0.3 else rng.randint(0, 300)
        nr = rng.randint(0, 300)
        left = right = None
        for k in rng.sample(range(0, 100000), nl):
            left = insert(left, k)
        for k in rng.sample(range(200001, 300000), nr):
            right = insert(right, k)
        hl, hr = h(left), h(right)
        before = stats["join_calls"]
        joined = join_nodes(left, 150000, right)
        assert stats["join_calls"] - before <= abs(hl - hr) + 1, "D1 call bound"
        assert h(joined) in (max(hl, hr), max(hl, hr) + 1), (hl, hr, h(joined))
        valid(joined)
        gaps.add(hl - hr)
    print(f"Lemma 4: holds on {trials} random joins (height gaps {min(gaps)}..{max(gaps)}),"
          " and each used at most |hL-hR|+1 calls")


def check_theorem2(trials=3000):
    rng = random.Random(253)
    worst = 0.0
    for _ in range(trials):
        n = rng.randint(1, 4000)
        sample = rng.sample(range(10 * n), n)
        root = None
        for k in (sorted(sample) if rng.random() < 0.3 else sample):
            root = insert(root, k)
        height = h(root)
        pivot = rng.choice(sample) if rng.random() < 0.7 else rng.randrange(10 * n)
        calls = {"left": 0, "right": 0}
        a, found, b = split_node(root, pivot, calls)
        for side in ("left", "right"):
            assert calls[side] <= 4 * height, (n, height, calls)
            worst = max(worst, calls[side] / height)
        valid(a); valid(b)
    print(f"Theorem 2: on {trials} random splits, join calls per side were at most"
          f" {worst:.2f} x H (bound 4 x H)")


if __name__ == "__main__":
    check_theorem1()
    check_worked_example()
    check_lemma4()
    check_theorem2()
