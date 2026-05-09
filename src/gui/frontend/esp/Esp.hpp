#pragma once

#include "core/engine/cache/Cache.hpp"

class Esp {
public:
    ~Esp() = default;
    Esp(const Esp&) = delete;
    Esp(Esp&&) = delete;
    Esp& operator=(const Esp&) = delete;
    Esp& operator=(Esp&&) = delete;

    static bool Init();
    static void Render();
private:
    Esp() {};

    static Esp& GetInstance()
    {
        static Esp i{};
        return i;
    }

    bool InitImpl();
    void RenderImpl();

    ImGuiIO io;
    ImFont* font;
    ImDrawList* d;

    // Temporary storage for ease
    view_matrix_t matrix;

    // dont know where vel_buffer should live
    std::vector<int> vel_buffer;
    size_t vel_index = 0;
    float vel_accumulator = 0.0f;

    void RenderPlayer(Player player, bool mate = false);
    void RenderPlayerBones(Player player, bool mate = false);
    void RenderPlayerBars(Player player, std::pair<Vec2_t, Vec2_t> bounds);
    void RenderPlayerFalgs(Player player, std::pair<Vec2_t, Vec2_t> bounds, bool mate = false);
    void RenderPlayerTracker(Player player, std::pair<Vec2_t, Vec2_t> bounds, bool mate = false);
    void RenderPlayerTracers(Player source, Player player, bool mate = false);

    void RenderSpeed(Player local);
	void RenderCrosshair(Player local);
    void RenderBomb(Player local, Bomb bomb);
    void RenderSpectatorList(std::vector<Player>& players);
};