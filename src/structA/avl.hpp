#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

namespace leaderboard {
using ll = long long;
using std::optional;
using std::pair;
using std::size_t;
using std::tuple;
using std::uint64_t;
using std::vector;

struct Entry {
    ll score;
    ll player_id;
};
// Higher score first; ties use smaller player ID first.
bool before(const Entry& a, const Entry& b);
bool same(const Entry& a, const Entry& b);

class Avl {
public:
    Avl() = default;
    ~Avl();
    Avl(const Avl&) = delete;
    Avl& operator=(const Avl&) = delete;
    Avl(Avl&& other) noexcept;
    Avl& operator=(Avl&& other) noexcept;

    void insert(Entry entry);
    void erase(Entry entry);
    size_t size() const;
    size_t rank(ll score) const; // competition rank, one based
    Entry select(size_t position) const;  // one based, player ID tie break
    vector<Entry> rank_range(size_t first, size_t last) const;
    vector<Entry> entries() const;
    size_t audit() const; // throws on BST, size, height or AVL violation
    uint64_t rotations() const { return rotations_; }

    // Every key in left must precede pivot; pivot must precede every key in right.
    static Avl join(Avl&& left, Entry pivot, Avl&& right);
    // Destructive split around an arbitrary pair key.
    tuple<Avl, optional<Entry>, Avl> split(Entry pivot);
    // Destructive split into scores > threshold and scores <= threshold.
    pair<Avl, Avl> split_score(ll threshold);

private:
    struct Node {
        Entry value;
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 1;
        size_t count = 1;
        explicit Node(Entry v) : value(v) {}
    };
    Node* root_ = nullptr;
    uint64_t rotations_ = 0;
    static int height(const Node* n);
    static size_t count(const Node* n);
    static void fix(Node* n);
    Node* rotate_left(Node* n);
    Node* rotate_right(Node* n);
    Node* balance(Node* n);
    Node* insert_node(Node* n, Entry entry);
    Node* erase_node(Node* n, Entry entry);
    Node* pop_first(Node* n, Entry& out);
    Node* join_nodes(Node* left, Entry pivot, Node* right);
    tuple<Node*, optional<Entry>, Node*> split_node(Node* n, Entry pivot);
    static void destroy(Node* n);
    static void collect(const Node* n, vector<Entry>& out);
    static void collect_range(const Node* n, size_t base,
                              size_t first, size_t last,
                              vector<Entry>& out);
    static pair<int, size_t> audit_node(const Node* n,
                                                    const Entry* lo,
                                                    const Entry* hi);
};
}
