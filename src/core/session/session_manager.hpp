#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include "core/core_pch.hpp"
#include "core/session/session_base.hpp"

namespace core {

class SessionManager {
public:
    void Add(const std::shared_ptr<SessionBase>& session);
    void Remove(const std::shared_ptr<SessionBase>& session);

    std::shared_ptr<SessionBase> FindBySessionId(uint64_t id) const;
    void FuncToAll(const std::function<void(std::shared_ptr<SessionBase>)>& func) const;

private:
    mutable std::mutex _mutex;
    std::unordered_map<uint64_t, std::shared_ptr<SessionBase>> _sessions;
};

}
#endif