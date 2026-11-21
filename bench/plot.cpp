#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Point { double n, mean, sd; };
struct Series { std::string name, color; std::vector<Point> points; bool model = false; };
static std::vector<std::string> fields(const std::string& line) {
    std::vector<std::string> out;
    std::stringstream in(line);
    std::string part;
    while (std::getline(in, part, ',')) out.push_back(part);
    return out;
}
static void figure(const std::string& path, const std::string& title,
                   const std::vector<Series>& series) {
    double minx = 1e100, maxx = 0, maxy = 0;
    for (const auto& s : series) for (auto p : s.points) {
        minx = std::min(minx, std::log10(p.n));
        maxx = std::max(maxx, std::log10(p.n));
        maxy = std::max(maxy, p.mean + p.sd);
    }
    if (maxx == minx) maxx = minx + 1;
    if (maxy == 0) maxy = 1;
    std::ofstream out(path);
    if (!out) throw std::runtime_error("cannot write SVG");
    out << "<svg xmlns='http://www.w3.org/2000/svg' width='900' height='560' viewBox='0 0 900 560'>\n"
        << "<rect width='900' height='560' fill='white'/>\n"
        << "<text x='70' y='35' font-family='sans-serif' font-size='22'>" << title << "</text>\n"
        << "<path d='M70 65 V480 H850' fill='none' stroke='black'/>\n"
        << "<text x='360' y='535' font-family='sans-serif'>log scale: number of players</text>\n"
        << "<text x='8' y='55' font-family='sans-serif'>microseconds/op</text>\n";
    for (int t = 0; t <= 5; ++t) {
        double y = 480 - 400.0 * t / 5;
        out << "<path d='M70 " << y << " H850' stroke='#ddd'/>"
            << "<text x='15' y='" << y + 4 << "' font-family='sans-serif' font-size='12'>"
            << maxy * t / 5 << "</text>\n";
    }
    int legend = 0;
    for (const auto& s : series) {
        out << "<polyline fill='none' stroke='" << s.color
            << "' stroke-width='3'";
        if (s.model) out << " stroke-dasharray='7 5'";
        out << " points='";
        for (auto p : s.points) {
            if (s.model) continue;
            double x = 70 + 780 * (std::log10(p.n) - minx) / (maxx - minx);
            double y = 480 - 400 * p.mean / maxy;
            out << x << ',' << y << ' ';
        }
        out << "'/>\n";
        for (auto p : s.points) {
            double x = 70 + 780 * (std::log10(p.n) - minx) / (maxx - minx);
            double y = 480 - 400 * p.mean / maxy;
            double dy = 400 * p.sd / maxy;
            out << "<path d='M" << x << ' ' << y-dy << " V" << y+dy
                << "' stroke='" << s.color << "'/>\n"
                << "<circle cx='" << x << "' cy='" << y
                << "' r='4' fill='" << s.color << "'/>\n";
        }
        out << "<text x='600' y='" << 60 + 22 * legend++
            << "' fill='" << s.color << "' font-family='sans-serif'>"
            << s.name << "</text>\n";
    }
    out << "</svg>\n";
}
int main(int argc, char** argv) {
    try {
        if (argc != 3) throw std::invalid_argument("usage: plot_results CSV OUT_PREFIX");
        std::ifstream in(argv[1]);
        if (!in) throw std::runtime_error("cannot open CSV");
        std::vector<Series> all = {
            {"AVL rank", "#1259a5", {}}, {"Fenwick rank", "#c44914", {}},
            {"AVL select", "#1259a5", {}}, {"Fenwick select score", "#c44914", {}},
            {"AVL split", "#1259a5", {}}, {"Fenwick split rebuild", "#c44914", {}},
            {"AVL ordered join", "#1259a5", {}}, {"Fenwick join rebuild", "#c44914", {}}
        };
        const std::vector<std::string> names = {
            "avl_rank", "fenwick_rank", "avl_select", "fenwick_select_score",
            "avl_split_score", "fenwick_split_rebuild", "avl_ordered_join",
            "fenwick_join_rebuild"};
        std::string line; std::getline(in, line);
        while (std::getline(in, line)) {
            auto f = fields(line);
            if (f.size() < 6) continue;
            for (std::size_t i = 0; i < names.size(); ++i)
                if (f[0] == names[i]) all[i].points.push_back({std::stod(f[1]),
                                                                 std::stod(f[4]),
                                                                 std::stod(f[5])});
        }
        for (auto& s : all)
            std::sort(s.points.begin(), s.points.end(),
                      [](Point a, Point b){ return a.n < b.n; });
        auto shape = [](Series s, std::string label, std::string color, int kind) {
            if (s.points.empty()) throw std::invalid_argument("missing benchmark series");
            s.name = label; s.color = color; s.model = true;
            double n0 = s.points.front().n, y0 = s.points.front().mean;
            for (auto& p : s.points) {
                double log_ratio = std::log2(p.n) / std::log2(n0);
                p.mean = y0 * (kind == 0 ? 1 : kind == 1 ? log_ratio
                                  : p.n / n0 * log_ratio);
                p.sd = 0;
            }
            return s;
        };
        figure(std::string(argv[2]) + "_rank.svg", "Rank latency",
               {all[0], all[1], shape(all[0], "AVL log shape", "#6694c7", 1),
                shape(all[1], "Fenwick log shape", "#e39a74", 1)});
        figure(std::string(argv[2]) + "_select.svg", "Select latency",
               {all[2], all[3], shape(all[2], "AVL log shape", "#6694c7", 1),
                shape(all[3], "Fenwick log shape", "#e39a74", 1)});
        figure(std::string(argv[2]) + "_split.svg", "Score split cost",
               {all[4], all[5], shape(all[4], "AVL log shape", "#6694c7", 1),
                shape(all[5], "Fenwick n log n shape", "#e39a74", 2)});
        figure(std::string(argv[2]) + "_join.svg", "Ordered join versus rebuild",
               {all[6], all[7], shape(all[6], "AVL constant-height shape", "#6694c7", 0),
                shape(all[7], "Fenwick n log n shape", "#e39a74", 2)});
        std::cout << "wrote four SVG figures with standard deviation bars\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "plot error: " << e.what() << '\n'; return 1;
    }
}
