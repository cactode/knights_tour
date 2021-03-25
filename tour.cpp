#include <iostream>
#include <cstdint>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <limits>

// Alias, should not be changed
constexpr int BOARD_SIZE{8};

// Simple class to simplify managing valid positions.
class pos_t {
    public:
        int row;
        int col;

        bool operator==(pos_t other) {
            return row == other.row && col == other.col;
        }
    
        pos_t operator+(pos_t other) {
            return pos_t{row + other.row, col + other.col};
        }

        friend std::ostream & operator<<(std::ostream & out, pos_t pos);

        bool valid() {
            return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
        }
};

// Simple struct to store positions
struct pos_array_t {
    pos_t array[8];
    int size{0};
};

// Alias, valid moves that a knight can make
constexpr pos_t KNIGHT_MOVES[]{
    pos_t{-2, -1},
    pos_t{-2, 1},
    pos_t{-1, -2},
    pos_t{-1, 2},
    pos_t{1, -2},
    pos_t{1, 2},
    pos_t{2, -1},
    pos_t{2, 1}
    };

std::ostream & operator<<(std::ostream & out, pos_t pos) {
    out << "(" << pos.row << ", " << pos.col << ")";
    return out;
}

// Class to represent a mutable chessboard. Uses bitset to minimize memory footprint.
class Chessboard {
    private:
        pos_t current{};
        uint64_t traversed{0};
    public:
        Chessboard(pos_t initial) : current{initial} {
            (*this).set(initial);
        }

        bool get(pos_t pos) const {
            return (traversed >> (pos.row * BOARD_SIZE + pos.col)) & ((uint64_t)1);
        }

        void set(pos_t pos) {
            current = pos;
            traversed |= ((uint64_t)1) << (pos.row * BOARD_SIZE + pos.col);
        }

        pos_t get_current() {
            return current;
        }

        bool reachable(pos_t target) {
            for (pos_t knight_move : KNIGHT_MOVES) {
                if (current + knight_move == target) return true;
            }
            return false;
        }

        pos_array_t valid_moves() {
            return valid_moves(current);
        }

        pos_array_t valid_moves(pos_t from) {
            pos_array_t moves;
            for (int i = 0; i < 8; ++i) {
                pos_t after_move{from + KNIGHT_MOVES[i]};
                if (after_move.valid() && !get(after_move)) {
                    moves.array[moves.size] = after_move;
                    ++moves.size;
                }
            }
            return moves;
        }

        bool successful() {
            return traversed == ((uint64_t)0xFFFFFFFFFFFFFFFF);
        }

        void print() {
            for (int x{0}; x < BOARD_SIZE; ++x) {
                for (int y{0}; y < BOARD_SIZE; ++y) {
                    if (pos_t{x, y} == current) {
                        std::cout << "X ";
                    } else if ( get(pos_t{x, y}) ) {
                        std::cout << "■ ";
                    } else {
                        std::cout << "□ ";
                    }
                }
                std::cout << "\n";
            }
            std::cout << std::flush;
        }
};

// Performs knights tour. Returns a vector of positions that the knight will be in.
std::vector<pos_t> knights_tour(pos_t INITIAL_POS) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<pos_t> moves{};
    moves.reserve(64);

    for (int i{0}; i < 10000; ++i) {
        Chessboard chess{INITIAL_POS};
        for (int i{0}; i < 64; ++i) {
            pos_array_t candidate_moves{chess.valid_moves()};
            int candidate_move_count{candidate_moves.size};
            if (candidate_move_count) {
                int move_count[BOARD_SIZE];
                int min{std::numeric_limits<int>::max()};
                for (int i{0}; i < candidate_move_count; ++i) {
                    move_count[i] = (chess.valid_moves(candidate_moves.array[i])).size;
                    if (min > move_count[i]) min = move_count[i];
                }
                
                pos_array_t best_moves;

                for (int i{0}; i < candidate_move_count; ++i) {
                    if (move_count[i] == min) {
                        best_moves.array[best_moves.size] = candidate_moves.array[i];
                        ++best_moves.size;
                    }
                }

                std::uniform_int_distribution<> distr{0, static_cast<int>(best_moves.size) - 1};

                pos_t best_move{best_moves.array[distr(gen)]};
                chess.set(best_move);
                moves.push_back(best_move);

            } else if (chess.successful() && chess.reachable(INITIAL_POS)) {
                return moves;
            } else {
                moves.clear();
                break;
            }
        }
    }
    return moves;
}

int main()
{   
    pos_t INITIAL_POS{1,1};

    std::vector<pos_t> tour{knights_tour(INITIAL_POS)};
    tour.push_back(INITIAL_POS);
    Chessboard chess{INITIAL_POS};
    for (pos_t move : tour) {
        chess.set(move);
        chess.print();
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    return 0;
}