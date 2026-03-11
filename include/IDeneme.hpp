#pragma once

class IDeneme {
public:
    virtual ~IDeneme() = default;

    virtual void Yazdir() const = 0;
    virtual int Topla(int a, int b) const = 0;
};
