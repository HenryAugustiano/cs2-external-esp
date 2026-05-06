#include "Engine.hpp"

#include "core/offsets/Dumper.hpp"
#include "core/engine/cache/Cache.hpp"

bool Engine::Init() {
    return GetInstance().InitImpl();
}

ProcessModule Engine::GetClient() {
    return GetInstance().client;
}

ProcessModule Engine::GetEngine() {
    return GetInstance().engine;
}

std::shared_ptr<pProcess> Engine::GetProcess() {
    return GetInstance().process;
}

bool Engine::InitImpl() {
    process = std::make_shared<pProcess>();

    if (!this->AwaitProcess()) {
        LOGF(FATAL, "Could not find process, please make sure the game is open");
        return false;
    }

    if (!this->AwaitModules()) {
        LOGF(FATAL, "Game took too long to load, please open me again once its fully loaded");
        return false;
    }

    if (!Dumper::Init()) {
        LOGF(FATAL, "Failed to dump game offsets");
        return false;
    }

    if (!Config::Read())
        LOGF(WARNING, "Failed to parse config, using default values");

#ifdef _DEBUG
    if (!cfg::settings::console)
        LogHelper::Free();
#endif

    std::thread(&Engine::Thread, this).detach();

    LOGF(INFO, "Successfully initialized engine...");
    return true;
}

void Engine::Thread() {
    // TODO: Check build number 
    // uintptr_t number = process->read<uintptr_t>(base_engine.base + offsets::buildNumber);

    while (true) {
        auto start = steady_clock::now();

        static bool was_pressed = false;
        bool is_pressed = (GetAsyncKeyState(cfg::triggerbot::hotkey) & 0x8000);

        if (is_pressed && !was_pressed) {
            cfg::triggerbot::enabled = !cfg::triggerbot::enabled;
            LOGF(INFO, "Triggerbot toggled: {}", cfg::triggerbot::enabled ? "ENABLED" : "DISABLED");
        }
        was_pressed = is_pressed;

        Cache::Refresh();
        
        if (cfg::triggerbot::enabled) {
            auto snapshot = Cache::CopySnapshot();
            auto local = snapshot.local;

            if (local.alive && local.crosshair_id > 0) {
                for (const auto& player : snapshot.players) {
                    int player_pawn_index = (player.pawn_controller_addr & 0x7FFF);
                    if (player_pawn_index == local.crosshair_id) { 
                        if (player.alive && player.team != local.team) {
                            if (cfg::triggerbot::delay > 0)
                                std::this_thread::sleep_for(std::chrono::milliseconds(cfg::triggerbot::delay));

                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                        }
                        break;
                    }
                }
            }
        }

        if (cfg::settings::free_cpu)
            std::this_thread::sleep_until(start + 1ms);
    }
}

bool Engine::AwaitProcess() {
    if (!process || process->handle_) // Process not initialized, or already attached
        return false;

    do {
        if (process->AttachProcess("cs2.exe"))
            break;

        if (process->pid_ && !process->handle_) {
            LOGF(FATAL, "Insufficient permissions to open a handle to the process. Try running as Administrator.");
            return false;
        }

        static int attempts = 0;

        if (!attempts)
            LOGF(INFO, "Waiting 50s for the game to open...");

        if (attempts > 10)
            return false;
        attempts++;

        std::this_thread::sleep_for(5s);
    } while (true);

    return true;
}

bool Engine::AwaitModules() {
    if (!process || !process->handle_) // Process not initialized, or not attached
        return false;

    LOGF(INFO, "Waiting for the game to open...");

    do {
        this->client = process->GetModule("client.dll");
        this->engine = process->GetModule("engine2.dll");

        if (this->client.base && this->engine.base)
            break;

        static int attempts = 0;
        if (attempts > 10)
            return false;
        attempts++;

        std::this_thread::sleep_for(5s);
    } while (true);

    return true;
}
