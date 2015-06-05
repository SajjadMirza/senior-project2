#ifndef _PUZZLE_HPP_
#define _PUZZLE_HPP_

#include <common.hpp>
#include <draw/Text.hpp>
#include <Entity.hpp>

#include <utility>
#include <memory>

typedef std::pair<int,int> ColRow;


class Puzzle {
protected:
    std::vector<Entity*> entities;
public:
    virtual ~Puzzle();

    virtual void notifyPosition(ColRow pos);

    virtual void notifySelect(Entity *selected_entity);
};

class LogicPuzzle : public Puzzle {
    Entity *correct_entity;
public:
    ~LogicPuzzle();
    void notifyPosition(ColRow pos);
    void notifySelect(Entity *selected_entity);
};

class PatternPuzzle : public Puzzle {
    int counter;
    std::vector<Entity*> correct_order;
public:
    void notifyPosition(ColRow pos);
    void notifySelect(Entity *selected_entity);
};

class PathingPuzzle : public Puzzle {
    
public:
    void notifyPosition(ColRow pos);
    void notifySelect(Entity *selected_entity);
};


class PuzzleFactory {
public:
    std::shared_ptr<Puzzle> createPuzzle(ColRow top_left,
                                         ColRow bottom_right,
                                         const char *config_file);
};

#endif
