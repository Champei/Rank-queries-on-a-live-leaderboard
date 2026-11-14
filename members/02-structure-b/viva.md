# Structure B viva practice - Teerth

Try answering without reading the answers first. Use scores `[10,20,30,40,50]`, counts `[2,0,1,3,0]`, total 6 throughout.

| Question | Answer/check |
|---|---|
| What does your structure store? | Player counts per score, not player IDs. |
| Why compress coordinates? | Fenwick needs dense integer indices while scores may be sparse. Sorting and deduplicating maps scores to coordinates. |
| Does adding a player create a coordinate? | No. Coordinates come from the allowed universe prepared beforehand. |
| What is `bit[4]`? | The total over coordinates 1 through 4: 6, not just the count at score 40. |
| What does `x & -x` give? | The lowest set bit, which is the length of the block stored at Fenwick index x. |
| How does `prefix(3)` work? | Read bit[3]=1, then bit[2]=2, then stop at 0. Total 3. |
| How does inserting score 30 update bit? | Its zero-based index is 2. Update Fenwick indices 3 and 4. |
| How do you revise a score? | Remove one occurrence of the old score, then add one occurrence of the new score. The app finds the old score by player ID. |
| Why is rank(30)=4? | Three players have score 40, so rank is 1+3. |
| Why use upper_bound in rank? | It locates the prefix containing all scores <= the query; subtracting those counts leaves strictly higher scores. |
| How do ties rank? | All score-40 players rank 1; score 30 ranks 4. Competition rank differs from distinct-score rank. |
| What does select_score(3) search for? | Ascending target 6-3+1=4. It returns 40. |
| Why strict < in the descent? | If a block reaches the remaining target, the desired element is inside it, so do not skip it. |
| Why scores[at], not scores[at+1]? | at is the number of coordinates skipped. The answer is one-based coordinate at+1, or zero-based array index at. |
| What happens at empty coordinates? | Prefix counts can stay equal. Nondecreasing prefix counts still support the search. |
| What invariant proves selection? | P(at)<k, target=k-P(at), and the largest prefix below k remains inside the interval narrowed by each power. Explain the alignment condition from D2. |
| What are the main complexities? | Update, rank and select are O(log U). Construction sorts the supplied universe. The current audit is O(U log U). |
| Can this Fenwick split/join like AVL? | It has no comparable structural operation; reconstruct the required counts from player scores. |
| What does rebuild do? | Constructs an empty tree and calls update for every supplied player score. It is not a linear-time Fenwick build. |
| What is audit's return value? | The class returns total players if valid and throws on inconsistency; the practice function returns bool. |
| What do steps and mutable mean? | steps reports a counter; mutable lets const queries change that counter while leaving leaderboard data unchanged. Audit also increases it through prefix calls. |
| Why have a header? | It declares the module interface so other .cpp files can compile calls; fenwick.cpp supplies the implementations. |
| What does throw do? | Stops the current operation and propagates an error to a matching catch, used here for invalid input or broken invariants. |
| What were the actual tests? | Codex ran the final application, 10,000-operation existing differential test, practice example, and 120,000-update sorted-array tester. No benchmark timings were measured. |
| Which work was assisted? | Codex reconstructed/adapted the class implementation and drafted D2/test/docs. Teerth rewrote practice code and reviewed concepts; explain this honestly. |

## Small modifications to practise yourself

1. Add one player with score 20. Predict cnt, bit, total and rank(10), then check.
2. Remove one player with score 40. Predict every valid selected score.
3. Explain how to count players with scores <=30 using prefix and upper_bound.
4. Trace select_score(5) and show exactly why replacing `<` with `<=` breaks it.
5. Explain why adding allowed score 35 requires a new coordinate layout.

Do not mark viva preparation complete solely because these notes exist. Practise explaining the functions and making a small change yourself.
