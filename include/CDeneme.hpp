#pragma once

#include "IDeneme.hpp"

class CDeneme final : public IDeneme {
public:
    void Yazdir() const override;
    int Topla(int a, int b) const override;
};
