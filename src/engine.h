#ifndef ENGINE_H
#define ENGINE_H

#include "player.h"

class Engine : public Player {
    Q_OBJECT
public:
    enum Protocol { UciProtocol };

    Engine(Game *parent);
    ~Engine();

    virtual QString fileName() const = 0;
    virtual QString engineName() const = 0;
    virtual Protocol protocol() const = 0;

    virtual bool isHuman() const { return false; }
};

#endif
