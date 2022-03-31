#include <iostream>
//#include <opencv2/core.hpp>
#include <vector>
#include <deque>
#include <cmath>
#include <string>

const uint32_t PIX_THIN = 3;

int height = 0;
int width = 0;
int channels = 0;

enum {
    NOT_USED = -1
};

struct RGB {
    int32_t r;
    int32_t g;
    int32_t b;
};

struct Point {
    int32_t x;
    int32_t y;
};

const double EPS = 1E-9;

inline int det(int a, int b, int c, int d) {
    return a * d - b * c;
}

inline bool between(int a, int b, double c) {
    return std::min(a, b) <= c + EPS && c <= std::max(a, b) + EPS;
}

inline bool intersect_1(int a, int b, int c, int d) {
    if (a > b) std::swap(a, b);
    if (c > d) std::swap(c, d);
    return std::max(a, c) <= std::min(b, d);
}

bool intersect(Point a, Point b, Point c, Point d) {
    int A1 = a.y - b.y, B1 = b.x - a.x, C1 = -A1 * a.x - B1 * a.y;
    int A2 = c.y - d.y, B2 = d.x - c.x, C2 = -A2 * c.x - B2 * c.y;
    int zn = det(A1, B1, A2, B2);
    if (zn != 0) {
        double x = -det(C1, B1, C2, B2) * 1. / zn;
        double y = -det(A1, C1, A2, C2) * 1. / zn;
        return between(a.x, b.x, x) && between(a.y, b.y, y)
               && between(c.x, d.x, x) && between(c.y, d.y, y);
    } else
        return det(A1, C1, A2, C2) == 0 && det(B1, C1, B2, C2) == 0
               && intersect_1(a.x, b.x, c.x, d.x)
               && intersect_1(a.y, b.y, c.y, d.y);
}

bool check_thin(int32_t start_x, int32_t cur_x, int32_t start_y, int32_t cur_y) {
    if (std::abs(start_x - cur_x) + std::abs(start_y - cur_y) > PIX_THIN) {
        return true;
    }

    return false;
}

