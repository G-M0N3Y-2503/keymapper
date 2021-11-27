
#include "Stage.h"
#include <cassert>
#include <algorithm>
#include <iterator>

namespace {
  KeySequence::const_iterator find_key(const KeySequence& sequence, KeyCode key) {
    return std::find_if(begin(sequence), end(sequence),
      [&](const auto& ev) { return ev.key == key; });
  }

  template<typename It, typename T>
  bool contains(It begin, It end, const T& v) {
    return std::find(begin, end, v) != end;
  }

  std::vector<MappingOverrideSet> sort(
      std::vector<MappingOverrideSet>&& override_sets) {
    // sort overrides sets by index
    for (auto& override_set : override_sets)
      std::sort(begin(override_set), end(override_set));
    return std::move(override_sets);
  }

  bool has_non_optional(const KeySequence& sequence) {
    return std::find_if(begin(sequence), end(sequence),
      [](const KeyEvent& e) {
        return (e.state == KeyState::Up || e.state == KeyState::Down);
      }) != end(sequence);
  }
} // namespace

bool operator<(const MappingOverride& a, int mapping_index) {
  return (a.mapping_index < mapping_index);
}

bool operator<(const MappingOverride& a, const MappingOverride& b) {
  return (a.mapping_index < b.mapping_index);
}

Stage::Stage(std::vector<Mapping> mappings,
             std::vector<MappingOverrideSet> override_sets)
  : m_mappings(std::move(mappings)),
    m_override_sets(sort(std::move(override_sets))) {
}

const std::vector<Mapping>& Stage::mappings() const {
  return m_mappings;
}

const std::vector<MappingOverrideSet>& Stage::override_sets() const {
  return m_override_sets;
}

void Stage::activate_override_set(int index) {
  m_active_override_set = (index < 0 || index >=
    static_cast<int>(m_override_sets.size()) ?
    nullptr : &m_override_sets[static_cast<size_t>(index)]);
}

KeySequence Stage::update(const KeyEvent event) {
  apply_input(event);
  return std::move(m_output_buffer);
}

void Stage::reuse_buffer(KeySequence&& buffer) {
  m_output_buffer = std::move(buffer);
  m_output_buffer.clear();
}

void Stage::validate_state(const std::function<bool(KeyCode)>& is_down) {
  m_sequence_might_match = false;

  m_sequence.erase(
    std::remove_if(begin(m_sequence), end(m_sequence),
      [&](const KeyEvent& event) {
        return !is_virtual_key(event.key) && !is_down(event.key);
      }),
    end(m_sequence));

  m_output_down.erase(
    std::remove_if(begin(m_output_down), end(m_output_down),
      [&](const OutputDown& output) {
        return !is_down(output.trigger);
      }),
    end(m_output_down));
}

std::pair<MatchResult, const Mapping*> Stage::find_mapping(
    const KeySequence& sequence, bool accept_might_match) const {
  for (const auto& mapping : m_mappings) {
    const auto result = m_match(mapping.input, sequence);

    if (accept_might_match && result == MatchResult::might_match)
      return { result, &mapping };

    if (result == MatchResult::match)
      return { result, &mapping };
  }
  return { MatchResult::no_match, nullptr };
}

void Stage::apply_input(const KeyEvent event) {
  assert(event.state == KeyState::Down ||
         event.state == KeyState::Up);

  if (event.state == KeyState::Down) {
    // merge key repeats
    auto it = find_key(m_sequence, event.key);
    if (it != end(m_sequence)) {
      const auto is_repeat = !std::count(m_sequence.begin(), m_sequence.end(),
        KeyEvent{ event.key, KeyState::Up });
      if (is_repeat)
        m_sequence.erase(it);
    }
  }
  m_sequence.push_back(event);

  if (event.state == KeyState::Up) {
    // release output when triggering input was released
    release_triggered(event.key);

    // remove from sequence
    // except when it was already used for a might match
    if (!m_sequence_might_match) {
      const auto it = find_key(m_sequence, event.key);
      assert(it != end(m_sequence));
      if (it->state == KeyState::DownMatched)
        m_sequence.erase(it);
    }
  }

  for (auto& output : m_output_down)
    output.suppressed = false;

  m_sequence_might_match = false;
  while (has_non_optional(m_sequence)) {
    // find first mapping which matches or might match sequence
    const auto [result, mapping] = find_mapping(m_sequence, true);

    if (result == MatchResult::might_match) {
      // hold back sequence when something might match
      m_sequence_might_match = true;
      break;
    }

    if (result == MatchResult::match) {
      apply_output(get_output(*mapping));

      // release new output when triggering input was released
      if (event.state == KeyState::Up)
        release_triggered(event.key);

      finish_sequence();
      break;
    }

    // when no match was found, forward beginning of sequence
    forward_from_sequence();
  }
}

void Stage::release_triggered(KeyCode key) {
  const auto it = std::stable_partition(begin(m_output_down), end(m_output_down),
    [&](const auto& k) { return k.trigger != key; });
  std::for_each(
    std::make_reverse_iterator(end(m_output_down)),
    std::make_reverse_iterator(it),
    [&](const auto& k) {
      if (!k.temporarily_released)
        m_output_buffer.push_back({ k.key, KeyState::Up });
    });
  m_output_down.erase(it, end(m_output_down));
}

