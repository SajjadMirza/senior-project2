#ifndef _PUZZLE_HPP_
#define _PUZZLE_HPP_

#include <common.hpp>
#include <draw/Text.hpp>
#include <Entity.hpp>

#include <utility>
#include <memory>

typedef std::pair<int,int> ColRow;

inline ColRow col_row(int col, int row) {
    return std::make_pair(col, row);
}

enum PuzzleType {
    LOGIC,
    PATTERN,
    PATHING,
};

struct PuzzleConfig {
    PuzzleType type;
    std::string name;
};


class Puzzle {
    bool success;
    friend class PuzzleFactory;
protected:
    std::vector<Entity*> entities;
public:
    virtual ~Puzzle() {}

    virtual void notifyPosition(ColRow pos) {}
    virtual void notifySelect(Entity *selected_entity) {}
    virtual void draw(Program *prog, MatrixStack *P, MatrixStack *MV,
                      Camera *cam, draw::Text *text, GLFWwindow *window) {}
    virtual void notifyKey(char c) {}
    void setSuccess(bool success);
    std::vector<Entity*> &getEntities() { return entities; }
    
    bool getSuccess();
};

class LogicPuzzle : public Puzzle {
    friend class PuzzleFactory;
    Entity *correct_entity;
    Entity *last_selected;
    bool display_text;
    std::string info;

public:
    LogicPuzzle();
    ~LogicPuzzle();
    void notifyPosition(ColRow pos);
    void notifySelect(Entity *selected_entity);
    void notifyKey(char c);
    void draw(Program *prog, MatrixStack *P, MatrixStack *MV,
              Camera *cam, draw::Text *text, GLFWwindow *window);
};

class PatternPuzzle : public Puzzle {
    friend class PuzzleFactory;
    int counter;
    std::vector<Entity*> correct_order;
public:
    void notifyPosition(ColRow pos);
    void notifySelect(Entity *selected_entity);
    void draw(Program *prog, MatrixStack *P, MatrixStack *MV,
              Camera *cam);
};

class PathingPuzzle : public Puzzle {
    friend class PuzzleFactory;
public:
    void notifyPosition(ColRow pos);
    void notifySelect(Entity *selected_entity);
    void draw(Program *prog, MatrixStack *P, MatrixStack *MV,
              Camera *cam);
};


class PuzzleFactory {
public:
    std::shared_ptr<Puzzle> createPuzzle(ColRow top_left,
                                         ColRow bottom_right,
                                         const char *config_file);
};

#endif