double line_length(int x1, int x2, int y1, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

class Color {
public:
    Color(int32_t r = 0, int32_t g = 0, int32_t b = 0) {
        r_ = r;
        g_ = g;
        b_ = b;
    }

    Color &operator=(const Color &other) = default;

    bool ColorAround(const RGB &rgb) {
        return (std::abs(r_ - rgb.r) + std::abs(g_ - rgb.g) + std::abs(b_ - rgb.b)) <= 10;
    }

private:
    int32_t r_ = 0;
    int32_t g_ = 0;
    int32_t b_ = 0;
};

class GraphIntersect {
public:
    int32_t Count(std::vector<std::vector<Color>>& points) {
        std::cout << "start\n";
        Vertexes res = BFS(points);
        std::cout << "bfs done\n";
        std::vector<bool> is_on_1_line = onOneLine(res);
        std::cout << "on 1 line done\n";

        int32_t res_count = intersectCount(is_on_1_line, res);

        return res_count;
    }

private:
    struct Vertexes {
        std::vector<Point> vertexes;
    };

    static bool
    isReachable(int32_t i, int32_t j, std::vector<std::vector<Color>>& points, std::vector<std::vector<int>>& used) {
        if (i < 0 || j < 0) {
            return false;
        }

        if (i > height || j > width) {
            return false;
        }

        if (points[i][j].ColorAround({0, 0, 0}) && used[i][j] == NOT_USED) {
            return true;
        }

        return false;
    }

    Vertexes BFS(std::vector<std::vector<Color>>& points) {
        Vertexes res;
        std::vector<std::vector<int>> visited(height, std::vector<int>(width, NOT_USED));
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {

                if (!isReachable(x, y, points, visited)) {
                    continue;
                }
                res.vertexes.push_back({x, y});
                int start_x = -1;
                int start_y = -1;

                std::deque<Point> deq;
                deq.push_back({x, y});
                visited[x][y] = 1;

                while (!deq.empty()) {
                    int size = int(deq.size());
                    Point start_point = deq.front();
                    start_x = start_point.x;
                    start_y = start_point.y;


                    for (int k = 0; k < size; k++) {
                        Point tmp = deq.front();
                        auto i = tmp.x;
                        auto j = tmp.y;
                        deq.pop_front();

                        // если расстояние между пикселями больше толщины линии то пропускаем этот шаг
                        if (!check_thin(start_x, i, start_y, j)) {
                            // идем в 4 стороны из каждой точки чтобы уменьшить толщину линии
                            if (GraphIntersect::isReachable(i + 1, j, points, visited)) {
                                deq.push_back({i + 1, j});
                                visited[i + 1][j] = 1;
                            }
                            if (GraphIntersect::isReachable(i, j + 1, points, visited)) {
                                deq.push_back({i, j + 1});
                                visited[i][j + 1] = 1;
                            }
                            if (GraphIntersect::isReachable(i - 1, j, points, visited)) {
                                deq.push_back({i - 1, j});
                                visited[i - 1][j] = 1;
                            }
                            if (GraphIntersect::isReachable(i, j - 1, points, visited)) {
                                deq.push_back({i, j - 1});
                                visited[i][j - 1] = 1;
                            }
                        }
                    }
                }

                res.vertexes.push_back({start_x, start_y});

            }
        }

        return res;
    }

    std::vector<bool> onOneLine(Vertexes& vertexes) {
        std::vector<bool> is_on_1_line(vertexes.vertexes.size(), true);

        for (int i = 0; i < vertexes.vertexes.size(); i++) {
            for (int j = 0; j < vertexes.vertexes.size(); j++) {
                for (int k = i + 1; k < vertexes.vertexes.size(); k++) {

                    if (i == j || j == k) {
                        continue;
                    }

                    Point p1 = vertexes.vertexes[i];
                    Point p2 = vertexes.vertexes[j];
                    Point p3 = vertexes.vertexes[k];

                    double current_l = line_length(p1.x, p3.x, p1.y, p3.y);
                    double l1 = line_length(p1.x, p2.x, p1.y, p2.y);
                    double l2 = line_length(p3.x, p2.x, p3.y, p2.y);

                    if (std::abs(l1 + l2 - current_l) < current_l * 0.002) {
                        is_on_1_line[j] = false;
                    }
                }
            }
        }

        return is_on_1_line;
    }

    int32_t intersectCount(std::vector<bool>& possible, Vertexes& vertexes) {
        std::vector<Point> result;
        for (int i = 0; i < vertexes.vertexes.size(); i++) {
            if (!possible[i]) {
                continue;
            }
            result.push_back(vertexes.vertexes[i]);
        }

        int res = 0;

        for (int v1 = 0; v1 < result.size(); v1++) {
            for (int v2 = v1 + 1; v2 < result.size(); v2++) {
                for (int v3 = v1 + 1; v3 < result.size(); v3++) {
                    for (int v4 = v3 + 1; v4 < result.size(); v4++) {
                        if (!(v1 == v3 || v1 == v4 || v2 == v3 || v2 == v4)) {
                            if (intersect(result[v1], result[v2], result[v3], result[v4])) {
                                res++;
                            }
                        }
                    }
                }
            }
        }

        return res / 2 + 1;
    }
};

int main() {
    std::string filename = "/Users/pavel/Desktop/1c/read_image.py";
    std::string command = "python3 ";
    command += filename;
    FILE *in = popen(command.c_str(), "r");
    int a, b, c;
//    int height, width, channels;
    fscanf(in, "%d %d %d", &height, &width, &channels);

    std::vector<std::vector<Color>> points(height, std::vector<Color>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(in, "%d %d %d", &a, &b, &c);
            points[i][j] = Color(a, b, c);
        }
    }
    pclose(in);

    GraphIntersect graph_inter;

    std::cout << "Intersections in the graph:" << " " << graph_inter.Count(points) << "\n";

    return 0;
}