#include "src/structA/avl.hpp"
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

namespace leaderboard {
bool before(const Entry& a, const Entry& b) {
    return a.score != b.score ? a.score > b.score : a.player_id < b.player_id;
}
bool same(const Entry& a, const Entry& b) {
    return a.score == b.score && a.player_id == b.player_id;
}
int Avl::height(const Node* n) { return n ? n->height : 0; }
std::size_t Avl::count(const Node* n) { return n ? n->count : 0; }
void Avl::fix(Node* n) {
    n->height = 1 + std::max(height(n->left), height(n->right));
    n->count = 1 + count(n->left) + count(n->right);
}
Avl::~Avl() { destroy(root_); }
Avl::Avl(Avl&& other) noexcept : root_(std::exchange(other.root_, nullptr)),
                                 rotations_(std::exchange(other.rotations_, 0)) {}
Avl& Avl::operator=(Avl&& other) noexcept {
    if (this != &other) {
        destroy(root_);
        root_ = std::exchange(other.root_, nullptr);
        rotations_ = std::exchange(other.rotations_, 0);
    }
    return *this;
}
void Avl::destroy(Node* n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}
Avl::Node* Avl::rotate_left(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    fix(x); fix(y); ++rotations_;
    return y;
}
Avl::Node* Avl::rotate_right(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    fix(y); fix(x); ++rotations_;
    return x;
}
Avl::Node* Avl::balance(Node* n) {
    fix(n);
    if (height(n->left) - height(n->right) > 1) {
        if (height(n->left->right) > height(n->left->left))
            n->left = rotate_left(n->left);
        return rotate_right(n);
    }
    if (height(n->right) - height(n->left) > 1) {
        if (height(n->right->left) > height(n->right->right))
            n->right = rotate_right(n->right);
        return rotate_left(n);
    }
    return n;
}
Avl::Node* Avl::insert_node(Node* n, Entry e) {
    if (!n) return new Node(e);
    if (same(e, n->value)) throw std::invalid_argument("duplicate pair key");
    if (before(e, n->value)) n->left = insert_node(n->left, e);
    else n->right = insert_node(n->right, e);
    return balance(n);
}
void Avl::insert(Entry e) {
    if (e.player_id < 0) throw std::invalid_argument("negative player ID");
    root_ = insert_node(root_, e);
}
Avl::Node* Avl::pop_first(Node* n, Entry& out) {
    if (!n->left) {
        out = n->value;
        Node* right = n->right;
        delete n;
        return right;
    }
    n->left = pop_first(n->left, out);
    return balance(n);
}
Avl::Node* Avl::erase_node(Node* n, Entry e) {
    if (!n) throw std::out_of_range("missing pair key");
    if (same(e, n->value)) {
        if (!n->left || !n->right) {
            Node* child = n->left ? n->left : n->right;
            delete n;
            return child;
        }
        Entry successor{};
        n->right = pop_first(n->right, successor);
        n->value = successor;
    } else if (before(e, n->value)) n->left = erase_node(n->left, e);
    else n->right = erase_node(n->right, e);
    return balance(n);
}
void Avl::erase(Entry e) { root_ = erase_node(root_, e); }
std::size_t Avl::size() const { return count(root_); }
std::size_t Avl::rank(std::int64_t score) const {
    std::size_t higher = 0;
    const Node* n = root_;
    while (n) {
        if (n->value.score > score) {
            higher += count(n->left) + 1;
            n = n->right;
        } else n = n->left;
    }
    return higher + 1;
}
Entry Avl::select(std::size_t pos) const {
    if (pos < 1 || pos > size()) throw std::out_of_range("position");
    const Node* n = root_;
    while (n) {
        auto left = count(n->left);
        if (pos == left + 1) return n->value;
        if (pos <= left) n = n->left;
        else { pos -= left + 1; n = n->right; }
    }
    throw std::logic_error("incorrect subtree size");
}
void Avl::collect_range(const Node* n, std::size_t base, std::size_t first,
                        std::size_t last, std::vector<Entry>& out) {
    if (!n || base + count(n) < first || base + 1 > last) return;
    std::size_t pos = base + count(n->left) + 1;
    if (first < pos) collect_range(n->left, base, first, last, out);
    if (first <= pos && pos <= last) out.push_back(n->value);
    if (pos < last) collect_range(n->right, pos, first, last, out);
}
std::vector<Entry> Avl::rank_range(std::size_t first, std::size_t last) const {
    if (first < 1 || last < first) throw std::invalid_argument("rank range");
    std::vector<Entry> out;
    collect_range(root_, 0, first, last, out);
    return out;
}
void Avl::collect(const Node* n, std::vector<Entry>& out) {
    if (!n) return;
    collect(n->left, out); out.push_back(n->value); collect(n->right, out);
}
std::vector<Entry> Avl::entries() const {
    std::vector<Entry> out;
    out.reserve(size()); collect(root_, out); return out;
}
Avl::Node* Avl::join_nodes(Node* left, Entry pivot, Node* right) {
    if (std::abs(height(left) - height(right)) <= 1) {
        Node* n = new Node(pivot);
        n->left = left; n->right = right; fix(n); return n;
    }
    if (height(left) > height(right)) {
        left->right = join_nodes(left->right, pivot, right);
        return balance(left);
    }
    right->left = join_nodes(left, pivot, right->left);
    return balance(right);
}
Avl Avl::join(Avl&& left, Entry pivot, Avl&& right) {
    if (pivot.player_id < 0) throw std::invalid_argument("negative player ID");
    auto last = [](Node* n) {
        while (n->right) n = n->right;
        return n->value;
    };
    auto first = [](Node* n) {
        while (n->left) n = n->left;
        return n->value;
    };
    // Check before taking ownership so a rejected join leaves both inputs intact.
    if ((left.root_ && !before(last(left.root_), pivot)) ||
        (right.root_ && !before(pivot, first(right.root_))))
        throw std::invalid_argument("join ordering precondition");
    Avl result;
    result.root_ = result.join_nodes(std::exchange(left.root_, nullptr), pivot,
                                     std::exchange(right.root_, nullptr));
    result.rotations_ += left.rotations_ + right.rotations_;
    return result;
}
std::tuple<Avl::Node*, std::optional<Entry>, Avl::Node*>
Avl::split_node(Node* n, Entry pivot) {
    if (!n) return {nullptr, std::nullopt, nullptr};
    if (same(n->value, pivot)) {
        Node* left = n->left; Node* right = n->right;
        Entry found = n->value; delete n;
        return {left, found, right};
    }
    if (before(pivot, n->value)) {
        auto [left, found, middle] = split_node(n->left, pivot);
        Node* right = join_nodes(middle, n->value, n->right);
        delete n;
        return {left, found, right};
    }
    auto [middle, found, right] = split_node(n->right, pivot);
    Node* left = join_nodes(n->left, n->value, middle);
    delete n;
    return {left, found, right};
}
std::tuple<Avl, std::optional<Entry>, Avl> Avl::split(Entry pivot) {
    auto [left, found, right] = split_node(std::exchange(root_, nullptr), pivot);
    Avl a, b; a.root_ = left; b.root_ = right;
    return {std::move(a), found, std::move(b)};
}
std::pair<Avl, Avl> Avl::split_score(std::int64_t threshold) {
    auto [high, found, low] = split({threshold, -1});
    if (found) throw std::logic_error("negative sentinel ID collided");
    return {std::move(high), std::move(low)};
}
std::pair<int, std::size_t> Avl::audit_node(const Node* n,
                                           const Entry* lo, const Entry* hi) {
    if (!n) return {0, 0};
    if ((lo && !before(*lo, n->value)) || (hi && !before(n->value, *hi)))
        throw std::logic_error("AVL key order");
    auto [lh, lc] = audit_node(n->left, lo, &n->value);
    auto [rh, rc] = audit_node(n->right, &n->value, hi);
    if (std::abs(lh - rh) > 1 || n->height != 1 + std::max(lh, rh) ||
        n->count != 1 + lc + rc) throw std::logic_error("AVL metadata/balance");
    return {n->height, n->count};
}
std::size_t Avl::audit() const { return audit_node(root_, nullptr, nullptr).second; }
}
