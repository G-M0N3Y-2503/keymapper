
#include "ClientState.h"
#include "common/output.h"

extern bool execute_terminal_command(const std::string& command);

void ClientState::on_execute_action_message(int triggered_action) {
  const auto& actions = m_config_file.config().actions;
  if (triggered_action >= 0 &&
      triggered_action < static_cast<int>(actions.size())) {
    const auto& action = actions[triggered_action];
    const auto& command = action.terminal_command;
    const auto succeeded = execute_terminal_command(command);
    verbose("Executing terminal command '%s'%s", 
      command.c_str(), succeeded ? "" : " failed");
  }
}

void ClientState::on_virtual_key_state_message(Key key, KeyState state) {
  m_control.on_virtual_key_state_changed(key, state);
}

void ClientState::on_set_virtual_key_state_message(Key key, KeyState state) {
  m_server.send_set_virtual_key_state(key, state);
}

const std::filesystem::path& ClientState::config_filename() const {
  return m_config_file.filename();
}

bool ClientState::is_focused_window_inaccessible() const {
  return m_focused_window.is_inaccessible();
}

std::optional<Socket> ClientState::connect_server() {
  verbose("Connecting to keymapperd");
  if (m_server.connect())
    return m_server.socket();
  error("Connecting to keymapperd failed");
  return { };
}

bool ClientState::read_server_messages(std::optional<Duration> timeout) {
  return m_server.read_messages(*this, timeout);
}

void ClientState::on_server_disconnected() {
  m_control.reset();
  m_server.disconnect();
  m_focused_window.shutdown();
}

bool ClientState::load_config(std::filesystem::path filename) {
  return m_config_file.load(std::move(filename));
}

bool ClientState::update_config(bool check_modified) {
  if (!m_config_file.update(check_modified))
    return false;
  message("Configuration updated");
  return true;
}

bool ClientState::send_config() {
  verbose("Sending configuration");
  if (!m_server.send_config(m_config_file.config())) {
    error("Sending configuration failed");
    return false;
  }

  m_control.set_virtual_key_aliases(
    m_config_file.config().virtual_key_aliases);
  
  clear_active_contexts();
  update_active_contexts();
  return send_active_contexts();
}

bool ClientState::send_validate_state() {
  return m_server.send_validate_state();
}

bool ClientState::initialize_contexts() {
  verbose("Initializing focused window detection:");
  return m_focused_window.initialize();
}

void ClientState::clear_active_contexts() {
  m_active_contexts.clear();
}

bool ClientState::update_active_contexts() {
  if (m_focused_window.update()) {
    verbose("Detected focused window changed:");
    verbose("  class = '%s'", m_focused_window.window_class().c_str());
    verbose("  title = '%s'", m_focused_window.window_title().c_str());
    verbose("  path = '%s'", m_focused_window.window_path().c_str());
  }
  else {
    if (!m_active_contexts.empty())
      return false;
  }

  m_new_active_contexts.clear();
  const auto& contexts = m_config_file.config().contexts;
  for (auto i = 0; i < static_cast<int>(contexts.size()); ++i)
    if (contexts[i].matches(
        m_focused_window.window_class(),
        m_focused_window.window_title(),
        m_focused_window.window_path()))
      m_new_active_contexts.push_back(i);

  if (m_new_active_contexts != m_active_contexts) {
    verbose("Active contexts updated");
    m_active_contexts.swap(m_new_active_contexts);
    return true;
  }
  return false;
}

bool ClientState::send_active_contexts() {
  verbose("Sending active contexts (%u)", m_active_contexts.size());
  return m_server.send_active_contexts(m_active_contexts);
}

std::optional<Socket> ClientState::listen_for_control_connections() {
  return m_control.listen();
}

std::optional<Socket> ClientState::accept_control_connection() {
  return m_control.accept();
}

void ClientState::read_control_messages() {
  m_control.read_messages(*this);
}
