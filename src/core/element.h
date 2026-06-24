#pragma once

class Element {
public:
    virtual ~Element() = default;
    virtual void process() = 0;
};
