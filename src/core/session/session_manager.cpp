#include "core/session/session_manager.hpp"

namespace core {

void SessionManager::Add(const std::shared_ptr<SessionBase>& session) {
    std::lock_guard lock(_mutex);
    _sessions[session->GetSessionId()] = session;
}

void SessionManager::Remove(const std::shared_ptr<SessionBase>& session) {
    std::lock_guard lock(_mutex);
    _sessions.erase(session->GetSessionId());
}

std::shared_ptr<SessionBase> SessionManager::FindBySessionId(const uint64_t id) const {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(id);
    if (it == _sessions.end()) return nullptr;

    return it->second;
}

void SessionManager::FuncToAll(
    const std::function<void(std::shared_ptr<SessionBase>)>& func) const {
    std::lock_guard lock(_mutex);
    for (const auto& [fst, snd] : _sessions) {
        func(snd);
    }
}

}  // namespace core
