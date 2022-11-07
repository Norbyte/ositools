#pragma once

#include <GameDefinitions/Base/Base.h>

BEGIN_SE()

struct VisualSetIndices
{
    static constexpr unsigned NumVisuals = 9;
    static constexpr unsigned NumColors = 5;

    static constexpr uint64_t VisualMasks[NumVisuals] = { 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, 0x0F, 0x1F, 0x07, 0x07 };
    static constexpr uint32_t VisualOffsets[NumVisuals] = { 0x00, 0x05, 0x0A, 0x0E, 0x12, 0x16, 0x1A, 0x1F, 0x22 };

    static constexpr uint64_t ColorMasks[NumColors] = { 0x0F, 0x0F, 0x3F, 0x3F, 0x3F };
    static constexpr uint32_t ColorOffsets[NumColors] = { 0x25, 0x29, 0x2D, 0x33, 0x39 };

    uint64_t Value;

    inline uint32_t GetColorIndex(unsigned index) const
    {
        return (uint32_t)((Value >> ColorOffsets[index]) & ColorMasks[index]);
    }

    inline uint32_t GetVisualIndex(unsigned index)
    {
        return (uint32_t)((Value >> VisualOffsets[index]) & VisualMasks[index]);
    }

    inline void SetColorIndex(unsigned index, uint32_t value)
    {
        auto val = Value & ~(ColorMasks[index] << ColorOffsets[index]);
        Value = val | ((value & ColorMasks[index]) << ColorOffsets[index]);
    }

    inline void SetVisualIndex(unsigned index, uint32_t value)
    {
        auto val = Value & ~(VisualMasks[index] << VisualOffsets[index]);
        Value = val | ((value & VisualMasks[index]) << VisualOffsets[index]);
    }

    uint32_t LuaGetColorIndex(lua_State* L, unsigned index);
    uint32_t LuaGetVisualIndex(lua_State* L, unsigned index);
    void LuaSetColorIndex(lua_State* L, unsigned index, uint32_t value);
    void LuaSetVisualIndex(lua_State* L, unsigned index, uint32_t value);
};

END_SE()
