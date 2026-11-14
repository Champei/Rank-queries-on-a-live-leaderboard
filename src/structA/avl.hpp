#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

namespace leaderboard {
struct Entry {
    std::int64_t score;
    std::int64_t player_id;
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
    std::size_t size() const;
    std::size_t rank(std::int64_t score) const; // competition rank, one based
    Entry select(std::size_t position) const;  // one based, player ID tie break
    std::vector<Entry> rank_range(std::size_t first, std::size_t last) const;
    std::vector<Entry> entries() const;
    std::size_t audit() const; // throws on BST, size, height or AVL violation
    std::uint64_t rotations() const { return rotations_; }

    // Every key in left must precede pivot; pivot must precede every key in right.
    static Avl join(Avl&& left, Entry pivot, Avl&& right);
    // Destructive split around an arbitrary pair key.
    std::tuple<Avl, std::optional<Entry>, Avl> split(Entry pivot);
    // Destructive split into scores > threshold and scores <= threshold.
    std::pair<Avl, Avl> split_score(std::int64_t threshold);

private:
    struct Node {
        Entry value;
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 1;
        std::size_t count = 1;
        explicit Node(Entry v) : value(v) {}
    };
    Node* root_ = nullptr;
    std::uint64_t rotations_ = 0;
    static int height(const Node* n);
    static std::size_t count(const Node* n);
    static void fix(Node* n);
    Node* rotate_left(Node* n);
    Node* rotate_right(Node* n);
    Node* balance(Node* n);
    Node* insert_node(Node* n, Entry entry);
    Node* erase_node(Node* n, Entry entry);
    Node* pop_first(Node* n, Entry& out);
    Node* join_nodes(Node* left, Entry pivot, Node* right);
    std::tuple<Node*, std::optional<Entry>, Node*> split_node(Node* n, Entry pivot);
    static void destroy(Node* n);
    static void collect(const Node* n, std::vector<Entry>& out);
    static void collect_range(const Node* n, std::size_t base,
                              std::size_t first, std::size_t last,
                              std::vector<Entry>& out);
    static std::pair<int, std::size_t> audit_node(const Node* n,
                                                    const Entry* lo,
                                                    const Entry* hi);
};
}
