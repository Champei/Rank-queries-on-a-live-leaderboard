#!/usr/bin/env bash
# Member 5: checks that adversary/check.cpp catches real faults in the AVL tree.
# Each line below plants one fault in a temporary copy of src/structA/avl.cpp, builds the
# checker against that copy and runs it. Every fault should make the checker fail.
# The repository itself is never changed. Run from the repository root:
#   bash scratch/scratch-e/plant_bugs.sh
set -u
root=$(pwd)
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
caught=0; missed=0

plant() {  # name, sed expression applied to avl.cpp
    local dir="$work/$caught$missed"; mkdir -p "$dir/src/structA"
    cp "$root/src/structA/avl.hpp" "$dir/src/structA/"
    sed "$2" "$root/src/structA/avl.cpp" > "$dir/src/structA/avl.cpp"
    if cmp -s "$root/src/structA/avl.cpp" "$dir/src/structA/avl.cpp"; then
        printf '%-44s NOT APPLIED (avl.cpp changed; update this script)\n' "$1"; missed=$((missed + 1)); return
    fi
    if ! g++ -std=c++17 -O2 -static -I"$dir" -I"$root" "$dir/src/structA/avl.cpp" \
         "$root/adversary/generators.cpp" "$root/adversary/check.cpp" -o "$dir/adv.exe" 2>/dev/null; then
        printf '%-44s does not compile\n' "$1"; missed=$((missed + 1)); return
    fi
    local out; out=$("$dir/adv.exe" 50 500 2>&1); local code=$?
    if [ $code -ne 0 ]; then
        local msg; msg=$(printf '%s\n' "$out" | grep -m1 'adversary error' | cut -c1-90)
        printf '%-44s caught: %s\n' "$1" "${msg:-program crashed (exit $code)}"; caught=$((caught + 1))
    else
        printf '%-44s MISSED\n' "$1"; missed=$((missed + 1))
    fi
}

plant "subtree count ignores right child"   's/n->count = 1 + count(n->left) + count(n->right);/n->count = 1 + count(n->left);/'
plant "rotate_left skips fix(x)"            '/Avl::Node\* Avl::rotate_left/,/return y;/ s/fix(x); fix(y);/fix(y);/'
plant "double-rotation test flipped"        's/if (height(n->left->right) > height(n->left->left))/if (height(n->left->right) < height(n->left->left))/'
plant "balance allows height gap 2"         's/height(n->left) - height(n->right) > 1/height(n->left) - height(n->right) > 2/'
plant "join_nodes allows height gap 2"      's/if (abs(height(left) - height(right)) <= 1) {/if (abs(height(left) - height(right)) <= 2) {/'
plant "split puts subtree on wrong side"    's/Node\* right = join_nodes(middle, n->value, n->right);/Node* right = join_nodes(n->right, n->value, middle);/'
plant "split_score off by one"              's/split({threshold, -1})/split({threshold + 1, -1})/'
plant "pop_first skips rebalance"           '/Avl::Node\* Avl::pop_first/,/^}/ s/return balance(n);/return n;/'
plant "erase drops right subtree"           's/Node\* child = n->left ? n->left : n->right;/Node* child = n->left;/'
plant "join no longer checks right side"    's/(right.root_ \&\& !before(pivot, first(right.root_)))/false/'
plant "join has no precondition check"      's/throw invalid_argument("join ordering precondition");/;/'
plant "join compares scores only"           's/!before(last(left.root_), pivot)/!(last(left.root_).score > pivot.score)/; s/!before(pivot, first(right.root_))/!(pivot.score > first(right.root_).score)/'
plant "ties ordered by higher ID first"     's/a.player_id < b.player_id;/a.player_id > b.player_id;/'
plant "rotate_right not counted"            '/Avl::Node\* Avl::rotate_right/,/return x;/ s/++rotations_;//'
plant "split loses the found pivot"         's/Entry found = n->value; delete n;/Entry found = n->value; delete n; return {left, nullopt, right};/'

echo "caught $caught of $((caught + missed)) planted faults"
[ $missed -eq 0 ]
