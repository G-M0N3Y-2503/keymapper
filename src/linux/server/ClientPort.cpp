
#include "ClientPort.h"
#include "runtime/Stage.h"
#include "../common.h"
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <cerrno>

namespace {
  bool read_key_sequence(int fd, KeySequence* sequence) {
    sequence->clear();

    auto size = uint8_t{ };
    if (!read(fd, &size))
      return false;

    auto event = KeyEvent{ };
    for (auto i = 0; i < size; ++i) {
      if (!read(fd, &event.key) ||
          !read(fd, &event.state))
        return false;
      sequence->push_back(event);
    }
    return true;
  }

  std::unique_ptr<Stage> read_config(int fd) {
    // receive commands
    auto commmand_count = uint16_t{ };
    if (!read(fd, &commmand_count))
      return nullptr;

    auto mappings = std::vector<Mapping>();
    auto input = KeySequence();
    auto output = KeySequence();
    for (auto i = 0; i < commmand_count; ++i) {
      if (!read_key_sequence(fd, &input) ||
          !read_key_sequence(fd, &output))
        return nullptr;
      mappings.push_back({ std::move(input), std::move(output) });
    }

    // receive mapping overrides
    auto override_sets_count = uint16_t{ };
    if (!read(fd, &override_sets_count))
      return nullptr;

    auto override_sets = std::vector<MappingOverrideSet>();
    for (auto i = 0; i < override_sets_count; ++i) {
      override_sets.emplace_back();
      auto& overrides = override_sets.back();

      auto overrides_count = uint16_t{ };
      if (!read(fd, &overrides_count))
        return nullptr;

      for (auto j = 0; j < overrides_count; ++j) {
        auto mapping_index = uint16_t{ };
        if (!read(fd, &mapping_index) ||
            !read_key_sequence(fd, &output))
          return nullptr;
        overrides.push_back({ mapping_index, std::move(output) });
      }
    }

    return std::make_unique<Stage>(
      std::move(mappings), std::move(override_sets));
  }
} // namespace

ClientPort::~ClientPort() {
  if (m_socket_fd >= 0)
    ::close(m_socket_fd);
}

bool ClientPort::initialize(const char* ipc_id) {
  m_socket_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
  if (m_socket_fd < 0)
    return false;

  auto addr = sockaddr_un{ };
  addr.sun_family = AF_UNIX;
  ::strncpy(&addr.sun_path[1], ipc_id, sizeof(addr.sun_path) - 2);
  if (::bind(m_socket_fd, reinterpret_cast<sockaddr*>(&addr),
      sizeof(sockaddr_un)) != 0)
    return false;

  if (::listen(m_socket_fd, 0) != 0)
    return false;

  return true;
}

std::unique_ptr<Stage> ClientPort::receive_config() {
  m_client_fd = ::accept(m_socket_fd, nullptr, nullptr);
  if (m_client_fd < 0)
    return nullptr;

  auto message_type = MessageType{ };
  ::read(m_client_fd, &message_type);
  if (message_type == MessageType::update_configuration)
    return ::read_config(m_client_fd);

  return nullptr;
}

bool ClientPort::receive_updates(std::unique_ptr<Stage>& stage) {
  const auto timeout_ms = 0;
  if (!select(m_client_fd, timeout_ms))
    return true;

  auto message_type = MessageType{ };
  ::read(m_client_fd, &message_type);

  if (message_type == MessageType::update_configuration) {
    if (auto new_stage = ::read_config(m_client_fd)) {
      stage = std::move(new_stage);
      return true;
    }
  }
  else if (message_type == MessageType::set_active_override_set) {
    auto activate_override_set = uint32_t{ };
    if (read(m_client_fd, &activate_override_set))
      stage->activate_override_set(activate_override_set);
    return true;
  }
  return false;
}

bool ClientPort::send_triggered_action(int action) {
  return send(m_client_fd, static_cast<uint32_t>(action));
}

void ClientPort::disconnect() {
  if (m_client_fd >= 0) {
    ::close(m_client_fd);
    m_client_fd = -1;
  }
}