const KeySequence& Stage::get_output(const Mapping& mapping) const {
  // look for override
  if (m_active_override_set) {
    const auto& override_set = *m_active_override_set;
    const auto index = static_cast<int>(std::distance(m_mappings.data(), &mapping));
    auto it = std::lower_bound(begin(override_set), end(override_set), index);
    if (it != end(override_set) && it->mapping_index == index)
      return it->output;
  }
  return mapping.output;
}

void Stage::toggle_virtual_key(KeyCode key) {
  auto it = find_key(m_sequence, key);
  if (it != cend(m_sequence))
    m_sequence.erase(it);
  else
    m_sequence.emplace_back(key, KeyState::Down);
}

void Stage::output_current_sequence(const KeySequence& expression,
    KeyState state, KeyCode trigger) {
  for (const auto& event : m_sequence)
    if (event.state != KeyState::DownMatched) {
      const auto it = find_key(expression, event.key);
      if (it == expression.end() || it->state != KeyState::Not)
        update_output({ event.key, state }, trigger);
    }
}

void Stage::apply_output(const KeySequence& expression) {
  for (const auto& event : expression)
    if (is_virtual_key(event.key)) {
      if (event.state == KeyState::Down)
        toggle_virtual_key(event.key);
    }
    else if (event.key == any_key) {
      output_current_sequence(expression, event.state, m_sequence.back().key);
    }
    else {
      update_output(event, m_sequence.back().key);
    }
}

void Stage::forward_from_sequence() {
  for (auto it = begin(m_sequence); it != end(m_sequence); ++it) {
    auto& event = *it;
    if (event.state == KeyState::Down || event.state == KeyState::DownMatched) {
      const auto up = std::find(it, end(m_sequence),
        KeyEvent{ event.key, KeyState::Up });
      if (up != end(m_sequence)) {
        // erase Down and Up
        update_output(event, event.key);
        release_triggered(event.key);
        m_sequence.erase(up);
        m_sequence.erase(it);
        return;
      }
      else if (event.state == KeyState::Down) {
        // no Up yet, convert to DownMatched
        update_output(event, event.key);
        event.state = KeyState::DownMatched;
        return;
      }
    }
    else if (event.state == KeyState::Up) {
      // remove remaining Up
      release_triggered(event.key);
      m_sequence.erase(it);
      return;
    }
  }
}

void Stage::update_output(const KeyEvent& event, KeyCode trigger) {
  const auto it = std::find_if(begin(m_output_down), end(m_output_down),
    [&](const OutputDown& down_key) { return down_key.key == event.key; });

  switch (event.state) {
    case KeyState::Up: {
      if (it != end(m_output_down)) {
        if (it->pressed_twice) {
          // try to remove current down
          auto it2 = find_key(m_output_buffer, event.key);
          if (it2 != m_output_buffer.end())
            m_output_buffer.erase(it2);

          it->pressed_twice = false;
        }
        else {
          m_output_down.erase(it);
          m_output_buffer.push_back(event);
        }
      }
      break;
    }

    case KeyState::Not: {
      // make sure it is released in output
      if (it != end(m_output_down)) {
        if (!it->temporarily_released) {
          m_output_buffer.emplace_back(event.key, KeyState::Up);
          it->temporarily_released = true;
        }
        it->suppressed = true;
      }
      break;
    }

    case KeyState::Down: {
      // reapply temporarily released
      auto reapplied = false;
      for (auto& output : m_output_down)
        if (output.temporarily_released && !output.suppressed) {
          output.temporarily_released = false;
          m_output_buffer.emplace_back(output.key, KeyState::Down);
          reapplied = true;
        }

      if (it == end(m_output_down)) {
        m_output_down.push_back({ event.key, trigger, false, false });
      }
      else {
        // already pressed, but something was reapplied in the meantime?
        if (reapplied)
          m_output_buffer.emplace_back(event.key, KeyState::Up);

        it->temporarily_released = false;
        it->pressed_twice = true;
      }
      m_output_buffer.emplace_back(event.key, KeyState::Down);
      break;
    }

    case KeyState::OutputOnRelease: {
      m_output_buffer.emplace_back(event.key, event.state);
      break;
    }

    case KeyState::DownMatched:
      // ignored
      break;

    case KeyState::UpAsync:
    case KeyState::DownAsync:
      assert(!"unreachable");
      break;
  }
}

void Stage::finish_sequence() {
  for (auto it = begin(m_sequence); it != end(m_sequence); ) {
    if (it->state == KeyState::Down || it->state == KeyState::DownMatched) {
      // convert to DownMatched when no Up follows, otherwise also erase
      if (!contains(it, end(m_sequence), KeyEvent{ it->key, KeyState::Up })) {
        it->state = KeyState::DownMatched;
        ++it;
        continue;
      }
    }
    it = m_sequence.erase(it);
  }
}
