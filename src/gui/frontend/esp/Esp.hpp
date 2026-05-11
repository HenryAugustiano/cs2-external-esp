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
    ImGuiIO io;
    ImFont* font;
    ImDrawList* d;

    // Temporary storage for ease
    view_matrix_t matrix;

    // dont know where vel_buffer should live - me neither, but doesnt look bad here
    size_t vel_index = 0;
    std::vector<int> vel_buffer;
    float vel_accumulator = 0.0f;
private:
    Esp() {};

    static Esp& GetInstance()
    {
        static Esp i{};
        return i;
    }

    bool InitImpl();
    void RenderImpl();

    void RenderPlayer(Player player, bool mate = false);
    void RenderPlayerBones(Player player, bool mate = false);
    void RenderPlayerBars(Player player, std::pair<Vec2_t, Vec2_t> bounds);
    void RenderPlayerFalgs(Player player, std::pair<Vec2_t, Vec2_t> bounds, bool mate = false);
    void RenderPlayerTracker(Player player, std::pair<Vec2_t, Vec2_t> bounds, bool mate = false);
    void RenderPlayerTracers(Player source, Player player, bool mate = false);

	void RenderCrosshair(Player local);
    void RenderBomb(Player local, Bomb bomb);
    void RenderSpeed(Player local, Globals globals);
    void RenderSpectatorList(std::vector<Player>& players);
};