
#include "test.h"

TEST_CASE("Input Expression", "[ParseKeySequence]") {
  // Empty
  CHECK(parse_input("") == (KeySequence{ }));

  // A has to be pressed.
  // "A"  =>  +A ~A
  CHECK(parse_input("A") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::UpAsync),
  }));

  // A has to be pressed first then B. A can still be hold.
  // "A B"  =>  +A ~A +B ~B
  CHECK(parse_input("A B") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::UpAsync),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::UpAsync),
  }));

  // A has to be pressed first then B. A must not be released in between.
  // "A{B}"  =>  +A +B ~B ~A
  CHECK(parse_input("A{B}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::UpAsync),
    KeyEvent(Key::A, KeyState::UpAsync),
  }));
  CHECK_THROWS(parse_input("{B}"));

  // A has to be pressed first then B, then C. None must be released in between.
  // "A{B{C}}"  =>  +A +B +C ~C ~A ~B
  CHECK(parse_input("A{B{C}}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::UpAsync),
    KeyEvent(Key::B, KeyState::UpAsync),
    KeyEvent(Key::A, KeyState::UpAsync),
  }));

  // A and B have to be pressed together, order does not matter.
  // "(A B)"  =>  *A *B +A +B
  CHECK(parse_input("(A B)") == (KeySequence{
    KeyEvent(Key::A, KeyState::DownAsync),
    KeyEvent(Key::B, KeyState::DownAsync),
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
  }));

  // A has to be pressed first then B and C together. A can be released any time.
  // "A(B C)"  =>  +A ~A *B *C +B +C
  CHECK(parse_input("A(B C)") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::UpAsync),
    KeyEvent(Key::B, KeyState::DownAsync),
    KeyEvent(Key::C, KeyState::DownAsync),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
  }));

  // A has to be pressed first then B, then C. A has to be released last.
  // "A{B C}"  =>  +A +B ~B +C ~A
  CHECK(parse_input("A{B C}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::UpAsync),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::UpAsync),
    KeyEvent(Key::A, KeyState::UpAsync),
  }));

  // A has to be pressed first then B and C together. A has to be released last.
  // "A{(B C)}"  =>  +A *B *C +B +C ~A ~B ~C
  CHECK(parse_input("A{(B C)}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::DownAsync),
    KeyEvent(Key::C, KeyState::DownAsync),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::UpAsync),
    KeyEvent(Key::B, KeyState::UpAsync),
    KeyEvent(Key::A, KeyState::UpAsync),
  }));

  // A and B have to be pressed together, order does not matter. Then C, then D.
  // "(A B){C D}"  =>  *A *B +A +B +C ~C +D ~D ~A ~B
  CHECK(parse_input("(A B){C D}") == (KeySequence{
    KeyEvent(Key::A, KeyState::DownAsync),
    KeyEvent(Key::B, KeyState::DownAsync),
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::UpAsync),
    KeyEvent(Key::D, KeyState::Down),
    KeyEvent(Key::D, KeyState::UpAsync),
    KeyEvent(Key::B, KeyState::UpAsync),
    KeyEvent(Key::A, KeyState::UpAsync),
  }));

  // Not
  CHECK(parse_input("!A") == (KeySequence{
    KeyEvent(Key::A, KeyState::Not),
  }));
  CHECK(parse_input("A !A B") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::UpAsync),
    KeyEvent(Key::A, KeyState::Not),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::UpAsync),
  }));
  CHECK_THROWS(parse_input("!"));
  CHECK_THROWS(parse_input("!(A B)"));
  CHECK_THROWS(parse_input("!A{B}"));
  CHECK_THROWS(parse_input("A{!B}"));

  // Output on release
  CHECK_THROWS(parse_input("A ^ B"));
}

//--------------------------------------------------------------------

TEST_CASE("Output Expression", "[ParseKeySequence]") {
  // Empty
  CHECK(parse_output("") == (KeySequence{ }));

  // Press A.
  // "A"  =>  +A
  CHECK(parse_output("A") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
  }));

  // Press A and then B.
  // "A B"  =>  +A -A +B -B
  CHECK(parse_output("A B") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::Up),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::Up),
  }));

  // Press A and keep hold while pressing B.
  //   "A{B}"  =>  +A +B -B -A
  CHECK(parse_output("A{B}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::Up),
    KeyEvent(Key::A, KeyState::Up),
  }));
  CHECK_THROWS(parse_output("{B}"));

  // Press A and B together, order does not matter.
  //   "(A B)"  =>  +A +B
  CHECK(parse_output("(A B)") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
  }));

  // Press A, B, C together, order does not matter.
  //   "(A B C)"  =>  +A +B +C
  CHECK(parse_output("(A B C)") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
  }));

  // Press A first and then B and C, order does not matter.
  // "A(B C)"  =>  +A -A +B +C -C -B
  CHECK(parse_output("A(B C)") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::Up),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::Up),
    KeyEvent(Key::B, KeyState::Up),
  }));

  // Press A and keep hold while pressing B and then C.
  // "A{B C}"  =>  +A +B B- +C -C -A
  CHECK(parse_output("A{B C}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::Up),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::Up),
    KeyEvent(Key::A, KeyState::Up),
  }));

  // Press A and keep hold while pressing B and C, order does not matter.
  // "A{(B C)}"  =>  +A +B +C -C -B -A
  CHECK(parse_output("A{(B C)}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::Up),
    KeyEvent(Key::B, KeyState::Up),
    KeyEvent(Key::A, KeyState::Up),
  }));

  // Press A and B together, order does not matter,
  // keep hold while pressing C and then D.
  // "(A B){C D}"  =>  +A +B +C -C +D -D -B -A
  CHECK(parse_output("(A B){C D}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::Up),
    KeyEvent(Key::D, KeyState::Down),
    KeyEvent(Key::D, KeyState::Up),
    KeyEvent(Key::B, KeyState::Up),
    KeyEvent(Key::A, KeyState::Up),
  }));

  // Press A, B and C together.
  // "A{B{C}}"  =>  +A +B +C -C -B -A
  CHECK(parse_output("A{B{C}}") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::C, KeyState::Down),
    KeyEvent(Key::C, KeyState::Up),
    KeyEvent(Key::B, KeyState::Up),
    KeyEvent(Key::A, KeyState::Up),
  }));

  // Not
  CHECK(parse_output("!A") == (KeySequence{
    KeyEvent(Key::A, KeyState::Not),
  }));

  // Output on release
  CHECK(parse_output("A ^ B") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::Up),
    KeyEvent(Key::none, KeyState::OutputOnRelease),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::Up),
  }));
  CHECK(parse_output("^ A B") == (KeySequence{
    KeyEvent(Key::none, KeyState::OutputOnRelease),
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::Up),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::Up),
  }));
  CHECK(parse_output("A B ^") == (KeySequence{
    KeyEvent(Key::A, KeyState::Down),
    KeyEvent(Key::A, KeyState::Up),
    KeyEvent(Key::B, KeyState::Down),
    KeyEvent(Key::B, KeyState::Up),
    KeyEvent(Key::none, KeyState::OutputOnRelease),
  }));
  CHECK(parse_output("^") == (KeySequence{
    KeyEvent(Key::none, KeyState::OutputOnRelease),
  }));
  CHECK_THROWS(parse_output("A ^ B ^ C"));
  CHECK_THROWS(parse_output("^ A ^ B"));
  CHECK_THROWS(parse_output("(A ^ B)"));
  CHECK_THROWS(parse_output("A{^ B}"));
  CHECK_THROWS(parse_output("A^{B}"));
}

//--------------------------------------------------------------------
