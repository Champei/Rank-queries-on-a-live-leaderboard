# Leaderboard interface and invariants

Shared contract for six-member integration. Language: C++17. Structure B is present; Structure A is pending in this checkout.

## Ordering and ranks

`Entry{score, player_id}` is ordered by **higher score first**, then **lower nonnegative player ID first**. IDs are unique within a league. Competition rank is `1 + number of players with strictly higher score`, so scores `[90, 80, 80, 70]` have ranks `[1, 2, 2, 4]`. A position, unlike a competition rank, identifies one element in the total pair order.

## Structure A

`Avl::insert/erase`, `rank(score)`, and `select(position)` are O(log n). `rank_range(first,last)` is O(log n + number returned), because writing the output takes time. `join(left,pivot,right)` requires all left pair keys before the pivot and all right pair keys after it; its target bound is O(|h_left-h_right|+1). `split(pivot)` returns the two strict sides and the optional exact pivot in O(log n). Every node stores height and subtree count. `audit()` verifies strict key ordering, AVL balance, heights and counts.

## Structure B

`Fenwick` receives the set of possible scores before replay, sorts and deduplicates it, and stores player counts at those coordinates. `update(score,+1/-1)` is O(log U). `rank(score)` is `1 + total - count(scores <= score)`. `select_score(position)` uses binary lifting in O(log U). Fenwick does not store IDs, so player selection and range listings come from Structure A. An unseen score needs a coordinate rebuild. Season split and merge baselines rebuild Fenwick trees from player scores.

## Application and hash

`Leaderboard::revise(id,score)` updates both structures and the from-scratch open addressing `PlayerHash`. `remove`, `rank`, `select`, `positions`, `split(threshold)`, `merge(other)`, and `audit` are the application API. `split(threshold)` consumes its input and returns `(scores > threshold, scores <= threshold)`. The underlying AVL split is O(log n); constructing two full application objects and Fenwick trees takes O(n log U). Regional leagues with overlapping score ranges cannot use the ordered `Avl::join` precondition, so `Leaderboard::merge` inserts the other league's entries in O(m(log(n+m) + log U)) and requires disjoint IDs. It consumes `other`.

## Failure behavior

Invalid player IDs, missing scores/IDs, invalid positions, duplicate keys, and incompatible score universes throw standard exceptions. Test runners exit nonzero on failure. The oracle is a sorted array and may use standard library containers because it is test code.
